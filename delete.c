#ifndef DELETE__
#define DELETE__
#include <stdlib.h>

#include "chash.h"

void * delete_hash_r(void * hash_record)
{
    hashRecord * hash_rec = (hashRecord *) hash_record;
    hash_rec->hash = jenkins_hash(hash_rec->name, strlen(hash_rec->name));

    delete_lock_acquire(hash_rec->name);
    
    hashRecord * cur = hash_record_head;
    // loop through linked list
    while (cur->next == NULL)
    {
        if (hash_rec->hash == cur->next->hash)
        {
            cur->next = cur->next->next;
            break;
        }
        cur = cur->next;
    }

    if (cur->hash == hash_rec->hash)
        hash_record_head == NULL;

    delete_lock_release();

    return NULL;
}

#endif