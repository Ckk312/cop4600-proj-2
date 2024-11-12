#ifndef CHASH__
#define CHASH__

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <string.h>

// globals
size_t queues[3];
int current_state = -1;
pthread_cond_t state_cond;
pthread_mutex_t write_lock, read_lock;
FILE * output_file;

// enum for state (should have used Semaphores but yk whatever)
typedef enum state
{
	NONE = -1,
	INSERT,
	DELETE,
	READ
} threadState;

typedef struct hash_struct
{
  uint32_t hash;
  char * name;
  uint32_t salary;
  struct hash_struct *next;
} hashRecord;

hashRecord * hash_record_head;

uint32_t jenkins_hash(const uint8_t * key, size_t length)
{
	uint32_t hash = 0;
	for (size_t i = 0; i < length; i++)
	{
		hash += key[i];
		hash += hash << 10;
		hash ^= hash >> 6;
	}
	hash += hash << 3;
	hash ^= hash >> 11;
	hash += hash << 15;
	return hash;
}

char * hashRecord_toString(hashRecord * hr)
{
	int length = snprintf(NULL, 0, "%d", (int) hr->hash);
	char * ret, * attach;
	ret = (char *) malloc((length + 1) * sizeof(char));
	attach = (char *) malloc((length + 1) * sizeof(char));
	snprintf(ret, length + 1, "%u", hr->hash);
	snprintf(attach, length + 1, "%d", hr->salary);
	strcat(ret, ",");
	strcat(ret, hr->name);
	strcat(ret, ",");
	strcat(ret, attach);
	free(attach);

	return ret;
}

#endif