CPPFLAGS=-std=gnu++11 -O3 -march=native *.cpp -fopenmp -Wl,-rpath=/usr/lib/gcc/x86_64-linux-gnu/4.8

all:
	/software/gcc/4.8.1/bin/g++ $(CPPFLAGS) -o closure

debug:
	g++ $(CPPFLAGS) -g -fopenmp -o closure

asm:
	g++ -S -g0 $(CPPFLAGS)

clean:
	-rm out/*
	-rm graph.png
	-rm *.s
	-rm *.o



	
