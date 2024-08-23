#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    printf("Hello from test\n");
    char aux[100];
    scanf("%s", aux);
    for (int i = 0; i < argc; i++)
        printf("%s\n", argv[i]);

    exit(EXIT_SUCCESS);
}