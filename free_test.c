#include <stdio.h>
#include <stdlib.h>

#include "my_malloc.h"

#define SIZE 10000
#define MALLOC_SIZE 32

int main(int argc, char *argv[]) {
    my_malloc_init();

    size_t total_size = sizeof(void**) + (SIZE * sizeof(void*)) + (SIZE*MALLOC_SIZE) + (size_t)32 * (1+SIZE);

    printf("Total size: %ld\n", total_size);

    void **ptr;

    void *list[SIZE];

    for (int i = 0; i < 500; i++) {
        printf("starting test %d\n", i);
        ptr = (void**)my_malloc(sizeof(void*) * SIZE);
        for (int j = 0; j < SIZE; j++) {
            fflush(stdout);
            ptr[j] = my_malloc(MALLOC_SIZE);
            if (i==0)
                list[j] = ptr[j];
            else if (list[j] != ptr[j]) {
                printf("\n");
                printf("%p!=%p\n", list[j], ptr[j]);
                printf("incongruence in (%d,%d): %p != %p\n", i, j, list[j], ptr[j]);
                exit(EXIT_FAILURE);
            }
        }
        for (int j = SIZE-1; j >= 0; j--)
            my_free(ptr[j]);

        my_free(ptr);
    }
    printf("All tests correct\n");
    exit(EXIT_SUCCESS);
}