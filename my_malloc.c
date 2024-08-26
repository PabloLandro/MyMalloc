#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>

#include "print_utils.h"


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
    printf("Block size: %ld\n", sizeof(struct block));
}

bool can_allocate(struct block *b, size_t size) {
    size_t alloc = size + sizeof(struct block);
    if (!b->free) {
        //printf("not free\n");
        return false;
    }

    // if (b->next != NULL)
    //     printf("1remaining size: %ld (%ld)\n", (size_t)(b->next - b), alloc);
    // else
    //     printf("2remaining size: %ld (%ld)\n", (size_t)((void*)STACK+STACK_SIZE - (void*)b), alloc);
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
            struct block *new = (struct block*)((void*)curr + sizeof(struct block) + curr->size);
            new->prev = curr;
            new->size = 0;
            new->next = NULL;
            new->free = true;
            curr->next = new;
            curr = new;
        }
    }

    curr->free = false;
    curr->size = size;
    if (curr->next != NULL && (void*)curr + size + sizeof(struct block) < (void*)curr->next) {
        struct block *aux = (struct block*)((void*)curr + size + sizeof(struct block));
        aux->next = curr->next;
        aux->prev = curr;
        aux->next->prev = aux;
        aux->free = true;
        aux->size = ((void*)aux->next - (void*)aux)-sizeof(struct block);
    }
    //printf("allocating %ld bytes in %p, head: %p, tail: %p\n", size+sizeof(struct block), curr, head, tail);
    return ((void*)curr)+sizeof(struct block);
}

void merge_forward (struct block *b1, struct block *b2) {
    b1->size = b1->size + b2->size + sizeof(struct block);
    if (b2->next != NULL) {
        b1->next = b2->next;
        (b1->next)->prev = b1;
    } else {
        b1->next = NULL;
    }
}

void compact (struct block *b1) {
    if (b1->next != NULL)
        merge_forward(b1, b1->next);
    if (b1->prev != NULL)
        merge_forward(b1->prev, b1);
}

void my_free (void *ptr) {
    struct block *b = ptr - sizeof(struct block);
    b->free = true;
    compact(b);
}

#define BLOCK_LEN 32

void print_state() {
    struct block *curr = head;
    while (curr != NULL) {
        printf("block %p {\n\tprev: %p\n\tnext: %p\n\tsize: %ld\n\tfree: %s\n}\n", curr, curr->prev, curr->next, curr->size, (curr->free) ? "true": "false");
        curr = curr->next;
    }
}