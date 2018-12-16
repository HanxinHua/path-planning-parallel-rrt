#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//#include <omp.h>
//#include <mpi.h>
//#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <map>
#include <vector>
#include <time.h>
#include <utility>
#include <unordered_set>

using namespace std;

//Caluculate the cost between the point and the destination
double costCalculation(int place, int dest, int col) {
  int xp, yp, xd, yd;
  xp = place / col;
  yp = place % col;
  xd = dest / col;
  yd = dest % col;
  return sqrt((xp-xd)*(xp - xd)+(yp-yd)*(yp - yd));
}


//Use the rra* algorithm to find the direction to go
int rrtstar(int robot, int dest, int col, int row, double prob, int * puzzel ,vector<vector<int> > & expected_robot,vector<vector<int> > & expected_target, int robot_number) {
  map<double, int> mymap;
  vector<int> stencil;
  //Store the neighbours with the considerations of the boundaries and the obstacles

  if (robot%col !=0) {
    if (puzzel[robot-1] != -1) stencil.push_back(-1);
    if (robot/col !=0 && puzzel[robot-1-col] != -1) stencil.push_back(-1-col);
    if (robot/col !=row-1 && puzzel[robot-1+col] != -1) stencil.push_back(-1+col);
  }
  if (robot%col !=col-1) {
    if (puzzel[robot+1] != -1) stencil.push_back(1);
    if (robot/col !=0 && puzzel[robot+1-col] != -1) stencil.push_back(1-col);
  if (robot/col !=row-1 && puzzel[robot+1+col] != -1) stencil.push_back(1+col);
  }
  if (robot/col !=0 && puzzel[robot-col] != -1) stencil.push_back(-col);
  if (robot/col !=row-1 && puzzel[robot+col] != -1) stencil.push_back(col);
  int neighbor;
  //add the neighbours with the cost to the map
  for (unsigned i = 0; i < stencil.size(); i++) {
    neighbor = robot + stencil[i];
    //deal with the robots who reaches the destination
      if (neighbor == dest) return neighbor;
      mymap[costCalculation(neighbor, dest, col)]=neighbor;
  }
  int best = mymap.begin()->second;
  srand(time(0));
  double p = rand()%1000/(double)1000;
  //p<prob, give the best direction the highest priority
  unsigned start=0;
  unsigned end = stencil.size();
  unordered_set<int> random;
  if (p < prob) {
	  expected_robot[0].push_back(robot_number);
	  expected_target[0].push_back(best);
	  start=1;
	  random.insert(best);
  }
  //random pick the left directions with the decreasing priority
  for (unsigned i = start; i<end; i++) {
	  unsigned pick = rand()%stencil.size();
      while (random.find(robot+stencil[pick]) != random.end()) {
        pick = rand()%stencil.size();
      }
	  random.insert(robot+stencil[pick]);
	  expected_robot[i].push_back(robot_number);
	  expected_target[i].push_back(robot+stencil[pick]);
  }
  //add the robot own position with the least priority
  expected_robot[end].push_back(robot_number);
  expected_target[end].push_back( robot);
  return 0;
}




int main(int argc, char **argv) {

  FILE*f = fopen(argv[1], "r");
  if(f==NULL){
    return 1;
  }
  double Prob=0.5;
  int i;
  int j;
  size_t sz = 0;
  ssize_t len = 0;
  char * line = NULL;
  int rows, cols;
  vector<vector<int> > expected_robot;
  vector<vector<int> > expected_target;//expectation, robot, target
  expected_robot.resize(9);
  expected_target.resize(9);
  //read the rows and the cols
  len = getline(&line, &sz, f);
  rows = atoi(line);
  len = getline(&line, &sz, f);
  cols = atoi(line);
  
  int * matrix = (int *)malloc(rows * cols* sizeof(int));
  i=0;
  //read the whole gamemap
  while((len = getline(&line, &sz, f))>0){
    for(j=0;j<len-1;j++){
      if(line[j]=='1'){
	matrix[i*cols+j]=-1;
      }
      else{
	matrix[i*cols+j]=0;
      } 
    }
    i++;
  }
  
  free(line);
  
  int robot_number =50;
  time_t t;
  int domain_size = rows * cols;
  
  /* Intializes random number generator */
  /* 1 represents the robot */
  
  srand((unsigned) time(&t));
  vector<int> task_robot;
  task_robot.assign(robot_number,0);
  for( int robot = 0 ; robot < robot_number ; robot++ ) {
    int index;
    index = rand() % domain_size;
    
    while(matrix[index]!=0 || index == rows*cols-1  ){ // cannot place the robot on an obstacle or the destination or a robot
      index = rand() % domain_size;
    }
    matrix[index] ++;
	task_robot[robot] = index; //store the robot index
  }

	
  //go for 1000 steps
  int newPlace;
  int cor;
  int robot_order;
  int target;
  unordered_set<int> set_down; 
  unordered_set<int> unavaliable;
  int signal=robot_number;
  for (int step=0; step<1000; step++) {
    for (unsigned i=0; i<task_robot.size(); i++) {
      //find the new direction for each robot in a cell
	  cor = task_robot[i];
	  //skip the destination
	  if (cor == rows*cols-1) continue;
	  newPlace=rrtstar(cor, rows*cols-1, cols, rows, Prob,matrix, expected_robot,expected_target, i);
	  //deal with robots reach the destination
	  if (newPlace != 0) {
		  task_robot[i]=newPlace;
		  signal--;
	      continue;
	  }
    }
	//use the method of priority queue and get the new position for all the robots without collision
	for (unsigned i =0; i<expected_robot.size(); i++) {
		//break the loop early once all robots are settled
		if ((int)set_down.size() == signal) break;
		for (unsigned j=0; j<expected_robot[i].size(); j++) {
			//break the loop early once all robots are settled
			if ((int)set_down.size() == signal) break;
			robot_order = expected_robot[i][j];
			target = expected_target[i][j];
			//continue loop once the target position is unavailable or the robot has been settled
			if (set_down.find(robot_order) != set_down.end() || unavaliable.find(target) != unavaliable.end()) continue;
			task_robot[robot_order] = target;
			set_down.insert(robot_order);
			unavaliable.insert(target);
		}
	}
	
	//rearrange the memory
	unavaliable.clear();
	set_down.clear();
	expected_robot.clear();
	expected_target.clear();
	expected_robot.resize(9);
	expected_target.resize(9);

    }
  int sum = 0;
  for (unsigned i = 0; i<task_robot.size(); i++) {
	  if (task_robot[i]==rows*cols-1) sum++;
  }
  printf("The successful percentage is %f\n",(double)sum/robot_number);
  free(matrix);
  
}
