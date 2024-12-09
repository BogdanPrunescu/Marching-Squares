#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <omp.h>

#define CONTOUR_CONFIG_COUNT    16
#define FILENAME_MAX_SIZE       50
#define STEP                    8
#define SIGMA                   200
#define RESCALE_X               4096
#define RESCALE_Y               4096

#define CLAMP(v, min, max) if(v < min) { v = min; } else if(v > max) { v = max; }

ppm_image* copy_image(ppm_image* image) {

    ppm_image *copy = (ppm_image *)malloc(sizeof(ppm_image));
    if (!copy) {
        fprintf(stderr, "Unable to allocate memory\n");
        exit(1);
    }
    copy->x = image->x;
    copy->y = image->y;

    copy->data = (ppm_pixel*)malloc(image->x * image->y * sizeof(ppm_pixel));

    if (!copy) {
        fprintf(stderr, "Unable to allocate memory\n");
        exit(1);
    }
    int i, j;

    #pragma omp parallel for private(i, j) collapse(2) schedule(auto)
    for (i = 0; i < copy->x; i++) {
        for (j = 0; j < copy->y; j++) {
            copy->data[i * copy->y + j].red = image->data[i * image->y + j].red;
            copy->data[i * copy->y + j].green = image->data[i * image->y + j].green;
            copy->data[i * copy->y + j].blue = image->data[i * image->y + j].blue;
        }
    }

    return copy;
}

// Initializes the contour map from files in the './contours' directory
ppm_image **init_contour_map() {
    ppm_image **map = (ppm_image **)malloc(CONTOUR_CONFIG_COUNT * sizeof(ppm_image *));
    if (!map) {
        fprintf(stderr, "Unable to allocate memory\n");
        exit(1);
    }

    for (int i = 0; i < CONTOUR_CONFIG_COUNT; i++) {
        char filename[FILENAME_MAX_SIZE];
        sprintf(filename, "./contours/%d.ppm", i);
        map[i] = read_ppm(filename);
    }

    return map;
}

// Samples the grid for the local portion of the image
unsigned char **sample_grid(ppm_image *local_image, int step_x, int step_y, unsigned char sigma, int rank, int size, int global_width) {
    int p = local_image->x / step_x;
    int q = local_image->y / step_y;
    
    unsigned char **grid = (unsigned char **)malloc((p + 1) * sizeof(unsigned char *));
    if (!grid) {
        fprintf(stderr, "Unable to allocate memory\n");
        exit(1);
    }
    int i, j;
    
    #pragma omp parallel for private(i) schedule(auto)
    for (i = 0; i <= p; i++) {
        grid[i] = (unsigned char *)malloc((q + 1) * sizeof(unsigned char));
        if (!grid[i]) {
            fprintf(stderr, "Unable to allocate memory\n");
            exit(1);
        }
    }

    #pragma omp parallel for private(i, j) collapse(2) schedule(auto)
    for (i = 0; i < p; i++) {
        for (j = 0; j < q; j++) {
            ppm_pixel curr_pixel = local_image->data[i * step_x * local_image->y + j * step_y];
            unsigned char curr_color = (curr_pixel.red + curr_pixel.green + curr_pixel.blue) / 3;
            
            grid[i][j] = (curr_color > sigma) ? 0 : 1;
        }
    }
    grid[p][q] = 0;
    
    // last sample points have no neighbors below / to the right, so we use pixels on the
    // last row / column of the input image for them
    #pragma omp parallel for private(i) schedule(auto)
    for (i = 0; i < p; i++) {
        ppm_pixel curr_pixel = local_image->data[i * step_x * local_image->y + global_width - 1];

        unsigned char curr_color = (curr_pixel.red + curr_pixel.green + curr_pixel.blue) / 3;
        
        if (curr_color > sigma) {
            grid[i][q] = 0;
        } else {
            grid[i][q] = 1;
        }
    }
    
    if (rank == size - 1) {
        #pragma omp parallel for private(j) schedule(auto)
        for (j = 0; j < q; j++) {
            ppm_pixel curr_pixel = local_image->data[(local_image->x - 1) * local_image->y + j * step_y];

            unsigned char curr_color = (curr_pixel.red + curr_pixel.green + curr_pixel.blue) / 3;
            if (curr_color > sigma) {
                grid[p][j] = 0;
            } else {
                grid[p][j] = 1;
            }
        }
    }

    return grid;
}

// Updates a region of the image with the corresponding contour pixels
void update_image(ppm_image *local_image, ppm_image *contour, int local_x, int y, int rank, int type, int k, int ii, int jj) {
    int i, j;

    #pragma omp parallel for private(i, j) collapse(2) schedule(auto)
    for (i = 0; i < contour->x; i++) {
        for (j = 0; j < contour->y; j++) {
            int contour_pixel_index = contour->x * i + j;
            int local_pixel_index = (local_x + i) * local_image->y + y + j;
            local_image->data[local_pixel_index].red = contour->data[contour_pixel_index].red;
            local_image->data[local_pixel_index].green = contour->data[contour_pixel_index].green;
            local_image->data[local_pixel_index].blue = contour->data[contour_pixel_index].blue;
        }
    }
}

// Performs the marching squares algorithm on the local image data
void march(ppm_image *local_image, unsigned char **grid, ppm_image **contour_map, int step_x, int step_y, int rank, int procs) {
    int p = local_image->x / step_x;
    int q = local_image->y / step_y;
    int i, j;

    #pragma omp parallel for private(i, j) collapse(2) schedule(auto)
    for (i = 0; i < p - (rank != procs - 1); i++) {
        for (j = 0; j < q; j++) {
            int k = 8 * (grid[i][j] - '\0') + 4 * (grid[i][j + 1] - '\0') + 
                              2 * (grid[i + 1][j + 1] - '\0') + 1 * (grid[i + 1][j] - '\0');
            update_image(local_image, contour_map[k], i * step_x, j * step_y, rank, 0, k, i, j);
        }
    }

    if (rank != procs - 1) {
        MPI_Recv(grid[p], q + 1, MPI_BYTE, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        #pragma omp parallel for private(j)
        for (j = 0; j < q; j++) {
            int k = 8 * (grid[p - 1][j] - '\0') + 4 * (grid[p - 1][j + 1] - '\0') + 
                                2 * (grid[p][j + 1] - '\0') + 1 * (grid[p][j] - '\0');
            update_image(local_image, contour_map[k], (p - 1) * step_x, j * step_y, rank, 1, k, p - 1, j);
        }
    }

    if (rank != 0) {
        MPI_Send(grid[0], q + 1, MPI_BYTE, rank - 1, 0, MPI_COMM_WORLD);
    }

}

// Frees the resources allocated for the grid and contour map
void free_resources(ppm_image *local_image, ppm_image **contour_map, unsigned char **grid) {
    for (int i = 0; i < CONTOUR_CONFIG_COUNT; i++) {
        free(contour_map[i]->data);
        free(contour_map[i]);
    }
    free(contour_map);

    // if (grid) {
    //     for (int i = 0; i <= local_image->x / STEP; i++) {
    //         free(grid[i]);
    //     }
    //     free(grid);
    // }

    free(local_image->data);
    free(local_image);
}

// Rescale local image
ppm_image *rescale_image(ppm_image *global_image, ppm_image *local_image, int* global_width, int* global_height, int nr_procs, int rank) {
    uint8_t sample[3];

    if (*global_width <= RESCALE_X && *global_height <= RESCALE_Y) {
        return local_image;
    }

    *global_width = RESCALE_X;
    *global_height = RESCALE_Y;

    ppm_image *new_local_image = (ppm_image *)malloc(sizeof(ppm_image));
    if (!new_local_image) {
        fprintf(stderr, "Unable to allocate memory\n");
        exit(1);
    }

    int rows_per_proc = RESCALE_X / nr_procs;
    new_local_image->x = rows_per_proc;
    new_local_image->y = RESCALE_Y;
    
    new_local_image->data = (ppm_pixel *)malloc(rows_per_proc * new_local_image->y * sizeof(ppm_pixel));
    if (!new_local_image->data) {
        fprintf(stderr, "Unable to allocate memory\n");
        exit(1);
    }
    int i, j;

    for (i = 0; i < new_local_image->x; i++) {
        for (j = 0; j < new_local_image->y; j++) {
            float u = (float)(i + rank * new_local_image->x) / (float)(RESCALE_X - 1);
            float v = (float)j / (float)(new_local_image->y - 1);
            sample_bicubic(global_image, u, v, sample);

            new_local_image->data[i * new_local_image->y + j].red = sample[0];
            new_local_image->data[i * new_local_image->y + j].green = sample[1];
            new_local_image->data[i * new_local_image->y + j].blue = sample[2];
        }
    }
    
    free(local_image->data);
    free(local_image);

    return new_local_image;
}

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if (argc < 4) {
        if (rank == 0) {
            fprintf(stderr, "Usage: ./tema1 <in_file> <out_file> <P>\n");
        }
        MPI_Finalize();
        return 1;
    }

    omp_set_num_threads(atoi(argv[3]));

    ppm_image *global_image = NULL;
    int global_width = 0, global_height = 0;

    // Rank 0 reads the image and determines its dimensions
        global_image = read_ppm(argv[1]);
        global_width = global_image->x;
        global_height = global_image->y;
  
        if (global_width % size != 0) {
            fprintf(stderr, "Error: Image width must be divisible by the number of processes.\n");
            MPI_Finalize();
            return 1;
        }

    // Calculate rows to be handled by each process
    int rows_per_proc = global_width / size;

    // Allocate local image for each process
    ppm_image *local_image = (ppm_image *)malloc(sizeof(ppm_image));
    local_image->x = rows_per_proc;
    local_image->y = global_height;
    local_image->data = (ppm_pixel *)malloc(rows_per_proc * global_height * sizeof(ppm_pixel));

    

    // Scatter the image data from rank 0 to all processes
    MPI_Scatter((rank == 0) ? global_image->data : NULL,
                rows_per_proc * global_height * sizeof(ppm_pixel),
                MPI_BYTE,
                local_image->data,
                rows_per_proc * global_height * sizeof(ppm_pixel),
                MPI_BYTE,
                0,
                MPI_COMM_WORLD);

    // Initialize contour map
    ppm_image **contour_map = init_contour_map();
    
    // Rescale the local image
    local_image = rescale_image(global_image, local_image, &global_width, &global_height, size, rank);

    MPI_Barrier(MPI_COMM_WORLD);
    // Perform processing for each sigma value
    ppm_image* image_copy = copy_image(local_image);

    ppm_image *final_image = NULL;
    if (rank == 0) {
        final_image = malloc(sizeof(*final_image));
        final_image->data = malloc(global_width * local_image->y * sizeof(*final_image->data));
        final_image->x = global_width;
        final_image->y = global_height;
    }

    int sig = 250;
    for (int sig = 10; sig <= 255; sig+=3) {
        unsigned char **grid = sample_grid(local_image, STEP, STEP, sig, rank, size, global_width);
        
        MPI_Barrier(MPI_COMM_WORLD);
        
        march(image_copy, grid, contour_map, STEP, STEP, rank, size);

        MPI_Barrier(MPI_COMM_WORLD);

        MPI_Gather(image_copy->data,
               image_copy->x * image_copy->y * sizeof(ppm_pixel),
               MPI_BYTE,
               (rank == 0) ? final_image->data : NULL,
               image_copy->x * image_copy->y * sizeof(ppm_pixel),
               MPI_BYTE,
               0,
               MPI_COMM_WORLD);

        // if (rank == 0) {
        //     write_ppm(final_image, argv[2]);
        // }
    }

    // // Free resources
    free_resources(local_image, contour_map, NULL);

    MPI_Finalize();
    return 0;
}
