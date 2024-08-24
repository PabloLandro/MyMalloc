#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>  


#define STACK_SIZE 1024*1024

char STACK[STACK_SIZE];

struct block {
    struct block *prev;
    struct block *next;
    size_t size;
    bool free;
} block;

struct block *head;

void my_malloc_init() {
    head = (struct block*)STACK;
    head->free = true;
    head->size = 0;
}

bool can_allocate(struct block *b, size_t size) {
    size_t alloc = size + sizeof(struct block);
    return b->free && (
        (((size_t)(b->next - b)) >= alloc)
        ||
        (b->next == NULL && ((size_t)((void*)STACK+STACK_SIZE - (void*)b) >= alloc)));
}

void *my_malloc (size_t size) {
    struct block *curr = head;

    while (!can_allocate(curr, size)) {
        if (curr->next != NULL)
            curr = curr->next;
        else {
            struct block *new = curr + sizeof(struct block) + curr->size;
            curr->next = new;
            new->prev = curr;
            new->size = 0;
            new->next = NULL;
            new->free = true;
            curr = new;
        }
    }

    curr->free = false;
    curr->size = size;
    return curr+sizeof(struct block);
}

void my_free (void *ptr) {
    struct block *b = ptr - sizeof(struct block);
    b->free = true;
}