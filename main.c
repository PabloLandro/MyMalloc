#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>

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
		printf("%s\n", dp->d_name);
	}
	
	if (errno == EBADF)
		perror("Error reading directory");
}

int handle_command(char command[LEN], char *argv[LEN]) {
	if (strncmp(command, "exit", LEN) == 0) {
		exit(EXIT_SUCCESS);
	} else if (strncmp(command, "ls", LEN) == 0) {
		ls_command();
		return -1;
	}

	char aux[LEN+strlen(BIN_DIR)];
	strcat(strcat(aux, BIN_DIR), command);
	switch(access(aux, X_OK)) {
		case 0:
			pid_t pid = fork();
			if (pid < 0) {
				perror("Error creating fork");
				return 1;
			}
			if (pid == 0) {
				execv(aux, argv);
			}
			break;
		case -1:
			if (errno == ENOENT || errno == ENOTDIR) {
				perror("Error: Command not recognized");
			} else {
				perror("Error");
			}
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
