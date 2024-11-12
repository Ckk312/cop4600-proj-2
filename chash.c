#include "chash.h"
#include "insert.c"
#include "delete.c"
#include "search.c"

#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define BUFLEN 100

hashRecord ** info;

char ** strsplit(char * _Str, const char * _Delim)
{
	uint32_t length = strlen(_Str);
	char ** strarray = (char **) malloc(length * sizeof(char *));
	char * res = strtok(_Str, _Delim);
	for (size_t i = 0; res != NULL; i++)
	{
		strarray[i] = res;
		res = strtok(NULL, _Delim);
	}

	return strarray;
}

int main(int argc, char * argv[])
{
	char FILENAME[50] = "commands.txt";
	if (argc > 1)
		strcpy(FILENAME, argv[1]);
	
	FILE * input_file = fopen(FILENAME, "r");
	if (input_file == NULL)
	{
		printf("Could not read file, %s", FILENAME);
		exit(1);
	}

	size_t processes_number = 1, i = 0;
	char check[BUFLEN];
	char ** items;

	output_file = fopen("output.txt", "a");
	fgets(check, BUFLEN, input_file);
	items = strsplit(check, ",");
	processes_number = atoi(items[1]);

	info = (hashRecord **) malloc(processes_number * sizeof(hashRecord *));
	for (size_t j = 0; j < processes_number; j++)
		info[j] = (hashRecord *) malloc(sizeof(hashRecord));
	pthread_t thread_list[processes_number];

	printf("Running %d threads\n", processes_number);
	fprintf(output_file, "Running %d threads\n", processes_number);

	write_lock = read_lock = PTHREAD_MUTEX_INITIALIZER;
	state_cond = PTHREAD_COND_INITIALIZER;

	while (fgets(check, BUFLEN, input_file))
	{
		items = strsplit(check, ",");

		info[i]->name = items[1];
		info[i]->salary = (uint32_t) atoi(items[2]);

		if (strcmp(items[0], "insert") == 0)
			pthread_create(&thread_list[i], NULL, insert_hash_r, info[i]);

		else if (strcmp(items[0], "delete") == 0)
			pthread_create(&thread_list[i], NULL, delete_hash_r, info[i]);

		else if(strcmp(items[0], "search") == 0)
			pthread_create(&thread_list[i], NULL, search_hash_r, info[i]);

		i++;
	}

	for (size_t j = 0; j < processes_number; j++)
	{
		pthread_join(thread_list[j], NULL);
		printf("joining thread %d\n", j);
	}

	fclose(input_file);

	puts("Finished all threads.");
	fprintf(output_file, "Finished all threads.\n");

	fclose(output_file);

	for(size_t j = 0; j < processes_number; j++)
		free(info[j]);
	free(info);

	return 0;
}