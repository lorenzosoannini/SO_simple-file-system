CFLAGS= -Wall -g -std=gnu99 -Wstrict-prototypes
LIBS= 
CC=gcc
AR=ar
BINS= simplefs_test

OBJS = bitmap.o disk_driver.o simplefs.o

HEADERS=bitmap.h\
	disk_driver.h\
	simplefs.h

%.o:	%.c $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@  $<

.phony: clean all


all:	$(BINS) 

objs: $(OBJS)

so_game: simplefs_test.c $(OBJS) 
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -rf *.o *~  $(BINS)
