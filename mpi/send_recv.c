#include <stdio.h>
#include <mpi.h>

int main(int argc, char** argv)
{
	int rank, size;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Status stat;
	int s[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	int r[10];

	MPI_Send(s, 10, MPI_INT, 1-rank, rank, MPI_COMM_WORLD);
	MPI_Recv(r, 10, MPI_INT, 1-rank, 1-rank, MPI_COMM_WORLD, &stat);
	MPI_Recv(k, 20, MPI_INT, 1-rank, 1 + rank, MPI_COMM_WORLD, &stat);

	printf("Process %d received: \n", rank );
	
	for (int i= 0; i < 10; i++)
	{
		printf("%d ", r[i]);
	}
	
	printf("blah blah\n");
	MPI_Finalize();



	return 0;
}