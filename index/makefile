CC = gcc
INCLUDES = -I/usr/local/mysql/include
LIBS = -L/usr/local/mysql/lib -lmysqlclient -lz
GTK_INC = `pkg-config --cflags gtk+-2.0`
GTK_LIBS = `pkg-config --libs gtk+-2.0`
DEBUG = -g

all: zindex

zindex: generic_index_window.o mysql_index.o
	$(CC) $(DEBUG) -o zindex generic_index_window.o mysql_index.o $(GTK_LIBS) $(LIBS)

generic_index_window.o: generic_index_window.c mysql_index.h
	$(CC) $(DEBUG) -c $(GTK_INC) $(INCLUDES) generic_index_window.c

mysql_index.o: mysql_index.c mysql_index.h
	$(CC) $(DEBUG) -c $(GTK_INC) $(INCLUDES) mysql_index.c

clean:
	rm -f zindex generic_index_window.o mysql_index.o
