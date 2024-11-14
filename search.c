#ifndef SEARCH__
#define SEARCH__
#include "chash.h"

void * search_hash_r(void * hash_record)
{
    hashRecord * hash_rec = (hashRecord *) hash_record;
    hash_rec->hash = jenkins_hash(hash_rec->name, strlen(hash_rec->name));

    search_lock_acquire();
    
    hashRecord * cur = hash_record_head, * found;
    // loop through linked list
    while (cur != NULL)
    {
        if (hash_rec == NULL)
            break;
        if (cur->hash == hash_rec->hash)
        {
            found = cur;
            break;
        }
        cur = cur->next;
    }

    if (found == NULL)
        printf("%ld: SEARCH: NOT FOUND, NOT FOUND\n", time(NULL));
    else
        printf("%ld: %s, %d\n", time(NULL), found->name, found->salary);

    search_lock_release();

    return cur;
}

#endif