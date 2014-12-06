/*
 *	@Author:		Jeff Berube
 *	@Title:			cat.c
 *	@Description:	cat command
 *
 */

#include "bcfs.h"

/*
 * show_help
 *
 * Displays direction on how to use cat command
 *
 */

void show_help() {

	printf("Usage: cat [OPTION] [FILE]\n");

	printf("Displays the contents of a file to stdout.\n");

	printf("Options:\n\t-h, --help\t\tDisplay this information\n");

}

int main(int argc, char *argv[]) {

	/* If no arguments specified or option -h, display help and exit */
	if (argc == 1 || (argc == 2 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")))) {
		show_help();
		return 0;
	}

	BCFS *file_system = init_BCFS();
	STREAM *file = Sopen(file_system, argv[1]);

	if (!file) return 1;

	int i = 0;

	while (i < file->total) {	
		printf("%.*s", BLOCKSIZE, file->current_block);
		get_next_block(file);
		i += BLOCKSIZE;
	}

	return 0;

}
