#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include <time.h>
#include <omp.h>


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
void Write2File(float *T, int size)
{
FILE *result=fopen("result2.txt", "a");
    int i;

    for(i=0;i<size;i++) 
    {
        fprintf(result, "%lf", *(T+i));
        fprintf(result, "\n");
    }

    fclose(result);
}
//=========================
void KhoiTao(float *T)
{
    int i,j;
    for (  i = 0 ; i < M ; i++ )
        *(T+i) = 25.0;
}
//=========================
void Daoham(float *T, float *dT, int start, int stop)
{
    int i;
    float c,l,r;

    for (  i = start ; i < stop ; i++ )
    {
        c = *(T+i);
        l = (i==0)   ? 100.0 : *(T+(i-1));
        r = (i==M-1) ? 30.0  : *(T+(i+1));
        *(dT+i) = (r-2*c+l)/(dx*dx);
    }


}
//=========================
int main()
{
    int i,t, Ntime;
    float *T,*dT;
    int id, n_threads;
    int start, stop;

    T  = (float *) malloc ((M)*sizeof(float));
    dT = (float *) malloc ((M)*sizeof(float));
    KhoiTao(T);
    printf("Gia tri khoi tao:\n");
    DisplayArray(T, M);
    Write2File(T,M);
    Ntime = Time/dt;

    omp_set_num_threads(4);
    n_threads = M / 4;

    #pragma omp parallel private(id, i, start, stop, t)
    {
        id = omp_get_thread_num();
        start = id * n_threads;
        stop  = start + n_threads;
        for (t=0;t<Ntime;t++)
        {
            #pragma omp barrier
            Daoham(T, dT, start, stop);
            #pragma omp barrier
            for (i = start ; i < stop ; i++ )
            {
                *(T+i) = *(T+i) + D*dt*(*(dT+i));
            }
            // Write2File(T,M);
        }
    }
    printf("Result of C:\n");
    DisplayArray(T, M);
    return 0;
}


  // 95.86  91.73  87.64  83.60  79.64  75.75  71.96  68.28  64.70  61.25  57.92  54.71  51.61  48.63  45.76  42.98  40.28  37.65  35.07  32.52
