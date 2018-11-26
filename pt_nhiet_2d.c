#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include <time.h>

#define  m       20
#define  n       20
#define  T       2
#define  dt      0.01
#define  dx      0.1
#define  D       0.1
//=========================
void DisplayMatrix(float *A, int row,  int col)
{
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
void FD(float *C, float *dC)
{
    int i, j;
    float c,u,d,l,r;
    for (  i = 0 ; i < m ; i++ )
        for ( j = 0 ; j < n ; j++ )
        {
            c = *(C+i*n+j);
            u = (i==0)   ? 25 : *(C+(i-1)*n+j);
            d = (i==m-1) ? 25 : *(C+(i+1)*n+j);
            l = (j==0)   ? 25 : *(C+i*n+j-1);
            r = (j==n-1) ? 25 : *(C+i*n+j+1);
            *(dC+i*n+j) = (D/(dx*dx))*(u+d+l+r-4*c);
        }
}
//=========================
int main()
{
    int i,j, count;
    float t;
    t=0;
    float *C,*dC;
    C = (float *) malloc ((m*n)*sizeof(float));
    dC = (float *) malloc ((m*n)*sizeof(float));
    KhoiTao(C);

    count=0;
    while (t<=T)
    {
        FD(C, dC);
        for (  i = 0 ; i < m ; i++ )
            for ( j = 0 ; j < n ; j++ )
                *(C+i*n+j) = *(C+i*n+j) + dt*(*(dC+i*n+j));
        t=t+dt;
        count=count+1;
    }
    FD(C, dC);

    printf("Gia tri cuoi cung:\n");
    DisplayMatrix(C, m, n);
    free(C);
    free(dC);
    return 0;
}
