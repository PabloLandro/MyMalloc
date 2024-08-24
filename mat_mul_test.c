#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <time.h>

#include "my_malloc.h"



void create_random_matrix(int rows, int cols, int **matrix) {
    // Seed the random number generator
    srand(time(NULL));
    int min = 500000;
    int max = -500000;
    // Fill the matrix with random integers within the specified range
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            //matrix[i][j] = rand()%max;
            matrix[i][j] = rand() % (max - min + 1) + min;

        }
    }
}

void copy_matrix (int rows, int cols, int **A, int **B) {
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            B[i][j] = A[i][j];
}

// Compare that all elements are equal
bool matrix_equal (int m, int p, int **C1, int **C2) {
    for (int i = 0; i < m; i++)
        for (int j = 0; j < p; j++)
            if (C1[i][j] != C2[i][j])
                return false;
    return true;
}

void matrix_multiply (int m, int n, int p, int **A, int **B, int **C) {
    // Initialize the result matrix C to zero
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < p; j++) {
            C[i][j] = 0;
        }
    }

    // Perform matrix multiplication
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < p; j++) {
            for (int k = 0; k < n; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

void **mat_alloc(size_t elem_size, int rows, int cols, void *(*custom_alloc)(size_t)) {
    void **out;
    out = (void**) custom_alloc(sizeof(void*) * rows);
    for (int i = 0; i < rows; i++) {
        out[i] = custom_alloc(elem_size * cols);
    }
    return out;
}

void mat_free(int **ptr, int rows, int cols, void (*custom_free)(void*)) {
    for (int i = 0; i < rows; i++) {
        custom_free(ptr[i]);
    }
    custom_free(ptr);
}

bool mat_mul_test(int n, bool debug) {
    int **A1, **A2, **B1, **B2, **C1, **C2;

    if (debug)
        printf("--------------Matrix Multiplication Test--------------\n");

    A1 = (int**) mat_alloc (sizeof(int), n, n, &malloc);
    B1 = (int**) mat_alloc (sizeof(int), n, n, &malloc);
    C1 = (int**) mat_alloc (sizeof(int), n, n, &malloc);
    A2 = (int**) mat_alloc (sizeof(int), n, n, &my_malloc);
    B2 = (int**) mat_alloc (sizeof(int), n, n, &my_malloc);
    C2 = (int**) mat_alloc (sizeof(int), n, n, &my_malloc);

    if (debug)
        printf("Memory allocated\n");

    create_random_matrix(n, n, A1);
    create_random_matrix(n, n, B1);

    if (debug)
        printf("Random matrices generated\n");

    copy_matrix(n, n, A1, A2);
    copy_matrix(n, n, B1, B2);

    if (debug)
        printf("Matrices copied\n");

    matrix_multiply (n, n, n, A1, B1, C1);
    matrix_multiply (n, n, n, A2, B2, C2);

    bool ans = matrix_equal(n, n, C1, C2);

    mat_free(A1, n, n, &free);
    mat_free(B1, n, n, &free);
    mat_free(C1, n, n, &free);
    mat_free(A2, n, n, &my_free);
    mat_free(B2, n, n, &my_free);
    mat_free(C2, n, n, &my_free);

    if (debug)
        printf("All data freed\n");

    if (ans) {
        printf("Matrix multiplication: SUCCESS\n");
    } else {
        printf("Matrix multiplication: FAILURE\n");
    }

}