#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <omp.h>



#define SIZE 1000

#define GENERATIONS 100



void initialize_board(int board[][SIZE]) {

    int i, j;

    for (i = 0; i < SIZE; i++) {

        for (j = 0; j < SIZE; j++) {

            board[i][j] = rand() % 2;

        }

    }

}



void print_board(int board[][SIZE]) {

    int i, j;

    printf("\033[0;40m");  // Set console background color to black

    for (i = 0; i < SIZE; i++) {

        for (j = 0; j < SIZE; j++) {

            if (board[i][j] == 1) {

                printf("\u2588\u2588");  // Unicode block character for black block

            } else {

                printf("\u2591\u2591");  // Unicode block character for white block

            }

        }

        printf("\n");

    }

    printf("\033[0m\n");  // Reset console color

}



void compute_next_generation(int old_board[][SIZE], int new_board[][SIZE]) {

    int i, j;

    #pragma omp parallel for private(j)

    for (i = 0; i < SIZE; i++) {

        #pragma omp parallel for

        for (j = 0; j < SIZE; j++) {

            int num_neighbors = 0;

            int ii, jj;



            for (ii = i - 1; ii <= i + 1; ii++) {

                for (jj = j - 1; jj <= j + 1; jj++) {

                    if (ii == i && jj == j) {

                        continue;

                    }

                    if (ii < 0 || ii >= SIZE || jj < 0 || jj >= SIZE) {

                        continue;

                    }

                    num_neighbors += old_board[ii][jj];

                }

            }



            if (old_board[i][j] == 1 && num_neighbors < 2) {

                new_board[i][j] = 0;

            } else if (old_board[i][j] == 1 && (num_neighbors == 2 || num_neighbors == 3)) {

                new_board[i][j] = 1;

            } else if (old_board[i][j] == 1 && num_neighbors > 3) {

                new_board[i][j] = 0;

            } else if (old_board[i][j] == 0 && num_neighbors == 3) {

                new_board[i][j] = 1;

            } else {

                new_board[i][j] = old_board[i][j];

            }

        }

    }

}



int main() {

    printf("by the way");

    int i, generation;

    int board[SIZE][SIZE];

    int new_board[SIZE][SIZE];

    omp_set_num_threads(2);

    srand(0);

    initialize_board(board);

    print_board(board);

    double start_time = omp_get_wtime(); 

    for (generation = 0; generation < GENERATIONS; generation++) {

        compute_next_generation(board, new_board);

        memcpy(board, new_board, SIZE * SIZE * sizeof(int));

    }

    double end_time = omp_get_wtime(); 

    double elapsed_time = end_time - start_time;

    printf("Time taken by the for loop in the main function: %f seconds\n", elapsed_time);

    return 0;

}

