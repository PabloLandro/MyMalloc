#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

#include "print_utils.h"
#include "ls_command.h"

#define LEN 1024
#define BIN_DIR "/usr/bin/"

void remove_end_line(char *str) {
	size_t len = strlen(str);
	if (len > 0 && str[len-1] == '\n')
		str[len-1] = '\0';
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
	} else {
		execute(command, argv);
	}
	
	return 1;
}

void print_header() {

	char *buf;

	// User
	buf = getenv("USER");
	set_foreground(PU_GREEN);
	printf("%s@", buf);

	// Current Working Directory
	buf = (char*) malloc(sizeof(char) * LEN);
	getcwd(buf, LEN);
	set_foreground(PU_BLUE);
	printf("%s", buf);
	free(buf);

	// Arrow
	set_foreground(PU_RED);
	printf("> ");
	reset_style();
}

int main() {
	int flag = 1;
	while(flag) {

		print_header();

		char input[LEN];
		fgets(input, LEN, stdin);
		remove_end_line(input);
		
		char *argv[LEN];
		argv[0] = strtok(input, " ");
		int i = 0;
		while ((argv[++i] = strtok(NULL, " ")) != NULL);
		
		if (argv[0] != NULL) {
			flag = handle_command(argv[0], argv);
		}

	}
	exit(EXIT_FAILURE);
}
