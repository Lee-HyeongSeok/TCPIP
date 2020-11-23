CC = gcc
CFLAGS = 
CLIBS = 
CMDS = captureProgram

all : $(CMDS)

captureProgram : capture_program.c
	$(CC) $(CFLAGS) $^ -o $@ $(CLIBS)

clean : 
	rm $(CMDS) core
