CC = gcc
OBJ = format.o dir.o cat.o delete.o copy.o bcfs.o
FLAGS = -Wall -std=c99 -g -o

format: format.o bcfs.o
	$(CC) $^ $(FLAGS) $@

dir: dir.o bcfs.o
	$(CC) $^ $(FLAGS) $@

cat: cat.o bcfs.o
	$(CC) $^ $(FLAGS) $@

delete: delete.o bcfs.o
	$(CC) $^ $(FLAGS) $@

copy: copy.o bcfs.o
	$(CC) $^ $(FLAGS) $@

.c.o:
	$(CC) -g -c $<

clean:
	rm *.o format dir cat delete copy 
