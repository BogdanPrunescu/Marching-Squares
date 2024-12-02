#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define CONTOUR_CONFIG_COUNT    16
#define FILENAME_MAX_SIZE       50
#define STEP                    8
#define SIGMA                   200
#define RESCALE_X               4096
#define RESCALE_Y               4096

#define CLAMP(v, min, max) if(v < min) { v = min; } else if(v > max) { v = max; }

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


// Rescale local image
ppm_image *rescale_image(ppm_image *local_image, int* global_width, int* global_height, int nr_procs) {
    uint8_t sample[3];

    if (*global_width <= RESCALE_X && *global_height <= RESCALE_Y) {
        return local_image;
    }
    *global_width = RESCALE_X;
    *global_height = RESCALE_Y;
    ppm_image *new_local_image = (ppm_image *)malloc(sizeof(ppm_image));
    int rows_per_proc = RESCALE_X / nr_procs;
    new_local_image->x = rows_per_proc;
    new_local_image->y = RESCALE_Y;
    new_local_image->data = (ppm_pixel *)malloc(new_local_image->x * new_local_image->y * sizeof(ppm_pixel));

    for (int i = 0; i < new_local_image->x; i++) {
        for (int j = 0; j < new_local_image->y; j++) {
            float u = (float)i / (float)(new_local_image->x - 1);
            float v = (float)j / (float)(new_local_image->y - 1);
            sample_bicubic(local_image, u, v, sample);
            new_local_image->data[i * new_local_image->y + j].red = sample[0];
            new_local_image->data[i * new_local_image->y + j].green = sample[1];
            new_local_image->data[i * new_local_image->y + j].blue = sample[2];
        }
    }
    free(local_image->data);
    free(local_image);
    return new_local_image;
}

// Updates a region of the image with the corresponding contour pixels
void update_image(ppm_image *local_image, ppm_image *contour, int local_x, int y) {
    for (int i = 0; i < contour->x; i++) {
        for (int j = 0; j < contour->y; j++) {
            int contour_pixel_index = contour->x * i + j;
            int local_pixel_index = (local_x + i) * local_image->y + y + j;

            local_image->data[local_pixel_index].red = contour->data[contour_pixel_index].red;
            local_image->data[local_pixel_index].green = contour->data[contour_pixel_index].green;
            local_image->data[local_pixel_index].blue = contour->data[contour_pixel_index].blue;
        }
    }
}

// Samples the grid for the local portion of the image
unsigned char **sample_grid(ppm_image *local_image, int step_x, int step_y, unsigned char sigma) {
    int local_p = (local_image->x - 1) / step_x;
    int q = local_image->y / step_y;

    unsigned char **grid = (unsigned char **)malloc((local_p + 1) * sizeof(unsigned char *));
    for (int i = 0; i <= local_p; i++) {
        grid[i] = (unsigned char *)malloc((q + 1) * sizeof(unsigned char));
    }

    for (int i = 0; i < local_p; i++) {
        for (int j = 0; j < q; j++) {
            ppm_pixel curr_pixel = local_image->data[i * step_x * local_image->y + j * step_y];
            unsigned char curr_color = (curr_pixel.red + curr_pixel.green + curr_pixel.blue) / 3;

            grid[i][j] = (curr_color > sigma) ? 0 : 1;
        }
    }

    grid[local_p][q] = 0;

    // last sample points have no neighbors below / to the right, so we use pixels on the
    // last row / column of the input image for them
    for (int i = 0; i < local_p; i++) {
        ppm_pixel curr_pixel = local_image->data[i * step_x * local_image->y + local_image->x - 1];

        unsigned char curr_color = (curr_pixel.red + curr_pixel.green + curr_pixel.blue) / 3;

        if (curr_color > sigma) {
            grid[i][q] = 0;
        } else {
            grid[i][q] = 1;
        }
    }
    for (int j = 0; j < q; j++) {
        ppm_pixel curr_pixel = local_image->data[(local_image->x - 1) * local_image->y + j * step_y];

        unsigned char curr_color = (curr_pixel.red + curr_pixel.green + curr_pixel.blue) / 3;

        if (curr_color > sigma) {
            grid[local_p][j] = 0;
        } else {
            grid[local_p][j] = 1;
        }
    }

    return grid;
}

// Performs the marching squares algorithm on the local image data
void march(ppm_image *local_image, unsigned char **grid, ppm_image **contour_map, int step_x, int step_y) {
    int local_p = local_image->x / step_x;
    int q = local_image->y / step_y;

    for (int i = 0; i < local_p; i++) {
        for (int j = 0; j < q; j++) {
            unsigned int k = 8 * (grid[i][j] - '\0') + 4 * (grid[i][j + 1] - '\0') + 
                              2 * (grid[i + 1][j + 1] - '\0') + 1 * (grid[i + 1][j] - '\0');
            update_image(local_image, contour_map[k], i * step_x, j * step_y);
        }
    }
}

// Frees the resources allocated for the grid and contour map
void free_resources(ppm_image *local_image, ppm_image **contour_map, unsigned char **grid) {
    for (int i = 0; i < CONTOUR_CONFIG_COUNT; i++) {
        free(contour_map[i]->data);
        free(contour_map[i]);
    }
    free(contour_map);

    if (grid) {
        for (int i = 0; i <= local_image->x / STEP; i++) {
            free(grid[i]);
        }
        free(grid);
    }

    free(local_image->data);
    free(local_image);
}

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (argc < 3) {
        if (rank == 0) {
            fprintf(stderr, "Usage: ./tema1 <in_file> <out_file>\n");
        }
        MPI_Finalize();
        return 1;
    }

    ppm_image *global_image = NULL;
    int global_width = 0, global_height = 0;

    // Rank 0 reads the image and determines its dimensions
    if (rank == 0) {
        global_image = read_ppm(argv[1]);
        global_width = global_image->x;
        global_height = global_image->y;
        if (global_width % size != 0) {
            fprintf(stderr, "Error: Image width must be divisible by the number of processes.\n");
            MPI_Finalize();
            return 1;
        }
    }

    // Broadcast dimensions to all processes
    MPI_Bcast(&global_width, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&global_height, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Calculate rows to be handled by each process
    int rows_per_proc = global_width / size;

    // Allocate local image for each process
     // Allocate local image for each process
    ppm_image *local_image = (ppm_image *)malloc(sizeof(ppm_image));
    local_image->x = rows_per_proc + 1;
    local_image->y = global_height;
    local_image->data = (ppm_pixel *)malloc(local_image->x * global_height * sizeof(ppm_pixel));

    if (rank == 0) {
        for (int i = 0; i < size; i++) {
            int start_row = i * (local_image->x - 1);
            int send_size = local_image->x * local_image->y * sizeof(ppm_pixel);
            if (i == 0) {
                memcpy(local_image->data, global_image->data, send_size); 
            } else {
                MPI_Send(&global_image->data[start_row * local_image->y],
                        send_size,
                        MPI_BYTE,
                        i,
                        0,
                        MPI_COMM_WORLD);
            }
        }
    } else {
        int recv_size = local_image->x * local_image->y * sizeof(ppm_pixel);
        MPI_Recv(local_image->data,
                recv_size,
                MPI_BYTE,
                0,
                0,
                MPI_COMM_WORLD,
                MPI_STATUS_IGNORE);
    }



    // Initialize contour map
    ppm_image **contour_map = init_contour_map();

    // Rescale the local image
    local_image = rescale_image(local_image, &global_width, &global_height, size);

    MPI_Barrier(MPI_COMM_WORLD);
    // Perform processing for each sigma value
    int sig = 200;
    // for (int sig = 50; sig <= 400; sig += 5) {
        unsigned char **grid = sample_grid(local_image, STEP, STEP, sig);
        MPI_Barrier(MPI_COMM_WORLD);
        march(local_image, grid, contour_map, STEP, STEP);

        for (int i = 0; i <= local_image->x / STEP; i++) {
            free(grid[i]);
        }
        free(grid);
    // }
    MPI_Barrier(MPI_COMM_WORLD);
    // Gather the processed image data back to rank 0

    ppm_image *final_image = NULL;
    if (rank == 0) {
        final_image = malloc(sizeof(*final_image));
        final_image->data = malloc(global_width * local_image->y * sizeof(*final_image->data));
        final_image->x = global_width;
        final_image->y = global_height;
    }
    
    MPI_Gather(local_image->data,
               (local_image->x - 1) * local_image->y * sizeof(ppm_pixel),
               MPI_BYTE,
               (rank == 0) ? final_image->data : NULL,
               (local_image->x - 1)* local_image->y * sizeof(ppm_pixel),
               MPI_BYTE,
               0,
               MPI_COMM_WORLD);

    // Write the final image on rank 0
    if (rank == 0) {
        write_ppm(final_image, argv[2]);
        free(final_image->data);
        free(final_image);
    }

    // // Free resources
    // free_resources(local_image, contour_map, NULL);

    MPI_Finalize();
    return 0;
}

