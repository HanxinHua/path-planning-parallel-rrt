all: input rrt_s rrt_p

input: input.o
	gcc -o input input.o -lm

input.o: input.c
	gcc -c -Wall input.c -lm
	
rrt_s: rrt_s.o
	g++ -o rrt_s rrt_s.o -lm

rrt_s.o: rrt_s.cpp
	g++ -c -Wall rrt_s.cpp -lm
	
rrt_p: rrt_p.o
	mpicxx -o rrt_p rrt_p.o -lm

rrt_p.o: rrt_p.cpp
	mpicxx -c -Wall rrt_p.cpp -lm

clean:
	rm -rf *.o
