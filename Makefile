CPPFLAGS=-std=gnu++11 -O3 -march=native *.cpp

all:
	g++ $(CPPFLAGS) -o closure

asm:
	g++ -S -g0 $(CPPFLAGS)
