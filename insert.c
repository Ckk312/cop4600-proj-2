#ifndef INSERT__
#define INSERT__
#include "chash.h"

void * insert_hash_r(void * hash_record)
{
    hashRecord * hash_rec = (hashRecord *) hash_record;
    hash_rec->hash = jenkins_hash((uint8_t *) hash_rec->name, strlen(hash_rec->name));
    printf("%lld: INSERT,%u,%s,%d\n", current_timestamp(), hash_rec->hash, hash_rec->name, hash_rec->salary);

    insert_lock_acquire();
    
    hashRecord * cur = hash_record_head;
    if (cur == NULL)
    {
        hash_record_head = hash_rec;
        insert_lock_release();
        return NULL;
    }

    if (hash_rec->hash == hash_record_head->hash)
    {
        hash_rec->next = hash_record_head->next;
        hash_record_head = hash_rec;
        insert_lock_release();
        return NULL;
    }

    if (hash_rec->hash < hash_record_head->hash)
    {
        hash_rec->next = hash_record_head;
        hash_record_head = hash_rec;
        insert_lock_release();
        return NULL;
    }

    // loop through linked list
    while (cur->next == NULL)
    {
        if (hash_rec->hash < cur->next->hash)
        {
            hash_rec->next = cur->next;
            cur->next = hash_rec;
            break;
        }
        else if (hash_rec->hash == cur->next->hash)
        {
            hash_rec->next = cur->next->next;
            cur->next = hash_rec;
            break;
        }

        cur = cur->next;
    }

    if (cur->next == NULL)
    {
        cur->next = hash_rec;
        hash_rec->next = NULL;
    }

    insert_lock_release();

    return NULL;
}

#endif