%%writefile example.cu
#include <stdio.h>
#include <stdlib.h>
#include <gd.h>
#include <cuda.h>
#include <cuda_runtime.h>
#include <stdio.h>
#include <time.h>

#define WIDTH 2000
#define HEIGHT 2000
#define NUM_ITER 100

__global__ void game_of_life_kernel(int *old_state, int *new_state, int width, int height) {
    int x = threadIdx.x + blockIdx.x * blockDim.x;
    int y = threadIdx.y + blockIdx.y * blockDim.y;

    int idx = y * width + x;
    int num_neighbors = 0;

    // Check the neighbors
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            int nx = (x + i + width) % width;
            int ny = (y + j + height) % height;
            int nidx = ny * width + nx;
            if (old_state[nidx]) {
                num_neighbors++;
            }
        }
    }

    // Apply the rules of the Game of Life
    if (old_state[idx]) {
        if (num_neighbors == 2 || num_neighbors == 3) {
            new_state[idx] = 1;
        } else {
            new_state[idx] = 0;
        }
    } else {
        if (num_neighbors == 3) {
            new_state[idx] = 1;
        } else {
            new_state[idx] = 0;
        }
    }
}

int main() {
    //printf("slm");
    clock_t start_time, end_time;
    double elapsed_time;

    // Record start time
    start_time = clock();

    // Initialize the grid with some cells
    int grid[HEIGHT][WIDTH] = {{0}};
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (rand() % 2 == 0) {
                grid[i][j] = 1;
            }
        }
    }

    // Copy the grid to the device
    int *d_old_state, *d_new_state;
    cudaMalloc(&d_old_state, WIDTH * HEIGHT * sizeof(int));
    cudaMalloc(&d_new_state, WIDTH * HEIGHT * sizeof(int));
    cudaMemcpy(d_old_state, grid, WIDTH * HEIGHT * sizeof(int), cudaMemcpyHostToDevice);

    // Set up the block and grid dimensions
    dim3 block_dim(32, 32);
    dim3 grid_dim((WIDTH + block_dim.x - 1) / block_dim.x, (HEIGHT + block_dim.y - 1) / block_dim.y);

    // Run the simulation for a fixed number of iterations
    for (int i = 0; i < NUM_ITER; i++) {
        game_of_life_kernel<<<grid_dim, block_dim>>>(d_old_state, d_new_state, WIDTH, HEIGHT);
        cudaDeviceSynchronize();

        // Swap the old and new states
        int *tmp = d_old_state;
        d_old_state = d_new_state;
        d_new_state = tmp;

        // Generate an image of the current state
        int *h_old_state = (int *)malloc(WIDTH * HEIGHT * sizeof(int));
        cudaMemcpy(h_old_state, d_old_state, WIDTH * HEIGHT * sizeof(int), cudaMemcpyDeviceToHost);

        gdImagePtr im;
        FILE *fp;
        int black, white;

        im = gdImageCreate(WIDTH, HEIGHT);
        white = gdImageColorAllocate(im, 255, 255, 255);
        black = gdImageColorAllocate(im, 0, 0, 0);

        for (int i = 0; i <WIDTH; i++) {
for (int j = 0; j < HEIGHT; j++) {
if (h_old_state[j * WIDTH + i]) {
gdImageSetPixel(im, i, j, black);
} else {
gdImageSetPixel(im, i, j, white);
}
}
}
    char filename[256];
    sprintf(filename, "frame%d.png", i);
    fp = fopen(filename, "wb");
    gdImagePng(im, fp);
    fclose(fp);
    gdImageDestroy(im);

    free(h_old_state);
}

// Free memory
cudaFree(d_old_state);
cudaFree(d_new_state);

// Record end time
    end_time = clock();

    // Calculate elapsed time
    elapsed_time = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;

    // Print elapsed time
    printf("Elapsed time: %f seconds\n", elapsed_time);

return 0;
}