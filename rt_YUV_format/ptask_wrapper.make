# Authors: Titouan ROOS, Julien FORGET

# Note: for correct execution privileges, run 'echo -1 > /proc/sys/kernel/sched_rt_runtime_us'

# modify these 2 folders depending on your installation
PTASK_SOURCE_FOLDER=/home/jwatine/ptask
PRELUDE_FOLDER=/home/jwatine/Prelude/trunk

PRELUDE_BIN_FOLDER=$(PRELUDE_FOLDER)/bin
PRELUDE_SHARE_FOLDER=$(PRELUDE_FOLDER)/share
PRELUDE_LIB_FOLDER=$(PRELUDE_FOLDER)/lib

# modify these 3 lines for each new Prelude program
PRELUDE_PROGRAM_FOLDER=.
PRELUDE_PROGRAM_NAME=camera
PRELUDE_NODE_NAME=camera

PTASK_WRAPPER=$(PRELUDE_PROGRAM_FOLDER)/ptask_wrapper

PRELUDE_PROGRAM=$(PRELUDE_PROGRAM_FOLDER)/$(PRELUDE_PROGRAM_NAME).plu
PRELUDE_C_PROGRAM=$(PRELUDE_PROGRAM_FOLDER)/$(PRELUDE_PROGRAM_NAME)_c/$(PRELUDE_NODE_NAME)

USER_C_FOLDER=$(PRELUDE_PROGRAM_FOLDER)
# next line is used only if nodes are not directly defined in header file
USER_C_NODES=$(USER_C_FOLDER)/$(PRELUDE_PROGRAM_NAME)_nodes

OBJS=$(PTASK_WRAPPER).o $(PRELUDE_C_PROGRAM).o $(USER_C_NODES).o open_device.o

PRELUDEC=$(PRELUDE_BIN_FOLDER)/preludec
PLUFLAGS=-with_encoding -print_deadlines

CC=gcc
CFLAGS=-I$(PTASK_SOURCE_FOLDER)/src -I$(USER_C_FOLDER) -I$(PRELUDE_PROGRAM_FOLDER)/$(PRELUDE_PROGRAM_NAME)_c -I$(PRELUDE_LIB_FOLDER)/prelude -Wall
LDFLAGS=-lptask -L$(PTASK_SOURCE_FOLDER)/build/src -pthread -lrt -lSDL
EXEC=$(PRELUDE_PROGRAM_NAME)

all: executable

executable: $(OBJS)
	$(CC) -o $(EXEC) $^ $(LDFLAGS)

$(PRELUDE_C_PROGRAM).c: $(PRELUDE_PROGRAM)
	$(PRELUDEC) $(PLUFLAGS) -node $(PRELUDE_NODE_NAME) $(PRELUDE_PROGRAM)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -rf *.o *~ $(EXEC) $(PRELUDE_C_PROGRAM).*  $(PTASK_WRAPPER).o \#*\#
