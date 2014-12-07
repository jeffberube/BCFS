/*
 *	@Author:		Jeff Berube
 *	@Title:			delete.c
 *	@Description:	delete command for BCFS mock file system
 *
 */

#include "bcfs.h"

/*
 * show_help()
 *
 * Displays directions on how to use the delete command
 *
 */

void show_help() {

	printf("Usage: delete [OPTION] [FILE]\n");

	printf("Deletes a file from the BCFS file system.\n\n");

	printf("Options:\n\t-h, --help\t\tDisplay this information\n");

}

int main(int argc, char *argv[]) {

	/* If no arguments specified or option -h or --help, display help and exit */
	if (argc == 1 || (argc == 2 &&
				(!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")))) {
		show_help();
		return 0;
	}

	BCFS *file_system = init_BCFS();
	STREAM *file = Sopen(file_system, argv[1]);

	if (!file) {
		printf("ERROR: Could not find file %s", argv[1]);
		return 1;
	}

	memset(file_system->dir[file->file_descriptor].name, 0, 12);
	file_system->dir[file->file_descriptor].start = -1;

	int prev_block = file->current_block_index;
	int next_block = file_system->table[prev_block];

	while (prev_block != -1) {
		file_system->table[prev_block] = -2;
		prev_block = next_block;
		next_block = file_system->table[prev_block];
	}

	file_system->free_space += file->total;

	save_BCFS(file_system);
	Sclose(file);

	return 0;

}
