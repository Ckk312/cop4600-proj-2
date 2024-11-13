#ifndef SEARCH__
#define SEARCH__
#include "chash.h"

void * search_hash_r(void * hash_record)
{
    hashRecord * hash_rec = (hashRecord *) hash_record;
    hash_rec->hash = jenkins_hash(hash_rec->name, strlen(hash_rec->name));
    printf("%ld: SEARCH, %s\n", time(NULL), hashRecord_toString(hash_rec));

    search_lock_acquire();
    
    hashRecord * cur = hash_record_head, * found;
    // loop through linked list
    while (cur != NULL)
    {
        if (cur->hash == hash_rec->hash)
        {
            found = cur;
            break;
        }
        cur = cur->next;
    }

    printf("%ld: SEARCH: ", time(NULL));
    if (found == NULL)
        printf("NOT FOUND, NOT FOUND\n");
    else
        printf("%s, %d\n", found->name, found->salary);

    search_lock_release();

    return cur;
}

#endif