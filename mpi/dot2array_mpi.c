#include<stdio.h>
#include<stdlib.h>
#include<omp.h>

int i, j, z;
int *A, *B, *C;
int *Ac, *Bc, *Cc;
const int m = 6, n= 7, k = 5;
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

void initMaster()
{
    /* allocate memory for A, B, C and init value for A, B*/
    // Allocate memory for A, B, C
    A = (int*) malloc(m*n * sizeof(int));
    B = (int*) malloc(n*k * sizeof(int));
    C = (int*) malloc(m*k * sizeof(int));

    initSlaver();

    // Init value for A and B
    for(i=0; i<m; i++)
        for(j=0; j<n; j++) {
            *(*(A+i)+j) = i+j;
        }

    for(i=0; i<n; i++)
        for(j=0; j<k; j++) {
           *(*(B+i)+j) = (i+j)*2;
        }
}

void initSlaver(int mc) {
    Ac = (int*) malloc(mc*n * sizeof(int));
    Bc = (int*) malloc(n*k * sizeof(int));
    Cc = (int*) malloc(mc*k * sizeof(int));
}

void normal_dot(int *A, int *B, int *C, int m, int n, int k)
{
    /* Dot 2 array by sequense method */
    for(i=0; i<m; i++)
    {
        for(j=0; j<k; j++)
        {
            // Loop to compute the each element of C array
            *(C + k*i + j) = 0;
            for(z=0; z<n; z++)
            {
                *(C + k*i + j) += *(A + k*i +z) * *(B + k*i +j);
            }
        }
    }
}






int main() 
{
    initMaster();

    displayArray(A, m, n);
    displayArray(B, n, k);

    normal_dot(A, B, C, m, n, k);
    displayArray(C, m, k);

    return 0;
}
