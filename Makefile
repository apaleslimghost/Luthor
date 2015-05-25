PROGRAM=luthor

CC=gcc
LDFLAGS=-lpcre
C_FILES := $(shell find . -name '*.c')
OBJS := $(patsubst %.c, %.o, $(C_FILES))

.SUFFIXES: .c .o

all: $(PROGRAM)

debug:
	make -B CFLAGS=-g

foo:
	@echo $(C_FILES) $(OBJS)

$(PROGRAM): $(OBJS)
	$(CC) $(OBJS) -o $(PROGRAM) $(LDFLAGS) $(CFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%: %.c
	$(CC) $(CFLAGS) -o $@ $< 

clean:
	rm -f $(PROGRAM) *.o

.PHONY: clean
