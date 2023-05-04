#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ROWS 1000
#define COLS 1000
#define GENERATIONS 100

void initialize(int grid[ROWS][COLS]) {
    int i, j;
    for (i = 0; i < ROWS; i++) {
        for (j = 0; j < COLS; j++) {
            grid[i][j] = rand() % 2;
        }
    }
}

int count_neighbors(int grid[ROWS][COLS], int row, int col) {
    int i, j, count = 0;
    for (i = row - 1; i <= row + 1; i++) {
        for (j = col - 1; j <= col + 1; j++) {
            if (i >= 0 && i < ROWS && j >= 0 && j < COLS && !(i == row && j == col)) {
                count += grid[i][j];
            }
        }
    }
    return count;
}

void evolve(int grid[ROWS][COLS]) {
    int new_grid[ROWS][COLS];
    int i, j, neighbors, cell;
    for (i = 0; i < ROWS; i++) {
        for (j = 0; j < COLS; j++) {
            neighbors = count_neighbors(grid, i, j);
            cell = grid[i][j];
            if (cell == 0 && neighbors == 3) {
                new_grid[i][j] = 1;
            } else if (cell == 1 && (neighbors < 2 || neighbors > 3)) {
                new_grid[i][j] = 0;
            } else {
                new_grid[i][j] = cell;
            }
        }
    }
    for (i = 0; i < ROWS; i++) {
        for (j = 0; j < COLS; j++) {
            grid[i][j] = new_grid[i][j];
        }
    }
}

int main() {
    clock_t start_time, end_time;
    double elapsed_time;
    // Record start time
    start_time = clock();
    int grid[ROWS][COLS];
    srand(time(NULL));
    initialize(grid);
    int i;
    for (i = 0; i < GENERATIONS; i++) {
        evolve(grid);
    }
    // Record end time
    end_time = clock();
    // Calculate elapsed time
    elapsed_time = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
    // Print elapsed time
    printf("Elapsed time: %f seconds\n", elapsed_time);
    return 0;
}
