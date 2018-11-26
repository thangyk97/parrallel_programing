#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>

#define N 1000



int main(int argc, char **argv) {
    int *A, *Ac;
    int size, rank;
    int Mc;
    int sum = 0;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Status status;

    // Init 
    A = (int*) malloc(N * sizeof(int));
    for(int i=0; i < N; ++i) {
        *(A + i) = i;
    }
    Mc = N / size;

    return 0;
}