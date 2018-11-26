#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define  m       40
#define  n       40
#define  T       2
#define  dt      0.001
#define  dx      0.1
#define  D       0.1

void write2file(float *C) {
    FILE * file;
    file = fopen("/home/thangnd/git/ltss/on_gk/result.txt", "w");
    for(int i=0;i<m;i++){
        for(int j=0;j<n;j++) fprintf(file, "%f ",*(C+i*n+j));
        fprintf(file, "\n");
    }
}

//=========================
void DisplayMatrix(float *A, int row,  int col)
{
    printf("m: %d\n", m);
    printf("n: %d\n", n);
    int i,j;
    for(i=0;i<row;i++){
        for(j=0;j<col;j++) printf(" %4.2f",*(A+i*col+j));
        printf("\n");
    }
}

//=========================
void KhoiTao(float *C)
{
    int i,j;
    for (  i = 0 ; i < m ; i++ )
        for ( j = 0 ; j < n ; j++ ){
            if (i>=(m/2-5)&&i<(m/2+5)&&j>=(n/2-5)&&j<(n/2+5)) 
                *(C+i*n+j) = 80.0;
            else
                *(C+i*n+j) = 25.0;
        }
}
//=========================
void FD(float *Cc, float *dC, float *Cu, float *Cd, int Mc) {
    int i, j;
    float c,u,d,l,r;
    for (  i = 0 ; i < Mc ; i++ ) {
        for ( j = 0 ; j < n ; j++ ) {
            c = *(Cc+i*n+j);
            u = (i==0)   ? *(Cu + j) : *(Cc+(i-1)*n+j);
            d = (i==Mc-1) ? *(Cd + j) : *(Cc+(i+1)*n+j);
            l = (j==0)   ? 25 : *(Cc+i*n+j-1);
            r = (j==n-1) ? 25 : *(Cc+i*n+j+1);
            *(dC+i*n+j) = (D/(dx*dx))*(u+d+l+r-4*c);
        }
    }
}
//=========================
int main(int argc, char **argv) {
    int i,j, count;
    float t = 0.0;
    float *C,*dC;
    float *Cc, *Cu, *Cd;
    int Mc, temp;

    int size, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Status status;

    // Init
    Mc = m / size;
    Cc = (float *) malloc ((Mc * n)*sizeof(float));
    dC = (float *) malloc ((Mc * n)*sizeof(float));
    Cu = (float *) malloc (n * sizeof(float));
    Cd = (float *) malloc (n * sizeof(float));

    if(rank == 0) {
        C = (float *) malloc ((m*n)*sizeof(float));
        KhoiTao(C);
    }

    // Decomposition
    MPI_Scatter(C, Mc*n, MPI_FLOAT,
                Cc, Mc*n, MPI_FLOAT, 0, MPI_COMM_WORLD);

    // Computation
    count=0;
    while (t <= T)
    {
        if(rank != size - 1) {
            temp = (Mc - 1) * n;
            MPI_Send(Cc + temp, n, MPI_FLOAT,
                    rank + 1, rank + 1, MPI_COMM_WORLD);
            MPI_Recv(Cd, n, MPI_FLOAT,
                    rank + 1, rank + 2*size, MPI_COMM_WORLD, &status);
        } else {
            for(int i=0; i < n; ++i) {
                *(Cd + i) = 25.0;
            }
        }
        if(rank != 0) {
            MPI_Recv(Cu, n, MPI_FLOAT,
                    rank - 1, rank, MPI_COMM_WORLD, &status);
            MPI_Send(Cc, n, MPI_FLOAT,
                    rank - 1, rank - 1 + 2*size, MPI_COMM_WORLD);
        } else {
            for(int i=0; i < n; ++i) {
                *(Cu + i) = 25.0;
            }
        }


        FD(Cc, dC, Cu, Cd, Mc);
        for (  i = 0 ; i < Mc ; i++ )
            for ( j = 0 ; j < n ; j++ )
                *(Cc+i*n+j) = *(Cc+i*n+j) + dt*(*(dC+i*n+j));
        t = t+dt;
        count = count+1;
    }

    // Collect data
    MPI_Gather(Cc, Mc*n, MPI_FLOAT,
                C, Mc*n, MPI_FLOAT, 0, MPI_COMM_WORLD);  


    if(rank == 0) {
        printf("Gia tri cuoi cung:\n");
        DisplayMatrix(C, m, n);
        write2file(C);
    }
    
    MPI_Finalize();
    return 0;
}
