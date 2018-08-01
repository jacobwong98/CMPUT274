CC= g++
OBJS= exercise7.o gcd.o
CFLAGS= -c -Wall -O2
LFLAGS= -static
PROGRAM= exercise7

$(PROGRAM): $(OBJS)
				$(CC) $(OBJS) -o $(PROGRAM) $(LFLAGS)

exercise7.o: exercise7.cpp gcd.h
				$(CC) exercise7.cpp -o exercise7.o $(CFLAGS)

gcd.o: gcd.cpp gcd.h
				$(CC) gcd.cpp -o gcd.o $(CFLAGS)

clean:
				@rm -f $(OBJS)
				@rm -f $(PROGRAM)
