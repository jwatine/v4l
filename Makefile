


CC=gcc
PROGRAM_NAME=open_device
CFLAGS =-Wall 
LDFLAGS= -lSDL_image -lSDL

all:
	$(CC) -o $(PROGRAM_NAME) $(PROGRAM_NAME).c $(CFLAGS) $(LDFLAGS)


clean:
	rm *~ 
