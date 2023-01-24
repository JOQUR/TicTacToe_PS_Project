CC=gcc
CFLAGS=-I. -lm

OBJECTS = server_working client_working

all: $(OBJECTS)

$(OBJECTS):%:%.c
	@echo Compiling $<  to  $@
	$(CC) -pthread -o $@ $< $(CFLAGS)

	
clean:
	rm  $(OBJECTS) 
