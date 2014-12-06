/*
 *	@Author:	Jeff Berube
 *	@Title:		format.c	
 *	@Description:	Format utility for BCFS
 *
 */

#include "bcfs.h"

int main(int argc, char *argv[]) {

	/* Confirm user wants to format the BCFS partition */
	char answer;
	
	printf("\nAre you sure you want to format the BCFS partition? All data will be lost. (y/n) ");
	
	while (answer != 'y' && answer != 'n') scanf(" %c", &answer);

	if (answer == 'n') {	
		printf("\n");
	   	return 0;
	}

	printf("\nFormatting BCFS partition...\n");

	/* Open or create partition file and clear data if file already exists */
	int open_flags = O_WRONLY | O_TRUNC | O_CREAT;
	mode_t permission_flags = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
	int partition_file = open(PARTITION_FILE, open_flags, permission_flags);

	if (partition_file < 0) {
		printf("\nERROR: Failed to open or create partition file.\n\n");
		return -1;
	}

	/* Write file system structure to partition file */
	BCFS *file_system = malloc(sizeof(BCFS));
	memset(file_system, 0, sizeof(BCFS));

	file_system->free_space = FATSIZE * BLOCKSIZE;
	
	int i = 0;
	while (i < DIRSIZE) {
		file_system->dir[i].start = -1;
		i++;
	}

	i = 0;
	while (i < FATSIZE) {
		file_system->table[i] = -2;
		i++;
	}

	int bytes = write(partition_file, file_system, sizeof(BCFS));
	
	if (bytes < 0) {
		printf("\nERROR: Failed to write file system.\n\n");
		free(file_system);
		return -1;
	}

	printf("\nBCFS partition successfuly formatted.\n\n");

	free(file_system);

	return 0;

}
