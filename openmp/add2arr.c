#include<stdio.h>
#include<stdlib.h>
#include<omp.h>
#include <mpi.h>

int id, i;
int n = 100;
int *a;
int *b;
int *c;
int *d;
int domain_size;

void init() {
    a = (int *) malloc(n * sizeof(int));
    b = (int *) malloc(n * sizeof(int));
    c = (int *) malloc(n * sizeof(int));
    d = (int *) malloc(n * sizeof(int));
    domain_size = n / omp_get_max_threads();
    for(i=1; i<n; i++) {
        *(a+i) = i;
        *(b+i) = i * 10;
    }
}

void add_array_parallel(int *a, int *b, int *d) {
    #pragma omp parallel private(id, i)
    {
	printf("%d", omp_get_num_threads());
        id = omp_get_thread_num();
        for(i=id*domain_size; i<(id+1)*domain_size; i++) {
            *(d+i) = *(a+i) + *(b+i);
        }
    }

}

void add_array_single(int *a, int *b, int *c) {

    for(i=0; i<n; i++) {
        *(c+i) = *(a+i) + *(b+i);
    }

}

int main(int argc, char** argv) {

    init();



    add_array_single(a, b, c);

    add_array_parallel(a, b, d);

    


    for(id=1; id<100; id++) {
        printf("value with id = %d: %d, %d \n", id, *(c+id), *(d+id));
    }

    return 0;
}
