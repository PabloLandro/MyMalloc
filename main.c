#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

int main() {
	printf("Hello, World!\n");
	while(1) {

		char cwd[1024];
		getcwd(cwd, 1024);
		printf("%s> ", cwd);


		char input[1024];
		fgets(input, 1024, stdin);
		char *command = strtok(input, " ");
		if (strncmp(command, "exit\n", 1024) == 0)
			break;
		char *argv[1024];
		printf("%s\n", command);
		argv[0] = (char*) malloc(sizeof(char)*1024);
		strncpy(argv[0], command, 1024);
		for (int i = 1; i < 1024; i++) {
			if ((argv[i] = strtok(NULL, "")) == NULL)
				break;
		}
		execv(command, argv);
	}
	return 0;
}
