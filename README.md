# path-planning-parallel-rrt


There are total 10 c/c++ files in this project and the makefile would compile all of them.

------------------------------------------------------------------------------------------------------------------------------
First is the map generator code:input.c input_complex_map.c input_more_complex_map.c, which generate three versions of the map from easy , hard to hell.
<br>
Run with the command:  <br><br>./input rows cols
                      <br>./input_complex_map rows cols
                      <br>./input_more_complex_map rows cols
<br><br>Then, a file called input.txt would be generated.


------------------------------------------------------------------------------------------------------------------------------
The other files are the main running files. 

<br>rrt_s.cpp and rrt_s_no_collision.cpp are serial code for path planning. The first is the one without collision rules while the second with the rules.
<br>
<br>Run the code with: <br><br>./rrt_s input.txt
                   <br>./rrt_s_no_collision input.txt
<br><br>You can get the time cost and the success rate.


------------------------------------------------------------------------------------------------------------------------------
rrt_p_region.cpp rrt_p_robot.cpp rrt_p_robot_no_collision.cpp rrt_p_robot_no_collision_weak.cpp:
<br>These three files are parallelism code.

<br>rrt_p_region.cpp is the one without the collision rule uses domain decomposition.
<br>rrt_p_robot.cpp is the one without the collision rule uses robot assignment. (You don't need to run this one since it is embarrasingly paralleled)
<br>rrt_p_robot_no_collision.cpp is the one with the collision rule uses robot assignment.

<br>Run the code with: mpirun -n [number] ./code_name input.txt
<br>You can get the same kinds of results


------------------------------------------------------------------------------------------------------------------------------
rrt_p_region_weak.cpp  rrt_p_robot_no_collision_weak.cpp are used for the weak scaling

<br>First decide the number of ranks you want to use: sizen
<br>Then run the map generator code: ./input[...] 100*sizen cols
<br>Then run: mpirun -n sizen ./code_name input.txt


------------------------------------------------------------------------------------------------------------------------------
One thing to notice is that if you want to run the validation test, makesure the srand() function is commented.
<br>Finally, some parameters can be changed in the code, such as the probability and the robot number.
<br>You can run the code anywhere. But the results we used in the paper is from the tests on stampede.
