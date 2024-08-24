#include <stdio.h>
#include <stdlib.h>
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
void *tail;

void my_malloc_init() {
    head = (struct block*)STACK;
    head->free = true;
    head->next = NULL;
    head->prev = NULL;
    head->size = 0;

    tail = (void*)&(STACK[STACK_SIZE-1]);

    printf("Head address: %p\n", head);
    printf("Tail address: %p\n", tail);
}

bool can_allocate(struct block *b, size_t size) {
    size_t alloc = size + sizeof(struct block);
    if (!b->free) {
        printf("not free\n");
        return false;
    }

    if (b->next != NULL)
        printf("1remaining size: %ld (%ld)\n", (size_t)(b->next - b), alloc);
    else
        printf("2remaining size: %ld (%ld)\n", (size_t)((void*)STACK+STACK_SIZE - (void*)b), alloc);
    fflush(stdout);
    return b->free && (
        (size <= b->size)
        ||
        (b->next != NULL && (size_t)(b->next - b) >= alloc)
        ||
        (b->next == NULL && ((size_t)((void*)STACK+STACK_SIZE - (void*)b) >= alloc)));
}

void *my_malloc (size_t size) {
    struct block *curr = head;

    while (!can_allocate(curr, size)) {
        fflush(stdout);
        if (curr->next != NULL)
            curr = curr->next;
        else {
            struct block *new = (struct block*)((void*) curr + sizeof(struct block) + curr->size);
            curr->next = new;
            new->prev = curr;
            new->size = 0;
            new->next = NULL;
            new->free = true;
            curr = new;
        }
        fflush(stdout);
    }


    curr->free = false;
    curr->size = size;
    printf("allocating %ld bytes in %p, head: %p, tail: %p\n", size+sizeof(struct block), curr, head, tail);
    return ((void*)curr)+sizeof(struct block);
}

void merge (struct block *b1, struct block *b2) {  
    printf("j\n");
    if (b2 == NULL) {
        printf("k1\n");
        return;
    }
    if (b1 == NULL) {
        printf("k2\n");
        return;
    }
    if (!b1->free) {
        printf("k3\n");
        return;
    }
    if (!b2->free) {
        printf("k4\n");
        return;
    }
    printf("k5\n");
    fflush(stdout);
    if (b1 == NULL || b2 == NULL || !b1->free || !b2->free)
        return;
    printf("c\n");
    if (b1->next == b2 && b1 == b2->prev) {
        printf("d\n");
        if (b2->next != NULL) {
            b1->next = b2->next;   
            b1->next->prev = b1;
        }
    }
    else if (b1->prev == b2 && b1 == b2->next) {
        printf("e\n");
        if (b2->prev != NULL) {
            b1->prev = b2->prev;
            b1->prev->next = b1;
        }
    }
    else {
        printf("Error in my_malloc: inconsistent state of the heap\n");
        exit(EXIT_FAILURE);
    }
    b1->size = b1->size + b2->size + sizeof(struct block);
}

void compact (struct block *b1) {
    printf("a\n");
    merge(b1, b1->next);
    printf("b\n");
    merge(b1, b1->prev);
    printf("sal\n");
    fflush(stdout);
}

void my_free (void *ptr) {
    printf("freeing %p\n", ptr);
    struct block *b = ptr - sizeof(struct block);
    printf("2\n");
    b->free = true;
    printf("3\n");
    compact(b);
    printf("4\n");
    fflush(stdout);
}