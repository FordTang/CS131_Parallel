#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



int main( int argc, char * argv[])
{
  //int argc;
  //char *argv;
    
  int th_id;
  int num_th;
    
  char *relative_path_to_the_input_file;
  char *relative_path_to_the_output_file;
    
  relative_path_to_the_input_file = argv[1];
  relative_path_to_the_output_file= argv[2];

  MPI_Init( &argc, &argv );
  MPI_Comm_rank( MPI_COMM_WORLD, &th_id );
  MPI_Comm_size( MPI_COMM_WORLD, &num_th );


  // **** Your SPMD program goes here ****
  char lines[100000][15];
  unsigned int slice_size = 100000 / num_th;
  char line_buffer[15];
  char lines_slice[slice_size][15];
  char search_string[15];
	int gather_array[num_th][2];
  int i;
	double t1 = MPI_Wtime(); // start timer


  // populate large array
  if (0 == th_id)
  {
    FILE *input_file;

		if (NULL != relative_path_to_the_input_file)
		  input_file = fopen(relative_path_to_the_input_file, "r");
		else
			input_file = fopen("partA.txt", "r");
    
    fgets(line_buffer, 15, input_file);	// skip process amount
    fgets(line_buffer, 15, input_file);	// skip slice amount
    fgets(search_string, 15, input_file);

    for (i = 0; i < 100000; ++i)
      fgets(lines[i], 15, input_file);

		for (i = 1; i < num_th; ++i)
		  MPI_Send(&search_string, 15, MPI_CHAR, i, 0, MPI_COMM_WORLD); // send search string to other processes

		fclose(input_file);
  }
    

	// other processes receive search string
	else
	{
		MPI_Status status;
		MPI_Recv(&search_string, 15, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
	}


  // distribute lines array to processes
  MPI_Scatter(&lines, slice_size * 15, MPI_CHAR, &lines_slice, slice_size * 15, MPI_CHAR, 0, MPI_COMM_WORLD);


  // processes search it's given small array       
  int data[2];
  data[0] = th_id;
  data[1] = -1;        	
        
  for (i = 0; i < slice_size; ++i)
    if (!strcmp(search_string, lines_slice[i]))
    {
      data[1] = (th_id * slice_size) + i;
      break;
    }


	// gather result back to process 0 (master)
	MPI_Gather(&data, 2, MPI_INT, &gather_array, 2, MPI_INT, 0, MPI_COMM_WORLD);


	// End Timer
	double t2 = MPI_Wtime();


  // process 0 (master) writes results to file
  if (th_id == 0)
	{
    //FILE *output_file;
		
		//if (NULL != relative_path_to_the_output_file)
		//	output_file = fopen(relative_path_to_the_output_file, "a");
		//else
		//	output_file = fopen("outputA.txt", "a");
       
  	for (i = 0; i < num_th; ++i)
  	{
		  if (-1 != gather_array[i][1])
			{
        //fprintf(output_file, "Process %d, found yes, slice %d, position %d\n", gather_array[i], gather_array[i], gather_array[i + 1]);
        printf("Process %d, found yes, slice %d, position %d\n", gather_array[i][0], gather_array[i][0], gather_array[i][1]);
			}
			else
			{
        //fprintf(output_file, "Process %d, found no, slice -1, position -1\n", gather_array[i]);
        printf("Process %d, found no, slice -1, position -1\n", gather_array[i][0]);
      }
		}

		//fprintf(output_file, "Total execution time: %d ms\n", t2-t1);
    //printf("t1: %f, t2: %f\n", t1, t2);
		printf("Total execution time: %f ms\n", (t2-t1) * 1000);
       			
    //fclose(output_file);
  }

	
	// Finish Processes
	MPI_Finalize();


  return 0;
}