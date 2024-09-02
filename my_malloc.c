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


/**
 * @brief Instantiates the first block of memory
 *
 * This function must be called before using any of the other functions.
 *
 */
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

/**
 * @brief Returns true if the block can fit the given bytes
 *
 * @param[in]  b The block to be checked.
 * @param[in]  size The number of bytes.
 * @return True if b can allocate.
 */
bool can_allocate(struct block *b, size_t size) {
    size_t alloc = size + sizeof(struct block);

    return b->free && (
        (size <= b->size)
        ||
        (b->next != NULL && (size_t)(b->next - b) >= alloc)
        ||
        (b->next == NULL && ((size_t)((void*)STACK+STACK_SIZE - (void*)b) >= alloc)));
}


/**
 * @brief Allocates a block of the given stack in the stack
 *
 * Searches the first available block starting from the head.
 *
 * @param[in]  size The size to be allocated.
 * @return Pointer to the start of the usable memory.
 */
void *my_malloc (size_t size) {

    // Start the search at the beginning of the stack
    struct block *curr = head;

    // Iterate until finding an available block
    while (!can_allocate(curr, size)) {
        // Go to the next block
        if (curr->next != NULL)
            curr = curr->next;
        // Append a block
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

/**
 * @brief Joins two consecutive blocks
 *
 * Blocks must be consecutive and free, this is not checked.
 * The memory of the second block is added to the first.
 * next and prev are modified so to maintain the linked list structure.
 *
 * @param[in]  b1 First block.
 * @param[in]  b2 Second block.
 */
void merge_forward (struct block *b1, struct block *b2) {
    b1->size = b1->size + b2->size + sizeof(struct block);
    if (b2->next != NULL) {
        b1->next = b2->next;
        (b1->next)->prev = b1;
    } else {
        b1->next = NULL;
    }
}

/**
 * @brief Merges a block with its neighbours
 *
 * Merges the block with whichever neighbor that isn't NULL and is free
 *
 * @param[in]  b1 block to be compacted.
 */
void compact (struct block *b1) {
    if (b1->next != NULL && b1->next->free)
        merge_forward(b1, b1->next);
    if (b1->prev != NULL && b1->prev->free)
        merge_forward(b1->prev, b1);
}

/**
 * @brief Frees up a block of memory
 *
 * Sets a block to free and checks if any compaction can be done.
 * The address passed is the first usable space of the block to be freed,
 * not the address of the block
 *
 * @param[in]  ptr Pointer to the first address of usable space to be freed.
 */
void my_free (void *ptr) {
    struct block *b = ptr - sizeof(struct block);
    b->free = true;
    compact(b);
}

/**
 * @brief Prints the linked list structure to the console
 *
 * Prints every parameter of every block in a JSON format
 *
 */
void print_state() {
    struct block *curr = head;
    while (curr != NULL) {
        printf("block %p {\n\tprev: %p\n\tnext: %p\n\tsize: %ld\n\tfree: %s\n}\n", curr, curr->prev, curr->next, curr->size, (curr->free) ? "true": "false");
        curr = curr->next;
    }
}