#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <sys/wait.h>

#include "print_utils.h"

#define LEN 1024
#define BIN_DIR "/usr/bin/"

void remove_end_line(char *str) {
	size_t len = strlen(str);
	if (len > 0 && str[len-1] == '\n')
		str[len-1] = '\0';
}

void ls_command() {
	DIR *dirp;
	struct dirent *dp;

	if ((dirp = opendir(".")) == NULL) {
		perror("Error opening directory");
		return;
	}

	while ((dp = readdir(dirp)) != NULL) {
		printf("%s  ", dp->d_name);
	}
	printf("\n");
	
	if (errno == EBADF)
		perror("Error reading directory");
}

void execute(char command[LEN], char *argv[LEN]) {
	pid_t pid = fork();

	if (pid < 0) {
		perror("Error creating fork");
	} else if (pid == 0) {
		execvp(command, argv);
	} else {
		wait(NULL);
	}
}

int handle_command(char command[LEN], char *argv[LEN]) {
	
	if (strncmp(command, "exit", LEN) == 0) {
		exit(EXIT_SUCCESS);
	} else if (strncmp(command, "ls", LEN) == 0) {
		ls_command();
		return -1;
	} else {
		execute(command, argv);
	}
	
	return -1;
}

int main() {
	printf("Hello, World!\n");
	while(1) {

		char cwd[LEN];
		getcwd(cwd, LEN);
		set_foreground(PU_GREEN);
		printf("%s", cwd);
		set_foreground(PU_BLUE);
		printf("> ");
		reset_style();


		char input[LEN];
		fgets(input, LEN, stdin);
		remove_end_line(input);
		
		char *argv[LEN];
		argv[0] = strtok(input, " ");
		int i = 0;
		while ((argv[++i] = strtok(NULL, " ")) != NULL);
		
		if (argv[0] != NULL) {
			int flag = handle_command(argv[0], argv);

			if (flag != -1)
				return flag;
		}

	}
	return 0;
}
