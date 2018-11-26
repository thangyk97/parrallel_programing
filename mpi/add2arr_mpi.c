#include<stdio.h>
#include<stdlib.h>
// #include<omp.h>
#include <mpi.h>

int id, i;
int n = 100;
int *a;
int *b;
int *c;
int *d;

int *ac, *bc, *dc;
int domain_size;
int test;

void init_master(int n, int domain_size) {
    a = (int *) malloc(n * sizeof(int));
    b = (int *) malloc(n * sizeof(int));
    c = (int *) malloc(n * sizeof(int));
    d = (int *) malloc(n * sizeof(int));
    
    for(i=1; i<n; i++) {
        *(a+i) = i;
        *(b+i) = i * 10;
    }
}

void init_slaver(int size) {
    ac = (int *) malloc(size * sizeof(int));
    bc = (int *) malloc(size * sizeof(int));
    dc = (int *) malloc(size * sizeof(int));
}

// void add_array_parallel(int *a, int *b, int *d) {
//     #pragma omp parallel private(id, i)
//     {
// 	printf("%d", omp_get_num_threads());
//         id = omp_get_thread_num();
//         for(i=id*domain_size; i<(id+1)*domain_size; i++) {
//             *(d+i) = *(a+i) + *(b+i);
//         }
//     }

// }

void add_array_single(int *a, int *b, int *c, int n) {

    for(i=0; i<n; i++) {
        *(c+i) = *(a+i) + *(b+i);
    }

}

int main(int argc, char** argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);


    MPI_Status stat;

    // Domain decomposition
    domain_size = n / size;

    // init I data in rank 0
    if (rank == 0) {
        init_master(n, domain_size);
        init_slaver(domain_size);
    } else {
        // memory array allocation in slaver
        init_slaver(domain_size);
    }

    // Input data distribution
    // if (rank == 0) {
    //     for (i = 1; i < size; i++) {
    //         MPI_Send((a + i * domain_size),       // buf
    //                 domain_size,                // count
    //                 MPI_INT,                    // dtype
    //                 i, i,                       // dest, tag
    //                 MPI_COMM_WORLD);            // communicater

    //         MPI_Send((b + i * domain_size),
    //                 domain_size,
    //                 MPI_INT,
    //                 i, i + size,                // tag = i + size
    //                 MPI_COMM_WORLD);
    //     }
    // }

    // if (rank != 0) {
    //     // receive
    //     MPI_Recv(a, 
    //             domain_size, 
    //             MPI_INT,
    //             0, rank, 
    //             MPI_COMM_WORLD, &stat);

    //     MPI_Recv(b, 
    //             domain_size, 
    //             MPI_INT,
    //             0, rank + size, 
    //             MPI_COMM_WORLD, &stat);
    // }

    MPI_Scatter(a,
                domain_size,
                MPI_INT,
                ac,
                domain_size,
                MPI_INT,
                0,
                MPI_COMM_WORLD);

    MPI_Scatter(b,
                domain_size,
                MPI_INT,
                bc,
                domain_size,
                MPI_INT,
                0,
                MPI_COMM_WORLD);

    // // Computation
    // if (rank == 0) {
    //     for (i = 0; i < domain_size; i ++) {
    //         *(d + i) = *(a + i) + *(b + i);
    //     }
    // } else {
    //     add_array_single(a, b, d, domain_size);
    // }
    for (i = 0; i < domain_size; ++i) {
        *(dc + i) = *(ac + i) + *(bc + i);
    }

    // // Output collection
    // if (rank != 0) {
    //     MPI_Send(d, domain_size, MPI_INT,
    //             0, rank + 2 * size, MPI_COMM_WORLD);
    // } else {
    //     for (i = 1; i < size; i++) {
    //         MPI_Recv((d + i * domain_size), domain_size, MPI_INT,
    //                 i, i + 2 * size, MPI_COMM_WORLD, &stat);
    //     }
    // }
    MPI_Gather( dc,
                domain_size,
                MPI_INT,
                d,
                domain_size,
                MPI_INT,
                0,
                MPI_COMM_WORLD);

    if (rank == 0) {
        // Test with normal add
        add_array_single(a, b, c, n);

        for(i=1; i<100; i++) {
            printf("value with id = %d: %d, %d \n", i, *(c+i), *(d+i));
        }
    }
    
    MPI_Finalize();
    return 0;
}
