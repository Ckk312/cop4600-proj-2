#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_NAME_LEN 50
#define HASH_SIZE 100

// Define a node structure for the hash map
typedef struct Employee {
    char name[MAX_NAME_LEN];
    uint32_t salary;
    struct Employee *next;
} Employee;

// Define the hash map structure
typedef struct HashMap {
    Employee *buckets[HASH_SIZE];
    pthread_mutex_t mutex;
    bool all_inserts_done;
    pthread_cond_t cond_var;
    int insert_threads_done;  // Count of threads that have finished inserting
    int total_insert_threads; // Total threads that are supposed to insert
} HashMap;

// Global hash map
HashMap hashmap = { .all_inserts_done = false, .insert_threads_done = 0, .total_insert_threads = 5 };

// Hash function to map the name to a bucket index
unsigned int hash(const char *name) {
    unsigned int hash_value = 0;
    while (*name) {
        hash_value = (hash_value * 31) + *name++;
    }
    return hash_value % HASH_SIZE;
}

// Function to insert an employee into the hash map
void insert(HashMap *map, const char *name, uint32_t salary) {
    unsigned int index = hash(name);
    Employee *new_employee = (Employee *)malloc(sizeof(Employee));
    if (new_employee) {
        strncpy(new_employee->name, name, MAX_NAME_LEN);
        new_employee->salary = salary;
        new_employee->next = map->buckets[index];
        map->buckets[index] = new_employee;
    }
}

// Function to search for an employee by name
void search(HashMap *map, const char *name) {
    unsigned int index = hash(name);
    Employee *current = map->buckets[index];
    while (current) {
        if (strcmp(current->name, name) == 0) {
            printf("Found %s with salary %u\n", name, current->salary);
            return;
        }
        current = current->next;
    }
    printf("Employee %s not found\n", name);
}

// Function to delete an employee by name
void delete(HashMap *map, const char *name) {
    unsigned int index = hash(name);
    Employee *current = map->buckets[index];
    Employee *prev = NULL;

    while (current) {
        if (strcmp(current->name, name) == 0) {
            if (prev) {
                prev->next = current->next;
            } else {
                map->buckets[index] = current->next;
            }
            free(current);
            printf("Deleted %s\n", name);
            return;
        }
        prev = current;
        current = current->next;
    }
    printf("Employee %s not found for deletion\n", name);
}

// Thread function to process commands
void *process_command(void *arg) {
    char command[20], name[MAX_NAME_LEN];
    uint32_t number;
    FILE *file = (FILE *)arg;

    while (fscanf(file, "%19[^,],%49[^,],%u\n", command, name, &number) != EOF) {
        pthread_mutex_lock(&hashmap.mutex);

        if (strcmp(command, "insert") == 0) {
            // Insert operation
            insert(&hashmap, name, number);
            printf("Inserted %s with salary %u\n", name, number);
            hashmap.insert_threads_done++;

            // If all insert threads are done, signal the delete threads
            if (hashmap.insert_threads_done == hashmap.total_insert_threads) {
                hashmap.all_inserts_done = true;
                pthread_cond_broadcast(&hashmap.cond_var);
            }

        } else if (strcmp(command, "delete") == 0) {
            // Delete operation
            if (!hashmap.all_inserts_done) {
                printf("Delete thread sleeping until all insert threads are done.\n");
                while (!hashmap.all_inserts_done) {
                    pthread_cond_wait(&hashmap.cond_var, &hashmap.mutex);
                }
                printf("Delete thread woke up, proceeding with delete.\n");
            }
            delete(&hashmap, name);
        } else if (strcmp(command, "search") == 0) {
            search(&hashmap, name);
        } else {
            printf("Unknown command: %s\n", command);
        }

        pthread_mutex_unlock(&hashmap.mutex);
    }

    return NULL;
}

// Main function to initiate processing
int main() {
    FILE *file = fopen("commands.txt", "r");
    if (!file) {
        printf("Error opening file.\n");
        return 1;
    }

    pthread_t threads[5];
    int i = 0;

    // Initialize mutex and condition variable
    pthread_mutex_init(&hashmap.mutex, NULL);
    pthread_cond_init(&hashmap.cond_var, NULL);

    // Create threads to process commands
    while (i < 5) {
        pthread_create(&threads[i], NULL, process_command, (void *)file);
        i++;
    }

    // Wait for threads to finish
    for (int i = 0; i < 5; i++) {
        pthread_join(threads[i], NULL);
    }

    // Clean up
    pthread_mutex_destroy(&hashmap.mutex);
    pthread_cond_destroy(&hashmap.cond_var);
    fclose(file);

    return 0;
}
