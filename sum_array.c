#include<stdio.h>
#include<stdlib.h>
#include<omp.h>

int *a, *temp, i, id, s;
int n = 100, num_threads, segment_size;

void init() 
{
    a = (int*)malloc(n * sizeof(int));
    for(i=0; i<n; i++)
    {
        *(a+i) = i;
    }
}

int sum_reduction(int *a, int len_a)
    {
    if(len_a == 1) return *a;

    int new_len;
    if (len_a % 2 == 0) new_len = len_a / 2;
    else new_len = len_a / 2 + 1;

    omp_set_num_threads(len_a / 2);
    #pragma omp parallel private(id)
    {
        id = omp_get_thread_num();
        *(a + id) = *(a + id) + *(a+ len_a - id -1);
    }

    return sum_reduction(a, new_len);
    }

int main() {
    init();
    int normal_sum = 0;
    for(i=0; i<n; i++)
    {
       normal_sum += *(a+i);
    }
    
    s = sum_reduction(a, n);

    printf("\nsum normal = %d\n", normal_sum);
    printf("\nsum parallel = %d\n", s);
    return 0;

}
