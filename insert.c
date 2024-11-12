#ifndef INSERT__
#define INSERT__
#include "chash.h"

void * insert_hash_r(void * hash_record)
{
    hashRecord * hash_rec = (hashRecord *) hash_record;
    hash_rec->hash = jenkins_hash((uint8_t *) hash_rec->name, strlen(hash_rec->name));
    printf("%ld: INSERT, %s\n", time(NULL), hashRecord_toString(hash_rec));
    fprintf(output_file, "%ld: INSERT, %s\n", time(NULL), hashRecord_toString(hash_rec));

    queues[INSERT]++;
    pthread_mutex_lock(&write_lock);
    printf("%ld: WRITE LOCK ACQUIRED\n", time(NULL));
    fprintf(output_file, "%ld: WRITE LOCK ACQUIRED\n", time(NULL));

    if (current_state == NONE)
        current_state = INSERT;

    while (current_state != NONE && current_state != INSERT)
    {
        printf("%ld: WAITING ON %s\n", time(NULL), (current_state == DELETE ? "DELETES" : "SEARCHES"));
        fprintf(output_file, "%ld: WAITING ON %s\n", time(NULL), (current_state == DELETE ? "DELETES" : "SEARCHES"));
        pthread_cond_wait(&state_cond, NULL);
        printf("INSERT AWAKENED\n");
        fprintf(output_file, "INSERT AWAKENED\n");
    }

    if (current_state == INSERT)
    {
        if (queues[INSERT]-- <= 0)
            current_state = NONE;
        pthread_cond_signal(&state_cond);
    }

    if (hash_record_head == NULL)
    {
        hash_record_head = hash_rec;
        return NULL;
    }
    
    hashRecord * cur = hash_record_head;
    hashRecord * tmp;
    // loop through linked list
    while (cur->next == NULL)
    {
        if (hash_rec->hash >= cur->next->hash)
        {
            tmp = cur->next;
            cur->next = hash_rec;
            hash_rec->next = tmp->next;
        }
    }

    pthread_mutex_unlock(&write_lock);
    printf("%ld: WRITE LOCK RELEASED\n", time(NULL));
    fprintf(output_file, "%ld: WRITE LOCK RELEASED\n", time(NULL));

    return NULL;
}

#endif