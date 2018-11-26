#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include <time.h>
#include <mpi.h>

#define  M       20
#define  Time    10
#define  dt      0.01
#define  dx      0.1
#define  D       0.1

//=========================
void DisplayArray(float *T, int size)
{
    int i;
    for(i=0;i<size;i++)
        printf("  %.2f",*(T+i));
    printf("\n");
}
//=========================
void init(float *T)
{
    int i,j;
    for (  i = 0 ; i < M ; i++ )
        *(T+i) = 25.0;
}
//=========================
void gradient(float *T, float *dT, float TLeft, float TRight, int stop)
{
    int i;
    float c,l,r;

    for (  i = 0 ; i < stop ; i++ )
    {
        c = *(T+i);
        l = (i==0)   ? TLeft : *(T+(i-1));
        r = (i==stop-1) ? TRight  : *(T+(i+1));
        *(dT+i) = (r-2*c+l)/(dx*dx);
    }
}
//=========================
int main(int argc, char** argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_Status status;

    // Init
    int i, t;
    float left, right;
    float *T, *dT;
    int domain_size = M / size;

    if (rank == 0) {
        T = (float*) malloc(M * sizeof(float));
        dT = (float*) malloc(M * sizeof(float));
        init(T);
    } else {
        T = (float*) malloc(domain_size * sizeof(float));
        dT = (float*) malloc(domain_size * sizeof(float));
    }
    // Input data distribution
    if (rank == 0) {
        for (i = 1; i < size; ++i) {
            MPI_Send(T,
                    domain_size,
                    MPI_FLOAT,
                    i,
                    i,
                    MPI_COMM_WORLD);
        }
    } else {
        MPI_Recv(T,
                 domain_size,
                 MPI_FLOAT,
                 0,
                 rank,
                 MPI_COMM_WORLD,
                 &status);
    }
    // Computation
    for (t = 0; t < Time / dt; t++) {
        // Send left
        if (rank != 0) {
            MPI_Send(T,
                     1,
                     MPI_FLOAT,
                     rank - 1,
                     rank + 2*size,
                     MPI_COMM_WORLD);
        } 
        // Send right
        if (rank != size - 1) {
            MPI_Send(T + domain_size - 1,
                     1,
                     MPI_FLOAT,
                     rank + 1,
                     rank + 4*size,
                     MPI_COMM_WORLD);
        }
        // Receiv left
        if (rank != 0) {
            MPI_Recv(&left,
                     1,
                     MPI_FLOAT,
                     rank - 1,
                     (rank -1) + 4*size,
                     MPI_COMM_WORLD,
                     &status);
        }
        // Receiv right
        if (rank != size - 1) {
            MPI_Recv(&right,
                     1,
                     MPI_FLOAT,
                     rank + 1,
                     (rank + 1) + 2*size,
                     MPI_COMM_WORLD,
                     &status);
        }
        if (rank == 0) {
            left = 100.0;
        }
        if (rank == size -1) {
            right = 30.0;
        }

        gradient(T, dT, left, right, domain_size);
        for (i = 0 ; i < domain_size ; i++ ){
            *(T+i) = *(T+i) + D*dt*(*(dT+i));
        }
    }
    // Collection
    if (rank != 0) {
        MPI_Send(T,
                 domain_size,
                 MPI_FLOAT,
                 0,
                 rank + 6*size,
                 MPI_COMM_WORLD);
    } else {
        for (i = 1; i < size; ++i) {
            MPI_Recv(T + i*domain_size,
                     domain_size,
                     MPI_FLOAT,
                     i,
                     i + 6*size,
                     MPI_COMM_WORLD,
                     &status);
        }
    }

    if (rank == 0) {
        printf("Result of C:\n");
        DisplayArray(T, M);
    }

    MPI_Finalize();
    return 0;
}


  // 95.86  91.73  87.64  83.60  79.64  75.75  71.96  68.28  64.70  61.25  57.92  54.71  51.61  48.63  45.76  42.98  40.28  37.65  35.07  32.52
