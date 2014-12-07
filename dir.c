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

	printf("FREE SPACE: \t\t\t%d bytes\n", file_system->free_space);

	int i = 0;

	while (i < DIRSIZE) {
		if (file_system->dir[i].start > -1) 
			printf("%-12s\t\t\t%d bytes\n", file_system->dir[i].name, 
											file_system->dir[i].size);
		i++;
	}

	return 0;

}
