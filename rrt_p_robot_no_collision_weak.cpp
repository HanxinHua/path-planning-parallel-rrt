#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//#include <omp.h>
#include "mpi.h"
//#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <map>
#include <vector>
#include <time.h>
#include <utility>
#include <unordered_set>
#include <iostream>

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
  MPI_Init(&argc,&argv);
  int rank, size;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  
  double Prob=0.6;
  int robot_number =100*size; //assign the same robot number for each task
  int i;
  int j;
  int rows, cols;
  vector<vector<int> > expected_robot;
  vector<vector<int> > expected_target;//expectation, robot, target
  vector<vector<int> > recv_robot;
  vector<vector<int> > recv_target;
  expected_robot.resize(9);
  expected_target.resize(9);
  recv_robot.resize(9);
  recv_target.resize(9);
  //read the rows and the cols
  if (rank == 0) {
    if (argc!=2) {
      printf("./rrt input.txt\n");
      exit(EXIT_FAILURE);
    }
    FILE*f = fopen(argv[1], "r");
    if(f==NULL){
      return 1;
    }
    
    size_t sz = 0;
    char * line = NULL;
    
    getline(&line, &sz, f);
    rows=atoi(line);
    getline(&line, &sz, f);
    cols=atoi(line);
    fclose(f);
  }
  MPI_Bcast(&rows,1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&cols,1, MPI_INT, 0, MPI_COMM_WORLD);
  
  int * matrix = (int *)malloc(rows * cols* sizeof(int));
  vector<int> task_robot;
  task_robot.assign(robot_number,0);
  vector<int> displs2 ; 
  vector<int> recvCount2 ;
  //read the whole gamemap
  if (rank == 0) {
    FILE*f = fopen(argv[1], "r"); 
    if(f==NULL){
      return 1;
    }
    
    size_t sz = 0;
    ssize_t len = 0;
    char * line = NULL;
    
    len = getline(&line, &sz, f);
    len = getline(&line, &sz, f);
    i=0;
    
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
    
    time_t t;
    int domain_size = rows * cols;
    
    /* Intializes random number generator for robots */
    /* 1 represents the robot */
    srand((unsigned) time(&t));
    
    for( int robot = 0 ; robot < robot_number ; robot++ ) {
      int index;
      index = rand() % domain_size;
      
      while(matrix[index]!=0 || index == rows*cols-1  ){ // cannot place the robot on an obstacle or the destination or a robot
	index = rand() % domain_size;
      }
      matrix[index] ++;
      task_robot[robot] = index; //store the robot index
    }
  }
  
  //Broadcast the whole gamemap
  MPI_Bcast(&matrix[0],rows*cols, MPI_INT, 0, MPI_COMM_WORLD);
  int stride = robot_number/size;
  int myRobotNumber;
  int myMinRobot;
  int left = robot_number%size;
  if (rank<left) {
    myRobotNumber = stride+1;
    myMinRobot = (stride+1)*rank;
  }
  else {
    myRobotNumber = stride;
    myMinRobot = (stride+1)*left+stride*(rank-left);
  }
  //resize the vector size
  vector<int> myRobot;
  myRobot.resize(myRobotNumber);
  //define the sendcounts and displacements
  vector<int> sendcounts;
  vector<int> displs;
  if (rank == 0) {
    for (int i=0; i<left; i++) {
      sendcounts.push_back(stride+1);
      displs.push_back((stride+1)*i);
    }
    for (int i = left; i<size; i++) {
      sendcounts.push_back(stride);
      displs.push_back((stride+1)* left+stride*(i-left));
    }		
  }
  //use scatterv to assign robot position
  MPI_Scatterv(&task_robot[0], &sendcounts[0], &displs[0], MPI_INT, &myRobot[0], myRobotNumber, MPI_INT, 0, MPI_COMM_WORLD);
  
  double commtime;
  
  double commstart = MPI_Wtime();
  double avetime;
  int newPlace;
  int cor;
  int robot_order;
  int target;
  unordered_set<int> set_down;
  unordered_set<int> unavaliable;
  vector<int> new_target_robot;
  vector<int> mysize(9,0);
  int reach=0;
  //go for 1000 steps
  for (int step=0; step<1000; step++) {
    int signal=myRobotNumber;
    
    for (unsigned i=0; i<myRobot.size(); i++) {
      //find the new direction for each robot in a cell
      
      cor = myRobot[i];
      newPlace=rrtstar(cor, rows*cols-1, cols, rows, Prob,matrix, expected_robot,expected_target, i+myMinRobot);
      //deal with robots reach the destination
      if (newPlace != 0) {
	myRobot[i]=newPlace;
	signal--;
	//store the number of robots reach the destination in each rank
	reach++;
	continue;
      }
      
    }
    // store the size for each expectations
    for (i = 0; i<9; i++) {
      mysize[i]=(int) expected_robot[i].size();
    }
    vector<int> whole_size;
    whole_size.resize(9*size);
    // gather the size for each expectations in rank 0
    MPI_Gather(&mysize[0], 9, MPI_INT,&whole_size[0] ,9, MPI_INT,0, MPI_COMM_WORLD);
    
    //use gatherv to get all the expectations with robots and the target position in rank 0
    int totalsize;
    for (int j=0; j<9; j++) {
      displs2.clear(); 
      recvCount2.clear();
      
      totalsize=0;
      for (int i=0; i<size; i++){		
	recvCount2.push_back(whole_size[i*9+j]);
	if (i>0) displs2.push_back(recvCount2[i-1]+displs2[i-1]);
	else displs2.push_back(0);
	totalsize+=recvCount2[i];
      }
      MPI_Barrier(MPI_COMM_WORLD);
      recv_robot[j].resize((unsigned)totalsize);
      recv_target[j].resize((unsigned)totalsize);
      MPI_Gatherv(&expected_robot[j][0],mysize[j],MPI_INT,&recv_robot[j][0],&recvCount2[0],&displs2[0],MPI_INT,0,MPI_COMM_WORLD);
      MPI_Gatherv(&expected_target[j][0],mysize[j],MPI_INT,&recv_target[j][0],&recvCount2[0],&displs2[0],MPI_INT,0,MPI_COMM_WORLD);
    }
    new_target_robot.clear();
    new_target_robot.assign(task_robot.size(),-1);
    int updatesize;
    int newsig;
    //get the size of the robots need to deal with in this time step
    MPI_Reduce(&signal,&newsig,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);
    
    //rank 0 uses the method of priority queue and get the new position for all the robots without collision
    if (rank == 0) {
      for (unsigned i =0; i<recv_robot.size(); i++) {
	
	//break the loop early once all robots are settled
	if ((int)set_down.size() == newsig) break;
	for (unsigned j=0; j<recv_robot[i].size(); j++) {
	  
	  //break the loop early once all robots are settled
	  if ((int)set_down.size() == newsig) break;
	  robot_order = recv_robot[i][j];
	  target = recv_target[i][j];
	  
	  //continue loop once the target position is unavailable or the robot has been settled
	  if (set_down.find(robot_order) != set_down.end() || unavaliable.find(target) != unavaliable.end()) continue;
	  new_target_robot[robot_order] = target;
	  set_down.insert(robot_order);
	  unavaliable.insert(target);
	}
      }
      
      task_robot.clear();
      //reget the number of robots need to be deal with so as to balance the workload in each task
      for (unsigned ig = 0; ig<new_target_robot.size(); ig ++) {
	if (new_target_robot[ig] != -1) task_robot.push_back(new_target_robot[ig]);
      }
      updatesize = (int)task_robot.size();
    }
    
    //reallocate the robot number for each task
    MPI_Bcast(&updatesize,1, MPI_INT, 0, MPI_COMM_WORLD);
    stride = updatesize/size;
    left = updatesize%size;
    if (rank<left) {
      myRobotNumber = stride+1;
      myMinRobot = (stride+1)*rank;
    }
    else {
      myRobotNumber = stride;
      myMinRobot = (stride+1)*left+stride*(rank-left);
    }
    
    myRobot.clear();
    myRobot.resize(myRobotNumber);
    sendcounts.clear();
    displs.clear();
    //define the sendcounts and displacement
    if (rank == 0) {
      for (int i=0; i<left; i++) {
	sendcounts.push_back(stride+1);
	displs.push_back((stride+1)*i);
      }
      for (int i = left; i<size; i++) {
	sendcounts.push_back(stride);
	displs.push_back((stride+1)* left+stride*(i-left));
      }		
    }
    //use scatterv to assign robot positions for each task
    MPI_Scatterv(&task_robot[0], &sendcounts[0], &displs[0], MPI_INT, &myRobot[0], myRobotNumber, MPI_INT, 0, MPI_COMM_WORLD);
    unavaliable.clear();
    set_down.clear();
    expected_robot.clear();
    expected_target.clear();
    expected_robot.resize(9);
    expected_target.resize(9);
    recv_robot.clear();
    recv_target.clear();
    recv_robot.resize(9);
    recv_target.resize(9);
    
  }
  
  
  commtime = MPI_Wtime() - commstart;
  MPI_Reduce(&commtime,&avetime,1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);
  int sum = 0;

  MPI_Reduce(&reach,&sum,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);
  if (rank == 0) {
    printf("the average time is %f\n",avetime/size);
    printf("Time resolution is %lf\n",MPI_Wtick());
    printf("The successful percentage is %f\n",(double)sum/robot_number);
  }
  free(matrix);
  MPI_Finalize();
}
