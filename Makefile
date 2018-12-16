all: input input_complex_map input_more_complex_map rrt_s rrt_s_no_collision rrt_p_region rrt_p_robot rrt_p_robot_no_collision

input: input.o
	gcc -o input input.o -lm

input.o: input.c
	gcc -c -Wall input.c -lm
	
input_complex_map: input_complex_map.o
	gcc -o input_complex_map input_complex_map.o -lm

input_complex_map.o: input_complex_map.c
	gcc -c -Wall input_complex_map.c -lm
	
input_more_complex_map: input_more_complex_map.o
	gcc -o input_more_complex_map input_more_complex_map.o -lm

input_more_complex_map.o: input_more_complex_map.c
	gcc -c -Wall input_more_complex_map.c -lm
	
rrt_s: rrt_s.o
	g++ -o rrt_s rrt_s.o -lm

rrt_s.o: rrt_s.cpp
	g++ -c -Wall rrt_s.cpp -lm
	
rrt_s_no_collision: rrt_s_no_collision.o
	g++ -o rrt_s_no_collision rrt_s_no_collision.o -lm

rrt_s_no_collision.o: rrt_s_no_collision.cpp
	g++ -c -Wall rrt_s_no_collision.cpp -lm
	
rrt_p_region: rrt_p_region.o
	mpicxx -o rrt_p_region rrt_p_region.o -lm

rrt_p_region.o: rrt_p_region.cpp
	mpicxx -c -Wall rrt_p_region.cpp -lm
	
rrt_p_robot: rrt_p_robot.o
	mpicxx -o rrt_p_robot rrt_p_robot.o -lm

rrt_p_robot.o: rrt_p_robot.cpp
	mpicxx -c -Wall rrt_p_robot.cpp -lm

rrt_p_robot_no_collision: rrt_p_robot_no_collision.o
	mpicxx -o rrt_p_robot_no_collision rrt_p_robot_no_collision.o -lm

rrt_p_robot_no_collision.o: rrt_p_robot_no_collision.cpp
	mpicxx -c -Wall rrt_p_robot_no_collision.cpp -lm


clean:
	rm -rf *.o
