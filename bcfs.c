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

static int get_file_descriptor(BCFS *file_system, char *filename) {

	int i = 0;
	int file_descriptor = -1;

	while (i < DIRSIZE) {
	
		if (file_system->dir[i].start == -1) {
			i++;
			continue;
		}
		
		int j = 0;

		while (j < 12) { 
	
			if (file_system->dir[i].name[j] != *(filename + j)) 
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
 * get_next_free_block_index
 *
 * Gets the index of the next free block in the FAT table
 *
 * INPUT:
 *		BCFS *file_system			Pointer to file system 
 *
 * RETURNS:
 *		Index of first empty block in fat table
 *
 */

static int get_next_free_block_index(BCFS *file_system) {

	int i = 0;

	while (i < FATSIZE) if (file_system->table[i] == -2) return i;	

	return -1;

}


/*
 * alloc_space
 *
 * Allocates the amount of space requested. Makes a chain of blocks and returns the
 * index of the first block in the chain.
 *
 * INPUT:
 *		BCFS *file_system		Pointer to file system to allocate space on
 *		int size				Amount of space to allocate
 *
 * RETURNS:
 *		Index of first block in the newly created chain
 *
 */

static int alloc_space(BCFS *file_system, int size) {

	int first_block = get_next_free_block_index(file_system);
	int total = size - BLOCKSIZE;

	int prev_block = first_block;
	int next_block = -1;

	while (total > 0) {
		next_block = get_next_free_block_index(file_system);
		file_system->table[prev_block] = next_block;
		prev_block = next_block;
		total -= BLOCKSIZE;
	}

	file_system->table[prev_block] = -1;

	return first_block;

}


/*
 * request_space
 *
 * Requests space in filesystem. If space is available, creates the block chain in the
 * fat table and returns the index of the first block.
 *
 * INPUT:
 *		BCFS *file_system		Pointer to file system to request space from
 *		int size				Amount of space requested
 *
 * RETURNS:
 *		Index of first block if space is available, -1 otherwise
 *
 */ 

static int request_space(BCFS *file_system, int size) {

	if (size <= file_system->free_space) 
		return alloc_space(file_system, size);
	else 
		return -1;	

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
 * new_file_BCFS
 *
 * Attempts to create a new file in the file system
 *
 * INPUT:
 *		BCFS *file_system		Pointer to file system to create file into
 *		char *filename			Filename of file to create. Maximum 12 characters.
 *		int size				Size of file to create
 *		char *buffer			Pointer to data to put inside of file
 *
 * RETURNS:
 *		New file index if succesful, -1 if error occured.
 *
 */

int new_file_BCFS(BCFS *fs, char *filename, int size, char *buffer) {

	

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

STREAM *Sopen(char *filename) {

	STREAM *stream;
	BCFS *fs;
	int fd;

	if (!(stream->file_system = init_BCFS())) return NULL;
	
	if ((stream->file_descriptor = get_file_descriptor(stream->file_system, filename)) < 0) {
		
		close_BCFS(stream->file_system);
		return NULL;
	
	} else {
		
		fs = stream->file_system;
		fd = stream->file_descriptor;

		stream->current_block = fs->blocks[fd];
		stream->total = fs->dir[stream->file_descriptor].size;
		stream->next = 0;
		stream->dirty = FALSE;

	}

	return stream;

}


/*
 * Sget_next_block
 *
 * Gets the next block in the stream
 *
 * INPUT:
 *		STREAM *stream			Pointer to a stream
 *
 * RETURNS:
 *		Returns TRUE if successful, FALSE if no more blocks 
 *
 */

BOOLEAN Sget_next_block(STREAM *stream) {

	BCFS *fs = stream->file_system;
	int fd = fs->table[stream->file_descriptor];

	if (fd < 0) 
		return FALSE;
	else {
		stream->file_descriptor = fd;
		stream->current_block = fs->blocks[fd];	
		return TRUE;
	}

}


/*
 * Sclose
 *
 * Closes a stream. Writes it out if dirty flag is set.
 *
 * INPUT:
 *		STREAM *stream			Pointer to a stream to close
 *
 * RETURNS:
 *		Returns TRUE if successful, FALSE if error happened
 *
 */

BOOLEAN Sclose(STREAM *stream) {

	if (stream->dirty) save_BCFS(stream->file_system);
	
	close_BCFS(stream->file_system);

	return TRUE;

}

