# path-planning-parallel-rrt


There are total 10 c/c++ files in this project and the makefile would compile all of them.

------------------------------------------------------------------------------------------------------------------------------
First is the map generator code:input.c input_complex_map.c input_more_complex_map.c, which generate three versions of the map from easy , hard to hell.
Run with the command: ./input rows cols
                      ./input_complex_map rows cols
                      ./input_more_complex_map rows cols
Then, a file called input.txt would be generated.


------------------------------------------------------------------------------------------------------------------------------
The other files are the main running files. 

rrt_s.cpp and rrt_s_no_collision.cpp are serial code for path planning. The first is the one without collision rules while the second with the rules.
Run the code with: ./rrt_s input.txt
                   ./rrt_s_no_collision input.txt
You can get the time cost and the success rate.


------------------------------------------------------------------------------------------------------------------------------
rrt_p_region.cpp rrt_p_robot.cpp rrt_p_robot_no_collision.cpp rrt_p_robot_no_collision_weak.cpp:
These three files are parallelism code.

rrt_p_region.cpp is the one without the collision rule uses domain decomposition.
rrt_p_robot.cpp is the one without the collision rule uses robot assignment. (You don't need to run this one since it is embarrasingly paralleled)
rrt_p_robot_no_collision.cpp is the one with the collision rule uses robot assignment.

Run the code with: mpirun -n [number] ./code_name input.txt
You can get the same kinds of results


------------------------------------------------------------------------------------------------------------------------------
rrt_p_region_weak.cpp  rrt_p_robot_no_collision_weak.cpp are used for the weak scaling

First decide the number of ranks you want to use: sizen
Then run the map generator code: ./input[...] 100*sizen cols
Then run: mpirun -n sizen ./code_name input.txt


------------------------------------------------------------------------------------------------------------------------------
One thing to notice is that if you want to run the validation test, makesure the srand() function is commented.
Finally, some parameters can be changed in the code, such as the probability and the robot number.
