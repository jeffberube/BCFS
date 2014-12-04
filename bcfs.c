/*
 *	@Author:	Jeff Berube
 *	@Title:		format.c	
 *	@Description:	BCFS API	
 *
 */

#include "bcfs.h"

/*
 * get_file_descriptor
 *
 * Returns the index of the starting block in the fat table
 *
 * INPUT:
 *		STREAM *stream			Pointer to a stream with an initiated file system
 *		char *filename			Name of the file to find in the file system
 *
 * RETURNS:
 *		A negative number if file couldn't be found, otherwise index of file in directory
 *
 */

static int get_file_descriptor(STREAM *stream, char *filename) {

	int i = 0;
	int file_descriptor = -1;

	while (i < DIRSIZE) {
	
		if (stream->file_system->dir[i].start == -1) {
			i++;
			continue;
		}
		
		int j = 0;

		while (j < 12) { 
	
			if (stream->file_system->dir[i].name[j] != *(filename + j)) 
				goto next_iteration;
			else 
				j++;
	
		}

		file_descriptor = i;
		break;

		next_iteration:
			i++;

	}

	if (file_descriptor == -1)
		printf("\nERROR: File not found.\n\n");

	return file_descriptor;

}


/*
 * init_BCFS
 *
 * Initiates connection to file system.
  *
 * RETURNS:
 *		NULL if error happened, otherwise pointer to BCFS struct populated 
 *		with full filesystem.
 *
 */

BCFS *init_BCFS() {

	int partition_file = OPEN_PARTITION(READ); 

	if (partition_file < 0) {
		printf("\nERROR: Failed to open partition file.\n\n");
		return NULL;
	}
	
	BCFS *file_system = malloc(sizeof(BCFS));
	
	if (read(partition_file, file_system, sizeof(BCFS)) < 0) {
		printf("\nERROR: Couldn't read partition file.\n\n");
		return NULL;
	} else
		close(partition_file);	

	return file_system;	

}


/*
 * close_BCFS
 *
 * Cleans up file system connection
 *
 * INPUT:
 *		BOOLEAN save		Saves the file system before closing if TRUE
 *
 */

inline void close_BCFS(BCFS *fs) {

	free(fs);
	
}


/*
 * save_BCFS
 *
 * Saves the file system to the partition file
 *
 * INPUT:
 *		BCFS *fs			Pointer to file system to write to partition file
 *
 * RETURNS:
 *		TRUE if save was successful, FALSE otherwise
 *
 */

BOOLEAN save_BCFS(BCFS *fs) {

	int partition_file = OPEN_PARTITION(WRITE);
		
	if (write(partition_file, fs, sizeof(BCFS))) {
		printf("\nERROR: Could not save modifications.\n\n");
		return FALSE;
	}

	return TRUE;

}

/*
 * Sopen
 *
 * Returns a pointer to a stream of the specified file.
 *
 * INPUT:
 *		char *filename			Name of file to create a stream for
 *		char *permission		Permission flags
 *
 * RETURNS:
 *		A pointer to a stream for the requested file or null if file was not found.
 *
 */

STREAM *Sopen(char *filename, char *permissions) {

	STREAM *stream;
	BCFS *fs;
	int fd;

	if (!(stream->file_system = init_BCFS())) return NULL;
	
	if ((stream->file_descriptor = get_file_descriptor(stream, filename)) < 0) {
		
		close_BCFS(stream->file_system);
		return NULL;
	
	} else {
		
		fs = stream->file_system;
		fd = stream->file_descriptor;

		stream->permissions = *permissions;
		stream->total = fs->dir[stream->file_descriptor].size;
		stream->current_block = fs->blocks[fd];
		stream->next = 0;

	}

	return stream;

}

/*
 * Sclose
 *
 * Closes a stream
 *
 * INPUT:
 *		STREAM *stream			Pointer to a stream to close
 *
 * RETURNS:
 *		Returns TRUE if successful, FALSE if error happened
 *
 */

BOOLEAN Sclose(STREAM *stream) {


	return TRUE;

}

/*
 * Sgetc
 *
 * Gets a single character from the specified stream.
 *
 * INPUT:
 *		STREAM *stream			Pointer to stream to get next character from
 *
 * RETURNS:
 *		Single character as an int or -1 if end of stream was reached
 *
 */

int Sgetc(STREAM *stream) {

}

/*
 * Sputc
 *
 * Write a single character to the specified stream.
 *
 * INPUT:
 *		STREAM *stream			Pointer to stream to write to
 *		char c					Character to write to stream
 *
 * RETURNS:
 *		0 if successful, -1 if error occured
 *
 */

BOOLEAN Sputc(STREAM *stream, char c) {

}


