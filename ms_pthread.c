#define _XOPEN_SOURCE 600
#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#define min(X,Y) ((X) < (Y) ? (X) : (Y))
#define CONTOUR_CONFIG_COUNT    16
#define FILENAME_MAX_SIZE       50
#define STEP                    8
#define SIGMA                   200
#define RESCALE_X               4096
#define RESCALE_Y               4096

#define CLAMP(v, min, max) if(v < min) { v = min; } else if(v > max) { v = max; }

// structure used for passing parameters to thread function
struct data {
    int step_x;
    int step_y;
    unsigned char** grid;
    ppm_image *image;
    ppm_image *new_image;
    ppm_image **contour_map;
    int id;
    int P;
    pthread_barrier_t* barrier;
};

// Creates a map between the binary configuration (e.g. 0110_2) and the corresponding pixels
// that need to be set on the output image. An array is used for this map since the keys are
// binary numbers in 0-15. Contour images are located in the './contours' directory.
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

// Updates a particular section of an image with the corresponding contour pixels.
// Used to create the complete contour image.
void update_image(ppm_image *image, ppm_image *contour, int x, int y) {
    for (int i = 0; i < contour->x; i++) {
        for (int j = 0; j < contour->y; j++) {
            int contour_pixel_index = contour->x * i + j;
            int image_pixel_index = (x + i) * image->y + y + j;

            image->data[image_pixel_index].red = contour->data[contour_pixel_index].red;
            image->data[image_pixel_index].green = contour->data[contour_pixel_index].green;
            image->data[image_pixel_index].blue = contour->data[contour_pixel_index].blue;
        }
    }
}

// Corresponds to step 1 of the marching squares algorithm, which focuses on sampling the image.
// Builds a p x q grid of points with values which can be either 0 or 1, depending on how the
// pixel values compare to the `sigma` reference value. The points are taken at equal distances
// in the original image, based on the `step_x` and `step_y` arguments.
void sample_grid(unsigned char** grid, ppm_image *image, int step_x, int step_y, unsigned char sigma, int id, int P) {
    int p = image->x / step_x;
    int q = image->y / step_y;

    // we are splitting the lines of the matrix for every thread
    // we calculate the start and end cell
    int start_l = id * (double)p / P;
    int start_c = id * (double)q / P;
    int end_l = min((id + 1) * (double)p / P, p);
    int end_c = min((id + 1) * (double)q / P, q);

    for (int i = start_l; i < end_l; i++) {
        for (int j = 0; j < q; j++) {
            ppm_pixel curr_pixel = image->data[i * step_x * image->y + j * step_y];

            unsigned char curr_color = (curr_pixel.red + curr_pixel.green + curr_pixel.blue) / 3;
            
            if (curr_color > sigma) {
                grid[i][j] = 0;
            } else {
                grid[i][j] = 1;
            }
        }
    }

    // last sample points have no neighbors below / to the right, so we use pixels on the
    // last row / column of the input image for them
    for (int i = start_l; i < end_l; i++) {
        ppm_pixel curr_pixel = image->data[i * step_x * image->y + image->x - 1];

        unsigned char curr_color = (curr_pixel.red + curr_pixel.green + curr_pixel.blue) / 3;

        if (curr_color > sigma) {
            grid[i][q] = 0;
        } else {
            grid[i][q] = 1;
        }
    }
    for (int j = start_c; j < end_c; j++) {
        ppm_pixel curr_pixel = image->data[(image->x - 1) * image->y + j * step_y];

        unsigned char curr_color = (curr_pixel.red + curr_pixel.green + curr_pixel.blue) / 3;

        if (curr_color > sigma) {
            grid[p][j] = 0;
        } else {
            grid[p][j] = 1;
        }
    }
}

// Corresponds to step 2 of the marching squares algorithm, which focuses on identifying the
// type of contour which corresponds to each subgrid. It determines the binary value of each
// sample fragment of the original image and replaces the pixels in the original image with
// the pixels of the corresponding contour image accordingly.
void march(ppm_image *image, unsigned char **grid, ppm_image **contour_map, int step_x, int step_y, int id, int P) {
    int p = image->x / step_x;
    int q = image->y / step_y;

    // again calculate the start and end lines for every thread to process
    int start_l = id * (double)p / P;
    int end_l = min((id + 1) * (double)p / P, p);
    for (int i = start_l; i < end_l; i++) {
        for (int j = 0; j < q; j++) {
            unsigned char k = 8 * grid[i][j] + 4 * grid[i][j + 1] + 2 * grid[i + 1][j + 1] + 1 * grid[i + 1][j];
            update_image(image, contour_map[k], i * step_x, j * step_y);
        }
    }
}

// Calls `free` method on the utilized resources.
void free_resources(ppm_image *image, ppm_image * new_image, ppm_image **contour_map, unsigned char **grid, int step_x) {
    for (int i = 0; i < CONTOUR_CONFIG_COUNT; i++) {
        free(contour_map[i]->data);
        free(contour_map[i]);
    }
    free(contour_map);

    int x = new_image == NULL ? image->x : new_image->x;
    for (int i = 0; i <= x / step_x; i++) {
        free(grid[i]);
    }
    free(grid);

    free(image->data);
    free(image);

    if (new_image != NULL) {
        free(new_image->data);
        free(new_image);
    }
}

void rescale_image(ppm_image *image, ppm_image *new_image, int id, int P) {
    uint8_t sample[3];

    // again calculate the start and end lines for every thread to process
    int start_l = id * (double)new_image->x / P;
    int end_l = min((id + 1) * (double)new_image->x / P, new_image->x);

    // use bicubic interpolation for scaling
    for (int i = start_l; i < end_l; i++) {
        for (int j = 0; j < new_image->y; j++) {
            float u = (float)i / (float)(new_image->x - 1);
            float v = (float)j / (float)(new_image->y - 1);
            sample_bicubic(image, u, v, sample);

            new_image->data[i * new_image->y + j].red = sample[0];
            new_image->data[i * new_image->y + j].green = sample[1];
            new_image->data[i * new_image->y + j].blue = sample[2];
        }
    }

}

// function that will be called 
void* f(void *arg) {

    // casting to the data structure
    struct data x = *(struct data *) arg;

    // checking if we need to rescale the image
    if (x.new_image != NULL) {
        // 1. Rescale the image
        rescale_image(x.image, x.new_image, x.id, x.P);
    }
    
    // waiting for all the threads to finish the rescaling so we can calculate the grid
    pthread_barrier_wait(x.barrier);

    // if we rescaled update the image pointer
    if (x.new_image != NULL) {
        x.image = x.new_image;
    }

    for (int sig = 50; sig <= 400; sig+=5) {
        // calling the sample grid function from every thread with the specific id
        sample_grid(x.grid, x.image, x.step_x, x.step_y, sig, x.id, x.P);

        // waiting for the grid to be completed by every thread
        pthread_barrier_wait(x.barrier);

        // calling the march function to update the image from every thread with the specific id
        march(x.image, x.grid, x.contour_map, x.step_x, x.step_y, x.id, x.P);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: ./tema1 <in_file> <out_file> <P>\n");
        return 1;
    }

    ppm_image *image = read_ppm(argv[1]);
    ppm_image *new_image = NULL;
    ppm_image *print_image = image;
    int step_x = STEP;
    int step_y = STEP;
    int n_threads = atoi(argv[3]);
    pthread_t threads[n_threads];
    int status;
    struct data arg[n_threads];
    pthread_barrier_t barrier;

    // initializing the barrier
    int r = pthread_barrier_init(&barrier, NULL, n_threads);
    
    // 0. Initialize contour map
    ppm_image **contour_map = init_contour_map();

    int p = image->x, q = image->y;
    // checking if we need to rescale the image to fit the maximum size
    // if we do then allocate the memory for the new image
    if (image->x > RESCALE_X || image->y > RESCALE_Y) {
        new_image = (ppm_image *)malloc(sizeof(ppm_image));
        if (!new_image) {
            fprintf(stderr, "Unable to allocate memory\n");
            exit(1);
        }
        p = RESCALE_X;
        q = RESCALE_Y;
        new_image->x = RESCALE_X;
        new_image->y = RESCALE_Y;

        new_image->data = (ppm_pixel*)malloc(new_image->x * new_image->y * sizeof(ppm_pixel));
        if (!new_image->data) {
            fprintf(stderr, "Unable to allocate memory\n");
            exit(1);
        }
    }

    // allocate memory for the grid
    p /= step_x;
    q /= step_y;
    unsigned char **grid = (unsigned char **)malloc((p + 1) * sizeof(unsigned char*));
    for (int i = 0; i <= p; i++) {
        grid[i] = (unsigned char *)malloc((q + 1) * sizeof(unsigned char));
        if (!grid[i]) {
            fprintf(stderr, "Unable to allocate memory\n");
            exit(1);
        }
    }
    grid[p][q] = 0;

    // building the argument structure for every thread
    for (int i = 0; i < n_threads; i++) {
        arg[i].step_x = step_x;
        arg[i].step_y = step_y;
        arg[i].grid = grid;
        arg[i].image = image;
        arg[i].new_image = new_image;
        arg[i].P = n_threads;
        arg[i].id = i;
        arg[i].contour_map = contour_map;
        arg[i].barrier = &barrier;
    }

    // creating the threads
    for (int i = 0; i < n_threads; i++) {
        int r = pthread_create(&threads[i], NULL, f, &arg[i]);
    }

    // waiting for every thread to finish
    for (int i = 0; i < n_threads; i++) {
        int r = pthread_join(threads[i], &status);
    }

    // destroying the barrier
    pthread_barrier_destroy(&barrier);

    // choosing the pointer to the image to pring
    if (new_image != NULL) {
        print_image = new_image;
    }
    
    //4. Write output
    write_ppm(print_image, argv[2]);

    // freeing the allocated memory
    free_resources(image, new_image, contour_map, grid, step_x);
    return 0;
}
