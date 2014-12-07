/*
 *	@Author:		Jeff Berube
 *	@Title:			copy.c
 *	@Description:	Copy command for BCFS file system
 *
 */

#include "bcfs.h"


/*
 * show_help
 *
 * Displays directions on how to use the copy command
 *
 */

void show_help() {

	printf("Usage: copy [OPTION] [SOURCE FILE] [DESTINATION FILE]\n");

	printf("Copies a file from source to destination.\n");
	printf("To copy to or from the BCFS file system, simply prefix the filename with 'bcfs:'\n\n");

	printf("Example: ./copy README bcfs:README\n\n");

	printf("Options:\n\t-h, --help\t\tDisplay this information\n");

}


/*
 * cp_local_to_fs
 *
 * Copies a file from local file system to BCFS file system
 *
 * INPUT:
 *		char *local_file		Name of the local file
 *		char *fs_file			Name of the file to create
 *
 */

void cp_local_to_fs(char *local_file, char *fs_file) {

	int source, source_size;

	/* Make sure source exists */
	if ((source = open(local_file, O_RDONLY)) < 0) {
		printf("ERROR: Could not find file %s\n", local_file);
		exit;
 	}

	/* Get size of source */
	lseek(source, 0L, SEEK_END);
	source_size = lseek(source, 0L, SEEK_CUR) + 1;
	lseek(source, 0L, SEEK_SET);

	char *buffer = malloc(source_size * sizeof(char) + 1);
	read(source, buffer, source_size);

	/* Create a new file */
	if (new_file_BCFS(fs_file + 5, source_size, buffer) < 0) {
		printf("ERROR: Could not create file.\n");
		exit(1);
	}

}


/*
 * cp_fs_to_local
 *
 * Copies a file from the BCFS file system to local file system.
 * Pipes the output of the cat executable part of this package.
 *
 * INPUT:
 *		char *local_file		Name of the local file
 *		char *fs_file			Name of the file to create
 *
 */

void cp_fs_to_local(char *fs_file, char *local_file) {

	char cat[] = "./cat ";
	char redirection_operator[] = " > ";

	int fs_file_len = strlen(fs_file);
	int local_file_len = strlen(local_file);
	int cat_len = strlen(cat);
	int ro_len = strlen(redirection_operator);

	char *command = malloc(cat_len + fs_file_len + ro_len + local_file_len + 1);

	/* Build command for shell to execute */	
	strcpy(command, cat);
	strcat(command, (fs_file + 5));
	strcat(command, redirection_operator);
	strcat(command, local_file);

	system(command);

	free(command);

}


/*
 * cp_fs_to_fs
 *
 * Copies a file from the BCFS file system to local file system.
 * Pipes the output of the cat executable part of this package.
 *
 * INPUT:
 *		char *source_file			Name of the source file
 *		char *destination_file		Name of the file to create
 *
 */

void cp_fs_to_fs(char *source_file, char *destination_file) {

	BCFS *file_system = init_BCFS();
	STREAM *file = Sopen(file_system, (source_file + 5));

	if (!file) {
		printf("ERROR: Could not find file %s", (source_file + 5));
		exit(1);
	}
	
	char *buffer = malloc(((file->total / BLOCKSIZE) + 1) * BLOCKSIZE);
	memset(buffer, 0, file->total + 1);

	int i = 0;

	while (i < file->total) {
		memcpy((buffer + i), file->current_block, BLOCKSIZE);
		get_next_block(file);
		i += BLOCKSIZE;
	}

	int buffer_size = file->total;
	Sclose(file);

	/* Create a new file */
	if (new_file_BCFS(destination_file + 5, buffer_size, buffer) < 0) {
		printf("ERROR: Could not create file.\n");
		exit(1);
	}
	
}


/*
 * cp_local_to_local
 *
 * Copy local file to local file. Uses cp unix command
 *
 */

void cp_local_to_local(char *source_file, char *destination_file) {
	
	char cmd[] = "cp";
	char *cpargs[] = {"cp", source_file, destination_file, NULL};

	execvp(cmd, cpargs);

}

int main(int argc, char *argv[]) {

	/* If no arguments specified or option -h or --help, display help and exit */
	if (argc == 1 || (argc == 2 && 
				(!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")))) {
		show_help();
		return 0;
	
	/* If not enough arguments */
	} else if (argc == 2) {
		printf("Invalid number of arguments.\nType ./copy --help for help\n");
		return 1;
	}

	char fsflag[] = "bcfs:";

	/* Local to local */
	if (!strstr(argv[1], fsflag) && !strstr(argv[2], fsflag)) {
		cp_local_to_local(argv[1], argv[2]);

	/* Local to BCFS */		
	} else if (!strstr(argv[1], fsflag)) {
		cp_local_to_fs(argv[1], argv[2]);

	/* BCFS to local */	
	} else if (!strstr(argv[2], fsflag)) {
		cp_fs_to_local(argv[1], argv[2]);	

	/* BCFS to BCFS */
	} else {
		cp_fs_to_fs(argv[1], argv[2]);	
	}

	return 0;

}
