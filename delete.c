#ifndef DELETE__
#define DELETE__
#include <stdlib.h>

#include "chash.h"

void * delete_hash_r(void * hash_record)
{
    hashRecord * hash_rec = (hashRecord *) hash_record;
    hash_rec->hash = jenkins_hash(hash_rec->name, strlen(hash_rec->name));

    while (hash_record_head == NULL || queues[INSERT] > 0)
    {
        printf("%ld: WAITING ON INSERTS\n", time(NULL));
        fprintf(output_file, "%ld: WAITING ON INSERTS\n", time(NULL));
        queues[DELETE]++;
        pthread_cond_wait(&state_cond, &write_lock);
    }
    printf("%ld: DELETE, %s\n", time(NULL), hash_rec->name);
    fprintf(output_file, "%ld: DELETE, %s\n", time(NULL), hash_rec->name);

    //queues[DELETE]++;
    //pthread_mutex_lock(&write_lock);
    printf("%ld: WRITE LOCK ACQUIRED\n", time(NULL));
    fprintf(output_file, "%ld: WRITE LOCK ACQUIRED\n", time(NULL));

    current_state = DELETE;

    while (current_state != NONE && current_state != DELETE)
    {
        
        pthread_cond_wait(&state_cond, NULL);
    }

    if (current_state == DELETE)
    {
        if (queues[DELETE]-- == 0)
            current_state = NONE;
        pthread_cond_signal(&state_cond);
    }

    if (hash_record_head == NULL)
        return NULL;
    
    hashRecord * cur = hash_record_head;
    hashRecord * tmp;
    // loop through linked list
    while (cur->next == NULL)
    {
        if (hash_rec->hash == cur->next->hash)
        {
            tmp = cur->next;
            cur->next = cur->next->next;
            free(tmp);
        }
    }

    pthread_mutex_unlock(&write_lock);
    printf("%ld: WRITE LOCK RELEASED\n", time(NULL));
    fprintf(output_file, "%ld: WRITE LOCK RELEASED\n", time(NULL));

    return NULL;
}

#endif