#include <stdio.h>

#include <stdlib.h>

#include <mpi.h>

#include <time.h>

#include <string.h>

#include <pthread.h>



#define SIZE 1000

#define GEN 100



pthread_mutex_t mutex;



int board[SIZE][SIZE];

int new_board[SIZE][SIZE];



void initialize_board(int board[][SIZE]) {

    int i, j;

    for (i = 0; i < SIZE; i++) {

        for (j = 0; j < SIZE; j++) {

            board[i][j] = rand() % 2;

        }

    }

}



void print(int board[][SIZE]) {

   int i, j;

    for (i = 0; i < SIZE; i++) {

        for (j = 0; j < SIZE; j++) {

            printf("%d ",board[i][j]);

        }

        printf("\n");

    }

    printf("\n");

}



void print_board(int board[][SIZE]) {

    int i, j;

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

}



void update_board(int board[][SIZE], int new_board[][SIZE], int start, int end, int rank) {

    int i, j;

    for (i = start; i < end; i++) {

        for (j = 0; j < SIZE; j++) {

        pthread_mutex_lock(&mutex);

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

                    //printf("%d ii=%d jj=%d %d\n",num_neighbors,ii,jj,board[ii][jj]);

                    num_neighbors += board[ii][jj];

                }

            }

            //printf("%d rank=%d i=%d j=%d\n",num_neighbors,rank,i,j);

            if (board[i][j] == 1 && num_neighbors < 2) {

                new_board[i][j] = 0;

            } else if (board[i][j] == 1 && (num_neighbors == 2 || num_neighbors == 3)) {

                new_board[i][j] = 1;

            } else if (board[i][j] == 1 && num_neighbors > 3) {

                new_board[i][j] = 0;

            } else if (board[i][j] == 0 && num_neighbors == 3) {

                new_board[i][j] = 1;

            } else {

                new_board[i][j] = board[i][j];

            }

            //printf("b=%d rank=%d i=%d j=%d\n",new_board[i][j],rank,i,j);

            pthread_mutex_unlock(&mutex);

        }

    }

}



int main(int argc, char **argv) {

    int rank, np, i, j, start, end;

    initialize_board(board);

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Comm_size(MPI_COMM_WORLD, &np);

    clock_t start_time, end_time;

    double elapsed_time;

    start_time = clock();

    if (rank==0) {

    print_board(board);

    	//print(board);

    }

    for (int g=0;g<GEN;g++) {

    	if (rank == 0) {

  		//print(board);

  		//print_board(board);

    		//printf("%d %d\n",SIZE,np);

    		int part = SIZE/np;

    		//printf("%d\n",part);

    	for (int i = 1; i < np; i++) {

    	MPI_Send(&board, SIZE*SIZE, MPI_INT, i, 0, MPI_COMM_WORLD);

       	MPI_Send(&part, 1, MPI_INT, i, 0, MPI_COMM_WORLD);

    	}

    	update_board(board,new_board,0,part,rank);

    	memcpy(board, new_board, SIZE * SIZE * sizeof(int));

    	int k[SIZE][SIZE];

    	int s;

    	for (int i=1;i<np;i++) {

    	     if (i!=np-1) {

    	     	MPI_Recv(k, SIZE*SIZE, MPI_INT, i, 0, MPI_COMM_WORLD, NULL);

    	     	for (int j=i*part;j<i*part+part;j++) {

    	     		for (int a=0;a<SIZE;a++) {

    	     			board[j][a]=k[j][a];

    	     		}

    	     	}

    	     }

    	     if (i==np-1) {

    	     	MPI_Recv(k, SIZE*SIZE, MPI_INT, i, 0, MPI_COMM_WORLD, NULL);

    	     	for (int j=i*part;j<SIZE;j++) {

    	     		for (int a=0;a<SIZE;a++) {

    	     			board[j][a]=k[j][a];

    	     		}

    	     	}

    	     }

    	}

    	//print(board);

    	//print_board(board);

    }

    else if (rank == np-1) {

    	//print(board);

    	int part;

    	MPI_Status status;

    	MPI_Recv(&board, SIZE*SIZE, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

    	//print(board);

   	MPI_Recv(&part, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

    	int start=part*rank;

    	//printf("%d\n",part);

    	update_board(board,new_board,start,SIZE,rank);

    	//print(new_board);

    	//printf("\n");

    	MPI_Send(new_board, SIZE*SIZE, MPI_INT, 0, 0, MPI_COMM_WORLD);

    }

    else {

    	//print(board);

    	int part;

    	MPI_Status status;

    	MPI_Recv(&board, SIZE*SIZE, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

    	//print(board);

    	MPI_Recv(&part, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

    	int start=part*rank;

    	//printf("%d\n",part);

    	//printf("%d \n",start);

    	update_board(board,new_board,start,start+part,rank);

    	//print(new_board);

    	//printf("\n");

    	MPI_Send(new_board, SIZE*SIZE, MPI_INT, 0, 0, MPI_COMM_WORLD);

    }

   	//memcpy(board, new_board, SIZE * SIZE * sizeof(int));

    }

    if (rank==0) {

    //print(board);

    print_board(board);

    // Record end time

        end_time = clock();

        // Calculate elapsed time

        elapsed_time = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;

        // Print elapsed time

        printf("Elapsed time: %f seconds\n", elapsed_time);

    	

    }

    printf("Time taken by the for loop in the main function: %f seconds\n", elapsed_time);

    MPI_Finalize();

    //print_board(board);

    //print(board);

    return 0;

}

    