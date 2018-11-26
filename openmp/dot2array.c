#include<stdio.h>
#include<stdlib.h>
#include<omp.h>
#include "dot2array.h"

int i, j, z;
int **A, **B, **C;
const int m = 6, n= 7, k = 5
int id;
int num_threads;

void displayArray(int **a, int r, int c)
{
    printf("\n");
    for(i=0; i<r; i++)
    {
        for(j=0; j<c; j++)
        {
            printf("%d ", *(*(a+i)+j));
        }
        printf("\n");
    }
}

void init()
{
    num_threads = omp_get_max_threads();
    /* allocate memory for A, B, C and init value for A, B*/
    // Allocate memory for A, B, C
    A = (int **)malloc(m * sizeof(int *));
    for(i=0; i<m; i++)
    {
        *(A+i) = (int *)malloc(n * sizeof(int));
    }
    //
    B = (int **)malloc(n * sizeof(int *));
    for(i=0; i<n; i++)
    {
        *(B+i) = (int *)malloc(k * sizeof(int));
    }
    //
    C = (int **)malloc(m * sizeof(int *));
    for(i=0; i<n; i++)
    {
        *(C+i) = (int *)malloc(k * sizeof(int));
    }
    // Init value for A and B
    for(i=0; i<m; i++)
        for(j=0; j<n; j++)
        {
            *(*(A+i)+j) = i+j;
        }
    for(i=0; i<n; i++)
        for(j=0; j<k; j++)
        {
           *(*(B+i)+j) = (i+j)*2;
        }

}

void normal_dot(int **A, int **B, int **C)
{
    /* Dot 2 array by sequense method */
    for(i=0; i<m; i++)
    {
        for(j=0; j<k; j++)
        {
            // Loop to compute the each element of C array
            *(*(C+i) + j) = 0;
            for(z=0; z<n; z++)
            {
                *(*(C+i) + j) += *(*(A+i)+z) * *(*(B+z)+j);
            }
        }
    }
}

void parallel_row_result_dot(int **A, int **B, int **C) {
    /* Dot 2 array by parallel compute result row method */
    int Mc = m / num_threads;
    int start, stop;
    omp_set_num_threads(num_threads);

    #pragma omp parallel private(id, i, j)
    {
        printf("\n%d\n", omp_get_thread_num());
        start = id*Mc;
        stop  = start + Mc;
        for(i = start; i < stop; i++)
            for(j=0; j<k; j++)
            {
                // Loop to compute the each element of C array
                *(*(C+i) + j) = 0;
                for(z=0; z<n; z++)
                {
                    *(*(C+i) + j) += *(*(A+i)+z) * *(*(B+z)+j);
                }
            }
    }
}

void parallel_column_result_dot(int **A, int **B, int **C)
{
    omp_set_num_threads(k);
    #pragma omp parallel private(id, i, j)
    {
        printf("\n%d\n", omp_get_thread_num());
        for(i = 0; i < m; i++)
        {
            *(*(C+i) + id) = 0;
            for(j = 0; j < n; j++)
            {
                *(*(C+i) + id) += *(*(A+i) + j) * *(*(B+j) + id);
            }
        }
    }
}


int main() 
{
    init();

    displayArray(A, m, n);
    displayArray(B, n, k);

    normal_dot(A, B, C);
    displayArray(C, m, k);
    
    parallel_row_result_dot(A, B, C);
    displayArray(C, m, k);
    
    parallel_column_result_dot(A, B, C);
    displayArray(C, m, k);

    return 0;
}
