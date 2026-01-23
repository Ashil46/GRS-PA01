
CC      = gcc
CFLAGS  = -Wall -Wextra -O2

LDFLAGS = -pthread -lm

# Output binaries 
PROG_A = program_a
PROG_B = program_b

# source files
WORKERS_SRC = MT25065_Part_B_workers.c
WORKERS_HDR = MT25065_Part_B_workers.h
PROG_A_SRC   = MT25065_Part_A_Program_A.c
PROG_B_SRC   = MT25065_Part_A_Program_B.c

.PHONY: all clean

all: $(PROG_A) $(PROG_B)

# Link both programs with LDFLAGS at the end for proper library resolution
$(PROG_A): $(PROG_A_SRC) $(WORKERS_SRC) $(WORKERS_HDR)
	$(CC) $(CFLAGS) $(PROG_A_SRC) $(WORKERS_SRC) -o $(PROG_A) $(LDFLAGS)

$(PROG_B): $(PROG_B_SRC) $(WORKERS_SRC) $(WORKERS_HDR)
	$(CC) $(CFLAGS) $(PROG_B_SRC) $(WORKERS_SRC) -o $(PROG_B) $(LDFLAGS)

clean:
	rm -f $(PROG_A) $(PROG_B) *.o io_worker_temp.txt