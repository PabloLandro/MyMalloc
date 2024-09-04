#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "my_malloc.h" // Include the header file with your function declarations

void test_my_realloc() {
    // Test 1: Reallocate to a larger size
    printf("Starting test 1\n");
    int *ptr = (int *)my_malloc(5 * sizeof(int));
    for (int i = 0; i < 5; i++) {
        ptr[i] = i;
    }
    ptr = (int *)my_realloc(ptr, 10 * sizeof(int));
    assert(ptr != NULL);
    for (int i = 0; i < 5; i++) {
        assert(ptr[i] == i);
    }

    // Test 2: Reallocate to a smaller size
    printf("Starting test 2\n");
    ptr = (int *)my_realloc(ptr, 3 * sizeof(int));
    assert(ptr != NULL);
    for (int i = 0; i < 3; i++) {
        assert(ptr[i] == i);
    }
    
    // Test 3: Reallocate to the same size
    printf("Starting test 3\n");
    int *ptr2 = (int *)my_realloc(ptr, 3 * sizeof(int));
    assert(ptr2 == ptr);
    for (int i = 0; i < 3; i++) {
        assert(ptr[i] == i);
    }
    // Test 4: Reallocate NULL pointer (should behave like my_malloc)
    printf("Starting test 4\n");
    char *str = (char *)my_realloc(NULL, 20);
    assert(str != NULL);
    strcpy(str, "Hello, World!");
    assert(strcmp(str, "Hello, World!") == 0);

    // Test 5: Reallocate to size 0 (should free the memory)
    printf("Starting test 5\n");
    str = (char *)my_realloc(str, 0);
    assert(str == NULL);

    // Test 6: Stress test with larger allocations
    printf("Starting test 6\n");
    void *large_ptr = my_malloc(5000);
    assert(large_ptr != NULL);
    large_ptr = my_realloc(large_ptr, 10000);
    assert(large_ptr != NULL);

    printf("All tests passed successfully!\n");
}

int main() {
    my_malloc_init();
    test_my_realloc();
    return 0;
}