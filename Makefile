CPPFLAGS=-std=gnu++11 -O3 -march=native *.cpp

all:
	g++ $(CPPFLAGS) -fopenmp -o closure

debug:
	g++ $(CPPFLAGS) -g -fopenmp -o closure

asm:
	g++ -S -g0 $(CPPFLAGS)

clean:
	-rm out/*
	-rm graph.png
	-rm *.s
	-rm *.o



	
