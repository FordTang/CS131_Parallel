//Ford Tang
//46564602
//CS 131

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


struct data
{
    char** lines;
    unsigned int slice_size;
    unsigned int number_of_lines;
    unsigned int number_of_threads;
    unsigned int number_of_slices;;
    char search_string[BUFSIZ];
};


struct arg_struct
{
    unsigned int thread_number;
    struct data *data;
};


void read_file(char *file, struct data *data)
{
    FILE *input_file = fopen(file, "r");

	//data->search_string = (char *)calloc((BUFSIZ), sizeof(char));

    char line_buffer[BUFSIZ];
    unsigned int length;

    data->number_of_threads = atoi(fgets(line_buffer, BUFSIZ, input_file));
    data->number_of_slices = atoi(fgets(line_buffer, BUFSIZ, input_file));
    fgets(data->search_string, BUFSIZ, input_file);

    while(fgets(line_buffer, BUFSIZ, input_file))
    {
        if (line_buffer[0] != '\n')
        {
            ++data->number_of_lines;
            if (strlen(line_buffer) > length)
                length = strlen(line_buffer);
        }
    }

    data->lines = (char**)calloc(data->number_of_lines, sizeof(char*)); // allocating number of lines

    input_file = fopen(file, "r");
    fgets(line_buffer, BUFSIZ, input_file);
    fgets(line_buffer, BUFSIZ, input_file);
    fgets(line_buffer, BUFSIZ, input_file);

	unsigned int i;

    for (i = 0; i < data->number_of_lines; ++i)
    {
        data->lines[i] = (char*)calloc(length, sizeof(char)); // allocating length of line
        strcpy(data->lines[i], fgets(line_buffer, BUFSIZ, input_file));
    }

    data->slice_size = data->number_of_lines / data->number_of_slices;

    printf("Slice Size: %d\nNumber of Lines: %d\nNumber of Threads: %d\nNumber of Slices: %d\nSearch String: %s\n", data->slice_size, data->number_of_lines, data->number_of_threads, data->number_of_slices, data->search_string);
}


void* search_function(void *arg_struct)
{
    struct arg_struct *argument = (struct arg_struct *)arg_struct;

    unsigned char found = 0;

    FILE *output_file = fopen("output.txt", "a");

    unsigned int end = 0;

    if (argument->thread_number == (argument->data->number_of_threads - 1) && ((argument->thread_number + 1) * argument->data->slice_size) != argument->data->number_of_lines)
        end = argument->data->number_of_lines;
    else
        end = (argument->thread_number + 1) * argument->data->slice_size;

    printf("Thread Number: %d\nStart: %d\nEnd: %d\n", argument->thread_number, argument->thread_number * argument->data->slice_size, end);

	unsigned int i;

    for (i = argument->thread_number * argument->data->slice_size; i < end; ++i)
        if (!strcmp(argument->data->search_string, argument->data->lines[i]))
        {
            found = 1;
            fprintf(output_file, "Thread %d, found yes, slice %d, position %d\n", argument->thread_number, argument->thread_number, i);
            printf("Thread %d, found yes, slice %d, position %d\n", argument->thread_number, argument->thread_number, i);
        }
    if (!found)
    {
        fprintf(output_file, "Thread %d, found no, slice -1, position -1\n", argument->thread_number);
        printf("Thread %d, found no, slice -1, position -1\n", argument->thread_number);
    }

	fclose(output_file);

    return argument;
}


int main(int argc, char *argv[])
{
    struct data data;

    read_file(argv[1], &data);

    struct arg_struct argument[data.number_of_threads];

	unsigned int i;

    for (i = 0; i < data.number_of_threads; ++i)
    {
        argument[i].thread_number = i;
        argument[i].data = &data;

        pthread_t thread;

        int thread_status = pthread_create(&thread, NULL, search_function, (void *) &argument[i]);
        if (thread_status)
        {
            printf("ERROR; return code from pthread_create() is %d\n", thread_status);
            exit(-1);
        }
    }
    printf("Finished creating threads\n");
    pthread_exit(NULL);

    return 0;
}

