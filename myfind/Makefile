CFLAGS = -std=c++17 -Wall

all: main

main: main.o search.o
	g++ $(CFLAGS) -o myfind main.o search.o
main.o: main.cpp
	g++ $(CFLAGS) -c main.cpp
search.o: search.cpp search.h
	g++ $(CFLAGS) -c search.cpp
clean:
	rm myfind *.o
