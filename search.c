#ifndef SEARCH__
#define SEARCH__
#include "chash.h"

void * search_hash_r(void * hash_record)
{
    hashRecord * hash_rec = (hashRecord *) hash_record;
    hash_rec->hash = jenkins_hash(hash_rec->name, strlen(hash_rec->name));
    printf("%ld: SEARCH, %s\n", time(NULL), hashRecord_toString(hash_rec));
    fprintf(output_file, "%ld: SEARCH, %s\n", time(NULL), hashRecord_toString(hash_rec));

    queues[READ]++;
    pthread_mutex_lock(&write_lock);
    printf("%ld: READ LOCK ACQUIRED\n", time(NULL));
    fprintf(output_file, "%ld: READ LOCK ACQUIRED\n", time(NULL));

    if (current_state == NONE)
        current_state = READ;

    while (current_state != NONE && current_state != READ)
    {
        printf("%ld: WAITING ON %s\n", time(NULL), (current_state == INSERT ? "INSERTS" : "DELETES"));
        fprintf(output_file, "%ld: WAITING ON %s\n", time(NULL), (current_state == INSERT ? "INSERTS" : "DELETES"));
        pthread_cond_wait(&state_cond, NULL);
    }

    if (current_state == READ)
    {
        if (queues[READ]-- == 0)
            current_state = NONE;
        pthread_cond_signal(&state_cond);
    }

    if (hash_record_head == NULL)
        return NULL;
    
    hashRecord * cur = hash_record_head;
    // loop through linked list
    while (cur != NULL)
    {
        if (cur->hash == hash_rec->hash)
            break;
        cur = cur->next;
    }

    pthread_mutex_unlock(&write_lock);
    printf("%ld: READ LOCK RELEASED\n", time(NULL));
    fprintf(output_file, "%ld: READ LOCK RELEASED\n", time(NULL));

    return (void *) cur;
}

#endif