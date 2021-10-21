CC = gcc
INCLUDES = -I/usr/local/mysql/include
LIBS = -L/usr/local/mysql/lib -lmysqlclient -lz
DEBUG = -g

all: importDict

importDict: importDict.o libImportDict.o
	$(CC) $(DEBUG) -o importDict importDict.o libImportDict.o $(LIBS)

libImportDict.o: libImportDict.c libImportDict.h
	$(CC) $(DEBUG) -c $(INCLUDES) libImportDict.c

importDict.o: importDict.c libImportDict.h
	$(CC) $(DEBUG) -c $(INCLUDES) importDict.c

clean:
	rm -f importDict importDict.o libImportDict.o
