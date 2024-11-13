#include "chash.h"
#include "insert.c"
#include "delete.c"
#include "search.c"

#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define BUFLEN 100

hashRecord ** hash_struct;

char ** strsplit(char ** _StrArray, char * _Str, const char * _Delim)
{
	char * res = strtok(_Str, _Delim);
	for (size_t i = 0; res != NULL; i++)
	{
		_StrArray[i] = res;
		res = strtok(NULL, _Delim);
	}

	return _StrArray;
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
	char * items[BUFLEN - 1];

	output_file = fopen("output.txt", "a");
	fgets(check, BUFLEN, input_file);
	strsplit(items, check, ",");
	processes_number = atoi(items[1]);

	hash_struct = (hashRecord **) malloc(processes_number * sizeof(hashRecord *));
	pthread_t thread_list[processes_number];

	printf("Running %d threads\n", processes_number);
	fprintf(output_file, "Running %d threads\n", processes_number);

	write_lock = delete_lock = PTHREAD_MUTEX_INITIALIZER;
	state_cond = PTHREAD_COND_INITIALIZER;

	while (fgets(check, BUFLEN, input_file))
	{
		hash_struct[i] = (hashRecord *) malloc(sizeof(hashRecord));
		strsplit(items, check, ",");

		printf("|| %s, %s, %d\n", items[0], items[1], atoi(items[2]));

		hash_struct[i]->name = items[1];

		if (strcmp(items[0], "insert") == 0) {
			hash_struct[i]->salary = atoi(items[2]);
			pthread_create(&thread_list[i], NULL, insert_hash_r, hash_struct[i]);
		}

		else if (strcmp(items[0], "delete") == 0)
			pthread_create(&thread_list[i], NULL, delete_hash_r, hash_struct[i]);

		else if(strcmp(items[0], "search") == 0)
			pthread_create(&thread_list[i], NULL, search_hash_r, hash_struct[i]);

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
	for (size_t j = 0; j < processes_number; j++)
		free(hash_struct[j]);
	free(hash_struct);

	return 0;
}