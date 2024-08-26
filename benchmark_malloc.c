#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#include "my_malloc.h"

#define NUM_TESTS 1000

#define N_MALLOCS 50
#define MALLOC_SIZE 1000



void benchmark (void* (*custom_malloc) (size_t size), void (*custom_free) (void* ptr), bool debug) {
    void **ptr = (void**)custom_malloc(sizeof(void*) * N_MALLOCS);

    if (debug)
        printf("starting mallocs\n");

    for (int i = 0; i < N_MALLOCS; i++) {
        ptr[i] = custom_malloc(MALLOC_SIZE);
    }

    if (debug)
        printf("starting frees\n");

    for (int i = 0; i < N_MALLOCS; i++) {
        custom_free(ptr[i]);
    }

    if (debug)
        printf("removing pointer\n");
    custom_free(ptr);
    fflush(stdout);

}

double get_time(void* (*custom_malloc) (size_t size), void (*custom_free) (void* ptr), bool debug) {

    clock_t start_time, end_time;
    double elapsed_time = 0;

    for (int i = 0; i < NUM_TESTS; i++) {
        if (debug)
            printf("Starting test %d\n", i);
        start_time = clock();
        benchmark(custom_malloc, custom_free, debug);
        end_time = clock();
        elapsed_time = (double) (end_time-start_time) / CLOCKS_PER_SEC;
    }
}

int main() {
    my_malloc_init();

    printf("Benchmark of standard malloc\n");
    printf("Average time: %lf\n", get_time(&malloc, &free, false));

    printf("Benchmark of my_malloc\n");
    printf("Average time: %lf\n", get_time(&my_malloc, &my_free, false));

}