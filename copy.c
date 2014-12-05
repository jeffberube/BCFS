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

	printf("Options:\n\t-h, --help\t\tDisplay this information\n");

}

void cp_local_to_fs(char *local_file, char *fs_file) {

	int source, source_size;

	if ((source = open(local_file, O_RDONLY)) < 0) {
		printf("ERROR: Could not find file %s\n", local_file);
		exit;
 	}

	lseek(source, 0L, SEEK_END);
	source_size = lseek(source, 0L, SEEK_CUR);
	lseek(source, 0L, SEEK_SET);

}

void cp_fs_to_local(char *fs_file, char *local_file) {

	STREAM *source = Sopen(fs_file+5); 

	int dest_flags = O_WRONLY | O_TRUNC | O_CREAT;
	mode_t dest_permissions = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
	
	int dest = open(local_file, dest_flags, dest_permissions);

	if (dest < 0) {
		printf("ERROR: Could not create/modify file %s\n", local_file);
		exit(1);
	}

}

/*
 * cp_local_to_local
 *
 * Copy local file to local file. Uses cp unix command
 *
 */

void cp_local_to_local(char *file1, char *file2) {
	
	char cmd[] = "cp";
	char *cpargs[] = {"cp", file1, file2, NULL};

	execvp(cmd, cpargs);

}

int main(int argc, char *argv[]) {

	/* If no arguments specified or option -h, display help and exit */
	if (argc == 1 || (argc == 2 && 
				(!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")))) {
	   	
		show_help();
		return 0;
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

	/* BCFS to local */	
	} else if (!strstr(argv[2], fsflag)) {
	

	/* BCFS to BCFS */
	} else {
	
	}

	return 0;

}
