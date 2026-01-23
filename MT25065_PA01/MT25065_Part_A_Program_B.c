#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "MT25065_Part_B_workers.h"


typedef struct {
    const char *worker_type;
} thread_arg_t;

static void* thread_start(void *arg)
{
    thread_arg_t *targ = (thread_arg_t *)arg;

    if (strcmp(targ->worker_type, "cpu") == 0) {
        cpu_worker();
    } else if (strcmp(targ->worker_type, "mem") == 0) {
        mem_worker();
    } else if (strcmp(targ->worker_type, "io") == 0) {
        io_worker();
    } else {
        fprintf(stderr, "Unknown worker type: %s\n", targ->worker_type);
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <cpu|mem|io> [num_threads]\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *worker_type = argv[1];
    int num_threads = 2; 

    if (argc >= 3) {
        num_threads = atoi(argv[2]);
        if (num_threads <= 0) {
            fprintf(stderr, "Invalid number of threads\n");
            return EXIT_FAILURE;
        }
    }

    pthread_t *threads = malloc(sizeof(pthread_t) * num_threads);
    if (threads == NULL) {
        perror("malloc failed");
        return EXIT_FAILURE;
    }

    thread_arg_t arg;
    arg.worker_type = worker_type;

    for (int i = 0; i < num_threads; i++) {
        if (pthread_create(&threads[i], NULL, thread_start, &arg) != 0) {
            perror("pthread_create failed");
            free(threads);
            return EXIT_FAILURE;
        }
    }

    // Wait for all worker threads to end
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    free(threads);
    return EXIT_SUCCESS;
}
