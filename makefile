CC = gcc -std=c99
INCLUDES = `mysql_config --cflags`
LIBS = `mysql_config --libs`
GTK_INC = `pkg-config --cflags gtk+-2.0`
GTK_LIBS = `pkg-config --libs gtk+-2.0`
DEBUG = -g -Wall -DGLIB_VERSION_MIN_REQUIRED=GLIB_VERSION_2_44 -DGLIB_VERSION_MAX_ALLOWED=GLIB_VERSION_2_60 -DGTK_DISABLE_DEPRECATED

all: etymos

main.o: main.c etymo_window.h
	$(CC) $(DEBUG) -c $(INCLUDES) main.c $(GTK_INC)

etymos: main.o root_window.o error.o mysql_change.o mysql_selects.o word_window.o etymo_window.o index/generic_index_window.o index/mysql_index.o word_selects.o word_options.o word_signals.o etymo_signals.o
	$(CC) $(DEBUG) -o etymos main.o root_window.o error.o mysql_change.o mysql_selects.o word_window.o etymo_window.o index/mysql_index.o index/generic_index_window.o word_selects.o word_options.o word_signals.o etymo_signals.o $(LIBS) $(GTK_LIBS)

root_window.o: root_window.c root_window.h
	$(CC) $(DEBUG) -c $(INCLUDES) root_window.c $(GTK_INC)

word_window.o: word_window.c word_window.h
	$(CC) $(DEBUG) -c $(INCLUDES) word_window.c $(GTK_INC)

word_signals.o: word_signals.c word_window.h
	$(CC) $(DEBUG) -c $(INCLUDES) word_signals.c $(GTK_INC)

mysql_change.o: mysql_change.c mysql_change.h
	$(CC) $(DEBUG) -c $(INCLUDES) mysql_change.c

error.o: error.c error.h
	$(CC) $(DEBUG) -c error.c $(GTK_INC)

mysql_selects.o: mysql_selects.c mysql_selects.h
	$(CC) $(DEBUG) -c $(INCLUDES) mysql_selects.c $(GTK_INC)

word_options.o: word_options.c
	$(CC) $(DEBUG) -c $(INCLUDES) word_options.c $(GTK_INC)

word_selects.o: word_selects.c word_selects.h
	$(CC) $(DEBUG) -c $(INCLUDES) word_selects.c $(GTK_INC)

etymo_window.o: etymo_window.c etymo_window.h
	$(CC) $(DEBUG) -c $(INCLUDES) etymo_window.c $(GTK_INC)

etymo_signals.o: etymo_signals.c etymo_window.h
	$(CC) $(DEBUG) -c $(INCLUDES) etymo_signals.c $(GTK_INC)

index/generic_index_window.o: index/generic_index_window.c index/generic_index_window.h index/mysql_index.c index/mysql_index.h
	$(CC) $(DEBUG) -c -o index/generic_index_window.o $(INCLUDES) index/generic_index_window.c $(GTK_INC)

index/mysql_index.o: index/mysql_index.c index/mysql_index.h
	$(CC) $(DEBUG) -c -o index/mysql_index.o $(INCLUDES) index/mysql_index.c $(GTK_INC)

clean:
	rm -f *.o index/*.o etymos
