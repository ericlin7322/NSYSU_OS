CC = gcc
CFLAGS = 
OUT = B073021021

all: mm
	$(CC) $(CFLAGS) *.o  -o $(OUT)
	
mm: mm.c
	$(CC) $(CFLAGS) -c mm.c
	$(CC) $(CFLAGS) -c main.c

clean:
	rm -f $(OUT) mm.o main.o
