#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int special = 0;

typedef struct urmom
{
    char * name;
    int number;
    int * spec;
} um;

um * args;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_sign = PTHREAD_COND_INITIALIZER;

void * func(void * arg)
{
    um * stuff = (um *) arg;
    printf("lock acquire? -> %d\n", pthread_mutex_lock(&lock));
    while ((*(stuff->spec) % 3) != 0)
    {
        *(stuff->spec)++;
        printf("condition wait? -> %d\n", pthread_cond_wait(&cond_sign, &lock));
    }
    *(stuff->spec)++;
    strcat(stuff->name, "x");
    stuff->number++;
    printf("woah hey %s there are %d of you.\n", stuff->name, stuff->number);
    printf("signal? -> %d\n", pthread_cond_signal(&cond_sign));
    printf("unlocking? -> %d\n", pthread_mutex_unlock(&lock));
    puts("Thread done.");
    return NULL;
}

int main()
{
    puts("start");
    pthread_t th[3];
    pthread_t blah;
    char somename[20] = "ale";

    args = (um *) malloc(sizeof(um));
    args->name = somename;
    args->number = 4;
    args->spec = &special;
    
    for (size_t i = 0; i < 3; i++)
        pthread_create(&th[i], NULL, func, args);

    puts("YOUR MOM");

     for (size_t i = 0; i < 3; i++)
        pthread_join(th[i], NULL);

    puts("LOL");

    free(args);
    return 0;
}