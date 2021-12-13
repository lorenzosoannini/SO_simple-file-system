CFLAGS= -Wall -g -std=gnu99 -Wstrict-prototypes
LIBS= 
CC=gcc
AR=ar
BINS= simplefs_test

OBJS = #add here your object files

HEADERS=bitmap.h\
	disk_driver.h\
	simplefs.h

%.o:	%.c $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@  $<

.phony: clean all


all:	$(BINS) 

so_game: simplefs_test.c $(OBJS) 
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -rf *.o *~  $(BINS)
