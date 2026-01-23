#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "MT25065_Part_B_workers.h"


static void run_worker(const char *worker)
{
    if (strcmp(worker, "cpu") == 0) {
        cpu_worker();
    } else if (strcmp(worker, "mem") == 0) {
        mem_worker();
    } else if (strcmp(worker, "io") == 0) {
        io_worker();
    } else {
        fprintf(stderr, "Unknown worker type: %s\n", worker);
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <cpu|mem|io> [num_processes]\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *worker_type = argv[1];
    int num_processes = 2; 

    if (argc >= 3) {
        num_processes = atoi(argv[2]);
        if (num_processes <= 0) {
            fprintf(stderr, "Invalid number of processes\n");
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < num_processes; i++) {
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork failed");
            return EXIT_FAILURE;
        }

        if (pid == 0) {
            // Child process executes the worker
            run_worker(worker_type);
            exit(EXIT_SUCCESS);
        }
        //Parent continues tocreate more children
    }

    //Parent waits for all child processes to endko
    for (int i = 0; i < num_processes; i++) {
        wait(NULL);
    }

    return EXIT_SUCCESS;
}
