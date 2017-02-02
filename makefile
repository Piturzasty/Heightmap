CC = gcc
CFLAGS = -ansi -O
HEADERS_DIR = -I /usr/lib/libglut.so.3 /usr/lib/i386-linux-gnu/libGL.so.1 /usr/lib/i386-linux-gnu/libGLU.so.1
OBJ = main.o app.o config.o graphics.o utility.o
LIBS_DIR = -L /usr/lib
LIBS = -lallegro_image -lallegro -lGL -lGLU

all: $(OBJ)
	$(CC) $(LIBS_DIR) $(HEADERS_DIR) $(LIBS) $(OBJ) -o main
	rm $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) $(HEADERS_DIR) -c -o $@ $<
