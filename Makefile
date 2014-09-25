CPPFLAGS=-std=gnu++11 -O3 -Ipreach2014 -march=native *.cpp */*.cc
CC=/software/gcc/4.9.0/bin/g++

all:
	$(CC) $(CPPFLAGS) -fopenmp -o closure

debug:
	g++ $(CPPFLAGS) -g -fopenmp -o closure

asm:
	g++ -S -g0 $(CPPFLAGS)

clean:
	-rm out/*
	-rm graph.png
	-rm *.s
	-rm *.o



	
