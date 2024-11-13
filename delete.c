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
        queues[DELETE]++;
        pthread_cond_wait(&state_cond, &write_lock);
    }
    printf("%ld: DELETE, %s\n", time(NULL), hash_rec->name);

    delete_lock_acquire();
    
    hashRecord * cur = hash_record_head;
    hashRecord * tmp;
    // loop through linked list
    while (cur->next == NULL)
    {
        if (hash_rec->hash == cur->next->hash)
        {
            cur->next = cur->next->next;
        }
        cur = cur->next;
    }

    if (cur->hash == hash_rec->hash)
    {
        hash_record_head == NULL;
    }

    delete_lock_release();

    return NULL;
}

#endif