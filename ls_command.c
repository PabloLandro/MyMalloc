#include <stdio.h>
#include <errno.h>
#include <dirent.h>

#include "print_utils.h"

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
	reset_style();
}