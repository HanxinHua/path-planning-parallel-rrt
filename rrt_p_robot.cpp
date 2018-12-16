#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//#include <omp.h>
#include <mpi.h>
//#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <map>
#include <vector>
#include <time.h>

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
int rrastar(int robot, int dest, int col, int row, double prob, int * puzzel) {
  map<double, int> mymap;
  vector<int> stencil;
  //Store the neighbours with the considerations of the boundaries
  if (robot%col !=0) {
    stencil.push_back(-1);
    if (robot/col !=0) stencil.push_back(-1-col);
    if (robot/col !=row-1) stencil.push_back(-1+col);
  }
  if (robot%col !=col-1) {
    stencil.push_back(1);
    if (robot/col !=0) stencil.push_back(1-col);
    if (robot/col !=row-1) stencil.push_back(1+col);
  }
  if (robot/col !=0) stencil.push_back(-col);
  if (robot/col !=row-1) stencil.push_back(col);
  int neighbor;
  
  //add the neighbours with the cost to the map
  for (unsigned i = 0; i < stencil.size(); i++) {
    neighbor = robot + stencil[i];
    //ignore the obstacles
    if (puzzel[neighbor] != -1) {
      if (neighbor == dest) return neighbor;
      mymap[costCalculation(neighbor, dest, col)]=neighbor;
    }
  }
  int best = mymap.begin()->second;
  srand(time(0));
  double p = rand()%1000/(double)1000;
  //p<prob, go to the best direction
  if (p < prob) return best;
  //else, random pick a direction but not the obstacle
  else {
    int pick = rand()%stencil.size();
    while (puzzel[robot+stencil[pick]] == -1) {
      pick = rand()%stencil.size();
    }
    return (robot+stencil[pick]);
  }	
}


int main(int argc, char **argv) {
  MPI_Init(&argc,&argv);
  int rank, size;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  
  double Prob=0.4;
  int robot_number =160;
  int i;
  int j;
  int rows, cols;
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
  int * robotIndex = (int *)malloc(robot_number* sizeof(int));
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
	  matrix[i*rows+j]=-1;
	}
	else{
	  matrix[i*rows+j]=0;
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
      
      while(matrix[index]==(-1) && index == rows*cols-1){ // cannot place the robot on an obstacle or the destination
	index = rand() % domain_size;
      }
      robotIndex[robot] = index;
    }
  }
  
  //Broadcast the whole gamemap
  MPI_Bcast(&matrix[0],rows*cols, MPI_INT, 0, MPI_COMM_WORLD);
  int stride = robot_number/size;
  int myRobotNumber;
	int left = robot_number%size;
	if (rank<left) {
		myRobotNumber = stride+1;
	}
	else {
		myRobotNumber = stride;
	}
	//resize the vector size
	int * myRobot = (int *)malloc(myRobotNumber* sizeof(int));
	
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
	//use scatterv to assign data
	MPI_Scatterv(&robotIndex[0], &sendcounts[0], &displs[0], MPI_INT, &myRobot[0], myRobotNumber, MPI_INT, 0, MPI_COMM_WORLD);

  double commtime;

  /*
    int sum=0;
    int total;
    for (int cor=cols; cor<my_lx+cols; cor++) {
    gamemap[cor]=gamemap_new[cor];
    if (gamemap_new[cor]>0) sum+=gamemap_new[cor];
    
    }
    MPI_Reduce(&sum,&total,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);
    if (rank == 0) printf("total: %d\n",total);
  */
  
  //Define the buff to receive the information

  double commstart = MPI_Wtime();
  double avetime;
  
  //go for 1000 steps
  for (int step=0; step<1000; step++) {		
    for (int i=0;i<myRobotNumber; i++) {
		
		if (myRobot[i] != rows*cols-1) {
			myRobot[i]=rrastar(myRobot[i], rows*cols-1, cols, rows, Prob,matrix);
		}
    
	}
  }
  commtime += MPI_Wtime() - commstart;
  MPI_Reduce(&commtime,&avetime,1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);
  int total = 0;
  int sum = 0;
  for (int i=0;i<myRobotNumber; i++) {
		
		if (myRobot[i] == rows*cols-1) {
			total++;
		}
  }
    MPI_Reduce(&total,&sum,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);
  if (rank == 0) {
    printf("the average time is %f\n",avetime/size);
    printf("Time resolution is %lf\n",MPI_Wtick());
	printf("The successful percentage is %f\n",(double)sum/robot_number);
  }
  MPI_Finalize();
  
}
