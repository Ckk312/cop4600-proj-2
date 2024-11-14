#ifndef CHASH__
#define CHASH__

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <string.h>

// globals
size_t queues[3] = {0, 0, 0};
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

uint64_t current_timestamp()
{
	struct timeval te;
	gettimeofday(&te, NULL);
	uint64_t microseconds = ((uint64_t)te.tv_sec * 1000000LL) + te.tv_usec;
	return microseconds;
}

void insert_lock_acquire()
{
	puts("aaaa");
	if (++(queues[INSERT]) == 1)
	{
		puts("bbbb");
		pthread_mutex_lock(&delete_lock);
	}
	puts("cccc");
	printf("%lld: 1WRITE LOCK ACQUIRED - %d\n", current_timestamp(), pthread_mutex_lock(&write_lock));
}

void insert_lock_release()
{
	puts("dddd");
	if (--(queues[INSERT]) == 0)
	{
		puts("eeee");
		pthread_cond_signal(&state_cond);
		pthread_mutex_unlock(&delete_lock);
	}
	printf("%lld: 1WRITE LOCK RELEASED - %d\n", current_timestamp(), pthread_mutex_unlock(&write_lock));
}

void search_lock_acquire()
{
	queues[READ]++;
	pthread_mutex_lock(&write_lock);
	printf("%lld: 2READ LOCK ACQUIRED\n", current_timestamp());
}

void search_lock_release()
{
	queues[READ]--;
	pthread_mutex_unlock(&write_lock);
	printf("%lld: 2READ LOCK RELASED\n", current_timestamp());
}

void delete_lock_acquire(char * string)
{
	if (queues[INSERT] > 0 || hash_record_head == NULL)
	{
		printf("%lld: WAITING ON INSERTS\n", current_timestamp());
		pthread_cond_wait(&state_cond, &delete_lock);
		printf("%lld: DELETE AWAKENED\n", current_timestamp());
	}
	printf("%lld: DELETE, %s\n", current_timestamp(), string);
	pthread_mutex_lock(&write_lock);
	printf("%lld: 3WRITE LOCK ACQUIRED\n", current_timestamp());
}

void delete_lock_release()
{
	queues[DELETE]--;
	pthread_mutex_unlock(&write_lock);
	printf("%lld: 3WRITE LOCK RELEASED\n", current_timestamp());
}

#endif