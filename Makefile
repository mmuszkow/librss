CC=gcc
CFLAGS=-Wall -shared
DEFINES=-DRSS_ASDLL
INCLUDES=-Iinclude -I/usr/include/libxml2
LIBS=-lxml2
OUTPUT=-oRSS.so

all: RSS.so

RSS.so: src/RSS.c
	$(CC) $(CFLAGS) $(INCLUDES) $(LIBSPATH) $(LIBS) $(DEFINES) $(OUTPUT) ./src/RSS.c

clean:
	rm -rf ./bin/*.o
