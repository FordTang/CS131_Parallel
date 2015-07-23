// Ford Tang
// 46564602
// CS 131 - Parallel
// Lab 3


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

int main(int argc, char **argv)
{
	char* path_to_input_file = argv[1];
	char* path_to_output_file = argv[2];
	int number_of_threads = atoi(argv[3]);
	int number_of_chunks = atoi(argv[4]);

	printf("input file: %s\noutput file: %s\nnumber of threads: %d\nnumber of chunks: %d\n", path_to_input_file, path_to_output_file, number_of_threads, number_of_chunks);

	char array[100000][15];
	char search_string[15];
	int chunk_size = 100000 / number_of_chunks;
	int resultsA[number_of_threads][2];
	int resultsB[number_of_chunks][2];
 	int i, j;

	omp_set_num_threads(number_of_threads);

	FILE* input_file = fopen(path_to_input_file, "r");

	fgets(search_string, 15, input_file);	// skip process amount
	fgets(search_string, 15, input_file);	// skip slice amount
    fgets(search_string, 15, input_file);

	for (i = 0; i < 100000; ++i)
		fgets(array[i], 15, input_file);



 	#pragma omp parallel shared(array, resultsA) private(i)
 	{
 		int thread_num = omp_get_thread_num();
 		resultsA[thread_num][0] = thread_num;
 		resultsA[thread_num][1] = -1;

	   	#pragma omp for schedule(static)
	    for (i = 0; i < 100000; ++i)
	    	if (!strcmp(search_string, array[i]))
	    		resultsA[thread_num][1] = i;
 	}

 	if (0 == omp_get_thread_num())
	{
		FILE* output_file = fopen(path_to_output_file, "a");
		for (i = 0; i < number_of_threads; ++i)
			if (-1 != resultsA[i][1])
		    	fprintf(output_file, "Static: Thread %d, found yes, chunk %d, position %d\n", resultsA[i][0], i + 1, resultsA[i][1]);
	}



 	for (i = 0; i < number_of_chunks; ++i)
 		for (j = 0; j < 2; ++j)
 			resultsB[i][j] = -1;


 	#pragma omp parallel shared(array, chunk_size, resultsB) private(i)
 	{
		#pragma omp for schedule(dynamic, chunk_size)
	    for (i = 0; i < 100000; ++i)
	    {
	    	if (!strcmp(search_string, array[i]))
	    	{
	    		resultsB[i/chunk_size][0] = omp_get_thread_num();
	    		resultsB[i/chunk_size][1] = i;
	    	}
	    }
 	}

 	if (0 == omp_get_thread_num())
	{
		FILE* output_file = fopen(path_to_output_file, "a");
			for (i = 0; i < number_of_chunks; ++i)
				if (-1 != resultsB[i][1])
		        	fprintf(output_file, "Dynamic: Thread %d, found yes, chunk %d, position %d\n", resultsB[i][0], i + 1, resultsB[i][1]);
	}

    return 0;
}