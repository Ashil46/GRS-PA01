#define _GNU_SOURCE
#include "MT25065_Part_B_workers.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>
#include <time.h>


#define ROLL_NO_COUNT 5000 

#define CPU_BURST_ITER 80000 

void cpu_worker(void) {
    volatile double result = 1.0;
    for (int i = 0; i < ROLL_NO_COUNT; i++) {
        for (int j = 0; j < CPU_BURST_ITER; j++) {
            
            result = sqrt(result + j) + sin(j) + cos(j);
        }
    }
}

void mem_worker(void) {
    
    const size_t array_size = 220 * 1024 * 1024;
    volatile char *buffer = (char *)malloc(array_size);
    if (!buffer) return;

    //ensure pages are mapped to physical memory
    memset((void*)buffer, 0, array_size);

    for (int i = 0; i < ROLL_NO_COUNT; i++) {
        /*Access one byte every 4096 bytes 
            stresses  memory controller and TLB without burning pure CPU cycles 
        */
        for (size_t k = 0; k < array_size; k += 4096) {
            buffer[k] = (char)(i % 128);
        }
        usleep(3000);
    }
    free((void*)buffer);
}

void io_worker(void) {
    const char *filename = "io_worker_temp.txt";
    int fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC | O_SYNC, 0644);
    if (fd < 0) return;

    const size_t block_size = 64 * 1024;    
    const size_t file_range = 100 * 1024 * 1024; 

    char *buffer = (char *)malloc(block_size);
    if (!buffer) { close(fd); return; }
    memset(buffer, 'Z', block_size);
    
    srand(42); 
    int total_iterations = ROLL_NO_COUNT * 5;

    for (int i = 0; i < total_iterations; i++) {
        off_t offset = rand() % (file_range - block_size);
        
        if (lseek(fd, offset, SEEK_SET) < 0) break;
        if (write(fd, buffer, block_size) < 0) break;
    }

    free(buffer);
    close(fd);
    unlink(filename);
}