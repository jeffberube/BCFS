/*
 *	@Author:	Jeff Berube
 *	@Title:		bcfs.h
 *	@Description:	Header for BCFS related files
 *
 */

#ifndef __BCFS_H__
#define __BCFS_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define DIRSIZE				32
#define FATSIZE				1024
#define	BLOCKSIZE			512
#define BUFFERSIZE			512
#define FILELENGTH			12
#define PARTITION_FILE		"floppy.fs"

#define ERR_NOMEM			1001
#define ERR_INVALID			1002

typedef enum {FALSE = 0, TRUE = 1} BOOLEAN;

typedef enum {
	READ = O_RDONLY, 
	WRITE = O_WRONLY | O_TRUNC
} OFLAGS;

typedef struct {
	char name[FILELENGTH];
	signed short start;
	short size;
} direntry;

typedef direntry DIRECTORY[DIRSIZE];
typedef short FAT[FATSIZE];
typedef unsigned char BLOCK[BLOCKSIZE];

typedef struct {
	int free_space;
	DIRECTORY dir;
	FAT table;
	BLOCK blocks[FATSIZE];
} BCFS;

typedef struct {
	BCFS *file_system;
	int file_descriptor;
	int current_block_index;
	unsigned char *current_block;
	int total;
	int next;
	BOOLEAN dirty;
} STREAM;

#define OPEN_PARTITION(flags) open(PARTITION_FILE, flags)

BCFS *init_BCFS();
void close_BCFS(BCFS *);

STREAM *Sopen(BCFS *, char *);
void Swrite_buffer(STREAM *, int, char *);
BOOLEAN Sclose(STREAM *);

#endif
