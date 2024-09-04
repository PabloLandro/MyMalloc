#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

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
    head->size = STACK_SIZE-sizeof(struct block);

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
bool can_allocate(struct block b, size_t size) {
    return b.free && size <= b.size;
}

struct block *find_available (size_t size) {
    struct block *curr = head;
    while (!can_allocate(*curr, size)) {
        if (curr->next == NULL)
            return NULL;
        curr = curr->next;
    }
    return curr;
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

    // Find available block
    struct block *curr = find_available(size);

    if (curr == NULL) {
        perror("Couldn't find a free block");
        exit(EXIT_FAILURE);
    }

    curr->free = false;

    // If a block fits between the current and the next, we create it
    if (curr->next != NULL && (((void*)curr + size + 2*sizeof(struct block)) < (void*)curr->next)) {
        struct block *aux = (struct block*)((void*)curr + size + sizeof(struct block));
        aux->next = curr->next;
        aux->prev = curr;
        curr->next->prev = aux;
        curr->next = aux;
        aux->free = true;
        aux->size = ((void*)(aux->next) - (void*)aux)-sizeof(struct block);

    }
    // If curr is the last block and another fits, we create it
    else if (curr->next == NULL && (void*)curr + size + sizeof(struct block) < tail) {
        struct block *aux = (struct block*)((void*)curr + size + sizeof(struct block));
        curr->next = aux;
        aux->next = NULL;
        aux->prev = curr;
        aux->free = true;
        aux->size = (tail - (void*)aux)-sizeof(struct block);
    }
    curr->size = size;
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

    // Update the size
    b1->size = b1->size + b2->size + sizeof(struct block);

    // Change neighbors
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
    // If possible merge with next and prev
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
    if (b->next == NULL)
        b->size = (void*) tail - (void*) b - 32;
    else
        b->size = (void*) (b->next) - (void*) b - 32;
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

/**
 * @brief Copies the data in ptr to a new block of given size
 * 
 * If size is the same, same pointer is returned
 * If ptr is NULL, malloc is called
 * If size is 0, NULL pointer is returned
 *
 * @param[in] ptr Pointer to the first address of usable space of the block to be rellocated
 * @param[in] size Size of the new block
 * @return Pointer to the first address of usable space of the new block
 *
 */
void *my_realloc (void *ptr, size_t size) {
    if (ptr == NULL)
        return my_malloc(size);
    else if (size == 0)
        return NULL;

    // Get pointer to the block
    struct block *from = (struct block*) (ptr - sizeof(struct block));

    if (from->size == size)
        return ptr;

    // Create pointer to the usable spaces to perform copy
    char *f = (char*)ptr;
    char *d = (char*)my_malloc(size);

    // Copy data
    for (int i = 0; i < ((from->size < size) ? from->size: size); i++) {
        d[i] = f[i];
    }

    // Free old block
    my_free(ptr);
    return (void*)d;

}