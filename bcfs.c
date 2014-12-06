/*
 *	@Author:	Jeff Berube
 *	@Title:		format.c	
 *	@Description:	BCFS API	
 *
 */

#include "bcfs.h"

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

BOOLEAN get_next_block(STREAM *stream) {

	BCFS *fs = stream->file_system;
	int index = stream->current_block_index;

	if (fs->table[index] < 0) 
		return FALSE;
	else {
		stream->current_block_index = fs->table[index];
		stream->current_block = fs->blocks[stream->current_block_index];	
		return TRUE;
	}

}


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

		if (!strcmp(file_system->dir[i].name, filename)) {
			file_descriptor = i;
			break;
		}
		
		i++;

	}

	return file_descriptor;

}

/*
 * get_next_free_dir_index
 *
 * Gets the index of the next free index in the directory list
 *
 * INPUT:
 *		BCFS *file_system			Pointer to file system 
 *
 * RETURNS:
 *		Index of first empty directory in directory list 
 *
 */

static int get_next_free_dir_index(BCFS *file_system) {

	int i = 0;

	while (i < DIRSIZE) {
		if (file_system->dir[i].start == -1) 
			return i;
		else
			i++;
	}	

	return -1;

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

	while (i < FATSIZE) {
		printf("FAT %d: %d\n", i, file_system->table[i]);
		if (file_system->table[i] == -2) 
			return i;
		else 
			i++;
	}	

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

	int total = size;
	
	int first_block = get_next_free_block_index(file_system);
	file_system->table[first_block] = -1;

	int prev_block = first_block;
	int next_block;

	while (total > 0) {
		next_block = get_next_free_block_index(file_system);
		file_system->table[prev_block] = next_block;
		file_system->table[next_block] = -1;
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
		printf("ERROR: Failed to open partition file.\n");
		return NULL;
	}
	
	BCFS *file_system = malloc(sizeof(BCFS));
	
	if (read(partition_file, file_system, sizeof(BCFS)) < 0) {
		printf("ERROR: Couldn't read partition file.\n");
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

BOOLEAN save_BCFS(BCFS *file_system) {

	int partition_file = OPEN_PARTITION(WRITE);
		
	if (!write(partition_file, file_system, sizeof(BCFS))) {
		printf("ERROR: Could not save modifications.\n");
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
 *		char *filename			Filename of file to create. Maximum 12 characters.
 *		int size				Size of file to create
 *		char *buffer			Pointer to data to put inside of file
 *
 * RETURNS:
 *		New file index if succesful, -1 if error occured.
 *
 */

int new_file_BCFS(char *filename, int size, char *buffer) {

	BCFS *file_system = init_BCFS();

	STREAM *file = Sopen(file_system, filename);
	if (file) {
		printf("ERROR: File already exists. Choose a different name.\n");
		Sclose(file);
		return -1;
	}	

	if (strlen(filename) > 12) {
		printf("ERROR: File name is too long. Maximum 12 characters.\n");
		return -1;
	}	

	file_system = init_BCFS();
	int index = get_next_free_dir_index(file_system);

	if (index < 0) {
		printf("ERROR: Maximum number of entries reached.\n");
		return -1;
	}	

	int start = request_space(file_system, size);
	
	if (start < 0) {
		printf("ERROR: Not enough space.");
		return -1;
	}	

	strcpy(file_system->dir[index].name, filename);
	file_system->dir[index].start = start;
	file_system->dir[index].size = size;
	
	file = Sopen(file_system, filename);
	Swrite_buffer(file, size, buffer);
	save_BCFS(file_system);
	Sclose(file);

	return index;

}


/*
 * Sopen
 *
 * Returns a pointer to a stream of the specified file.
 *
 * INPUT:
 *		char *filename			Name of file to create a stream for
 *
 * RETURNS:
 *		A pointer to a stream for the requested file or null if file was not found.
 *
 */

STREAM *Sopen(BCFS *file_system, char *filename) {

	STREAM *stream = malloc(sizeof(STREAM));
	BCFS *fs;
	int fd;

	if (!(stream->file_system = file_system)) 
		return NULL;
	else if ((stream->file_descriptor = get_file_descriptor(file_system, filename)) < 0) {
		return NULL;
	} else {
		fs = stream->file_system;
		fd = stream->file_descriptor;

		stream->current_block_index = fs->dir[fd].start;
		stream->current_block = fs->blocks[fs->dir[fd].start];
		stream->total = fs->dir[fd].size;
		stream->next = 0;
		stream->dirty = FALSE;
	}

	return stream;

}

/*
 * Swrite_buffer
 *
 * Writes buffer to file
 *
 * INPUT:
 *		STREAM *stream			Pointer to stream to write to
 *		int size				Size of buffer to write
 *		char *buffer			Buffer to copy into stream
 *
 */

void Swrite_buffer(STREAM *stream, int size, char *buffer) {

	int i = 0;
	int bytes_in_block = size;

	while (i < size && buffer[i]) {
		bytes_in_block = size - i >= BLOCKSIZE ? BLOCKSIZE : size - i;
		printf("i:%d size:%d s:%.512s", i, bytes_in_block, (buffer + i));
		memcpy(stream->current_block, (buffer + i), bytes_in_block);
		i += BLOCKSIZE;
		get_next_block(stream);
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

