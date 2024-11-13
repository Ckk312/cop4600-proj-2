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
pthread_cond_t state_cond;
pthread_mutex_t write_lock, delete_lock;
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

void insert_lock_acquire()
{
	printf("%ld: WRITE LOCK ACQUIRED\n", time(NULL));
	pthread_mutex_lock(&write_lock);
	if (queues[INSERT]++ == 1)
	{
		pthread_mutex_lock(&delete_lock);
	}
}

void insert_lock_release()
{
	if (queues[INSERT]-- == 0)
	{
		pthread_cond_signal(&state_cond);
		pthread_mutex_unlock(&delete_lock);
	}
	printf("%ld: WRITE LOCK RELEASED\n", time(NULL));
	pthread_mutex_unlock(&write_lock);
}

void search_lock_acquire()
{
	queues[READ]++;
	pthread_mutex_lock(&write_lock);
	printf("%ld: READ LOCK ACQUIRED\n", time(NULL));
}

void search_lock_release()
{
	queues[READ]--;
	pthread_mutex_unlock(&write_lock);
	printf("%ld: READ LOCK RELASED\n", time(NULL));
}

void delete_lock_acquire()
{
	pthread_mutex_lock(&delete_lock);
	if (queues[INSERT] > 0 || hash_record_head == NULL)
	{
		printf("%ld: WAITING ON INSERTS\n", time(NULL));
		pthread_cond_wait(&state_cond, &delete_lock);
		printf("%ld: DELETE AWAKENED\n", time(NULL));
	}
	pthread_mutex_lock(&write_lock);
	printf("%ld: WRITE LOCK ACQUIRED\n", time(NULL));
}

void delete_lock_release()
{
	queues[DELETE]--;
	pthread_cond_signal(&state_cond);
	pthread_mutex_unlock(&delete_lock);
	pthread_mutex_unlock(&write_lock);
	printf("%ld: WRITE LOCK RELEASED\n", time(NULL));
}

#endif