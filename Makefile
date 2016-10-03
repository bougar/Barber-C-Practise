
CC=gcc
CFLAGS=-Wall -pthread -g
GTK=`pkg-config --libs --cflags gtk+-3.0`
LIBS=cola.o barber.o `pkg-config --libs --cflags gtk+-3.0`
PROGS= barber myGtk

all: $(PROGS)

%.o: %.c
	$(CC) $(CFLAGS) $(GTK) -c $<
	
barber: cola.o barber.o
	$(CC) $(CFLAGS) -o $@ $(LIBS) 
myGtk:myGtk.o
	$(CC) $(CFLAGS) $(GTK) -o $@ myGtk.o
clean:
	rm -f $(PROGS) *.o *~

