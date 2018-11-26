#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define N 100

int main(int argc, char **argv) {

    int a[N], b[N], myrank;
    MPI_Init(&argc, &argv);
    MPI_Status status;
    for(int i=0; i<N; ++i) {
        a[i] = 1;
        b[i] = 2;
    }
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

    if (myrank == 0) {
        for(int i=0; i<N; ++i) {
            printf("%d, %d \n", a[i], b[i]);
        }
        printf("%s\n", "=======================================");
    }

    if (myrank == 0) {
        MPI_Send(a, N, MPI_INT, 1, 1, MPI_COMM_WORLD);
        MPI_Send(b, N, MPI_INT, 1, 2, MPI_COMM_WORLD);
    } else if (myrank == 1) {
        MPI_Recv(b, N, MPI_INT, 0, 2, MPI_COMM_WORLD, &status);
        MPI_Recv(a, N, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
    }

    if (myrank == 0) {
        for(int i=0; i<N; ++i) {
            printf("%d, %d \n", a[i], b[i]);
        }
    }

    MPI_Finalize();
    return 0;
}