// import libs
#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>

#define MASTER_RANK 0
#define n 100

void test(int *C) {
    int c, r;
    for(int i = 0; i < n; ++i) {
        c = *(C + i);
        r = i + 10 * i;
        if(c != r) {
            printf("%s\n", "Test result: Failed !");
            return;
        }
    }
    printf("%s\n", "Test result: Passed !");
}

void init_master(int **A, int **B, int **C) {
    *A = (int *) malloc(n * sizeof(int));
    *B = (int *) malloc(n * sizeof(int));
    *C = (int *) malloc(n * sizeof(int));

    for(int i = 0; i < n; ++i) {
        *(*A + i) = i;
        *(*B + i) = 10 * i;
        *(*C + i) = 0;
    }
}

void init_slaver(int **A, int **B, int **C, int Mc) {
    *A = (int *) malloc(Mc * sizeof(int));
    *B = (int *) malloc(Mc * sizeof(int));
    *C = (int *) malloc(Mc * sizeof(int));
}

int main(int argc, char** argv) {

    // declare variables
    int *A, *B, *C;
    int *Ac, *Bc, *Cc;
    int Mc;
    int _sum;

    // init MPI communication
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Status status;
    Mc = n / size;

    // init slave + master
    init_slaver(&Ac, &Bc, &Cc, Mc);
    if(rank == MASTER_RANK) {
        init_master(&A, &B, &C);
    }

    // decomposition
    // if(rank == MASTER_RANK) {
    //     for(int i = 1; i < size; ++i) {
    //         MPI_Send(A + i*Mc, Mc, MPI_INT, i, i, MPI_COMM_WORLD);
    //         MPI_Send(B + i*Mc, Mc, MPI_INT, i, i + 3*size, MPI_COMM_WORLD);
    //     }
    // } else {
    //     MPI_Recv(Ac, Mc, MPI_INT, MASTER_RANK, rank, MPI_COMM_WORLD, &status);
    //     MPI_Recv(Bc, Mc, MPI_INT, MASTER_RANK, rank + 3*size, MPI_COMM_WORLD, &status);
    // }
    MPI_Scatter(A, Mc, MPI_INT,
                Ac, Mc, MPI_INT, MASTER_RANK, MPI_COMM_WORLD);
    MPI_Scatter(B, Mc, MPI_INT,
                Bc, Mc, MPI_INT, MASTER_RANK, MPI_COMM_WORLD);
    // computation
    // if(rank == MASTER_RANK) {
    //     for(int i = 0; i < Mc; ++i) {
    //         *(C + i) = *(A + i) + *(B + i);
    //     }
    // } else {
    //     for(int i = 0; i < Mc; ++i) {
    //         *(Cc + i) = *(Ac + i) + *(Bc + i);
    //     }
    // }
    for(int i = 0; i < Mc; ++i) {
        *(Cc + i) = *(Ac + i) + *(Bc + i);
    }
    // collect
    // if(rank == MASTER_RANK) {
    //     for(int i = 1; i < size; ++i) {
    //         MPI_Recv(C + i*Mc, Mc, MPI_INT, i, i, MPI_COMM_WORLD, &status);
    //     }
    // } else {
    //     MPI_Send(Cc, Mc, MPI_INT, 0, rank, MPI_COMM_WORLD);
    // }
    MPI_Gather(Cc, Mc, MPI_INT,
               C, Mc, MPI_INT, MASTER_RANK, MPI_COMM_WORLD);
    // show result at master
    if(rank == MASTER_RANK) {
        for (int i = 0; i < n; ++i) {
            printf("%d, %d, %d\n", *(A + i), *(B + i), *(C + i));
        }
        test(C);
    }

    //
    MPI_Finalize(); 
    return 0;
}