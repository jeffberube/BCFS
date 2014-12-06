/*
 *	@Author:		Jeff Berube
 *	@Title:			dir.c
 *	@Description:	dir command for BCFS mock file system
 *
 */

#include "bcfs.h"

void show_help() {

}

int main(int argc, char *argv[]) {

	BCFS *file_system = init_BCFS();

	int i = 0;

	while (i < DIRSIZE) {
		if (file_system->dir[i].start > -1) printf("%s\n", file_system->dir[i].name);
		i++;
	}

	return 0;

}
