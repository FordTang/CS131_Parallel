#include "mpi.h"
#include <stdio.h>

int th_id;
int num_th;

void my_barrier()
{
	int i, temp;
	MPI_Status status;

	if (0 == th_id)
	{
		for (i = 1; i < num_th; ++i)
		{
			MPI_Recv(&temp, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
			printf("%d received message from %d\n", th_id, i);
		}

		for (i = 1; i < num_th; ++i)
		{
			MPI_Send(&i, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			printf("%d sent message to %d\n", th_id, i);
		}
	}
	else
	{
		MPI_Send(&th_id, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
		printf("%d sent message to 0\n", th_id);
		MPI_Recv(&temp, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		printf("%d received message from 0\n", th_id);
	}
}



int main( int argc, char * argv[] )
{
	//int argc;
	//char *argv;

	//int th_id;
	//int num_th;
	
	//char *relative_path_to_the_input_file;
	//char *relative_path_to_the_output_file;
	
	//relative_path_to_the_input_file = argv[1];
	//relative_path_to_the_output_file= argv[2];
	
	MPI_Init( &argc, &argv );
	MPI_Comm_rank( MPI_COMM_WORLD, &th_id );
	MPI_Comm_size( MPI_COMM_WORLD, &num_th );
	
	// **** Your SPMD program goes here ****
	
	double t1 = MPI_Wtime(); // start timer


	my_barrier();


	// End Timer
	double t2 = MPI_Wtime();

	if (0 == th_id)
	{
		//printf("t1: %f, t2: %f\n", t1, t2);
		printf("Total execution time: %f ms\n", (t2-t1) * 1000);
	}

	MPI_Finalize();
	return 0;
}
