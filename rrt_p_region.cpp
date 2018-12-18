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
int rrastar(int robot, int dest, int col, int row, double prob, int * puzzel, int my_xmin) {
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
    if (puzzel[neighbor-my_xmin+col] != -1) {
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
    while (puzzel[robot+stencil[pick]-my_xmin+col] == -1) {
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
  double commtime;
  double Prob=0.6;
  int robot_number =1000;
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
		//printf("%d\n",i);
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
      
      while(matrix[index]==(-1) || index == rows*cols-1){ // cannot place the robot on an obstacle or the destination
	index = rand() % domain_size;
      }
      matrix[index] ++;
    }
  }
  
  //Broadcast the whole gamemap
  MPI_Bcast(&matrix[0],rows*cols, MPI_INT, 0, MPI_COMM_WORLD);
  
  int stride = rows/size;
  int my_xmin,my_xmax;
  int left = rows%size;
  //define the domain for each task
  if (rank<left) {
    my_xmin = (stride+1) * rank;
    my_xmax = (stride+1) * (rank + 1);
  }
  else {
    my_xmin = (stride+1) * left+stride*(rank-left);
    my_xmax = (stride+1) * left+stride*(rank-left+1);
  }
  my_xmin*=cols;
  my_xmax*=cols;
  int my_lx = my_xmax-my_xmin;
  int my_lx_in = my_lx;
  if (rank == size-1) my_lx_in-=1;
  int * gamemap = (int *)malloc((my_lx+2*cols)* sizeof(int));
  int * gamemap_new = (int *)malloc((my_lx+2*cols)* sizeof(int));
  
  //Initialize the gamemap for each task
  for (i=cols; i<my_lx+cols; i++) {
    gamemap[i] = matrix[i+my_xmin-cols];
    gamemap_new[i]=gamemap[i];
  }
  if (rank != 0) {
    for (i=0; i<cols; i++) {
      gamemap[i] = matrix[i+my_xmin-cols];
      gamemap_new[i]=gamemap[i];
    }
  }
  if (rank != size -1 ) {
    for (i=my_lx+cols; i<my_lx+2*cols; i++) {
      gamemap[i] = matrix[i+my_xmin-cols];
      gamemap_new[i]=gamemap[i];
    }
  }
  
  //delete robots in halo
  for (i=0; i<cols; i++) { 
    if (gamemap[i]>0){
      gamemap[i]=0;
      gamemap_new[i]=0;
    }
    if (gamemap[i+my_lx+cols]>0) {
      gamemap[i+my_lx+cols]=0;
      gamemap_new[i+my_lx]=0;
    }
  }
  
  int newPlace;
  int prev, next;
  
  prev=rank-1;
  next=rank+1;
  
  //Define the buff to receive the information
  int * buffprev = (int *)malloc(cols* sizeof(int));
  int * buffnext = (int *)malloc(cols* sizeof(int));
  double commstart = MPI_Wtime();
  double avetime;
  
  //go for 1000 steps
  for (int step=0; step<1000; step++) {		
    for (int cor=cols; cor<my_lx_in+cols; cor++) {
      
      //find the new direction for each robot in a cell
      while (gamemap[cor]>0) {
	newPlace=rrastar(cor+my_xmin-cols, rows*cols-1, cols, rows, Prob,gamemap, my_xmin)-my_xmin+cols;
	gamemap_new[newPlace]++;
	gamemap_new[cor]--;
	gamemap[cor]--;
      }
    }
    
    int count = 0;
    MPI_Request reqs[4];
    //send the robot information in the halo to the neighbour tasks
    if (rank != 0){
      MPI_Isend(&gamemap_new[0], cols, MPI_INT, prev, 0, MPI_COMM_WORLD, reqs + count);
      count = count + 1;
      MPI_Irecv(&buffprev[0], cols, MPI_INT, prev, 0, MPI_COMM_WORLD, reqs + count);
      count = count + 1;
    }
    if (rank != size-1){
      MPI_Isend(&gamemap_new[my_lx+cols], cols, MPI_INT, next, 0, MPI_COMM_WORLD, reqs + count);
      count = count + 1;
      
      MPI_Irecv(&buffnext[0], cols, MPI_INT, next, 0, MPI_COMM_WORLD, reqs + count);
      count = count + 1;
    }
    
    if (count > 0) MPI_Waitall(count, reqs, MPI_STATUSES_IGNORE);
    
    //update the robot information for each task
    if (rank !=0) {
      for (i = 0; i<cols; i++) {
	if (buffprev[i]>0) gamemap_new[i+cols]+=buffprev[i];
      }
    }
    if (rank !=size-1) {
      for (i = 0; i<cols; i++) {
	if (buffnext[i]>0) gamemap_new[i+my_lx]+=buffnext[i];
      }
    }
    
    //delete robots in halo
    for (i=0; i<cols; i++) { 
      if (gamemap_new[i]>0){
	gamemap_new[i]=0;
      }
      if (gamemap_new[i+my_lx+cols]>0) {
	gamemap_new[i+my_lx+cols]=0;
      }
    }
    for (int cor=cols; cor<my_lx+cols; cor++) {
      gamemap[cor]=gamemap_new[cor];
      
    }	
    
  }
  commtime += MPI_Wtime() - commstart;
  MPI_Reduce(&commtime,&avetime,1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);
  if (rank == 0) {
    printf("the average time is %f\n",avetime/size);
    printf("Time resolution is %lf\n",MPI_Wtick());
  }
  if (rank == size-1) printf("The successful percentage is %f\n",(double)gamemap[my_lx+cols-1]/robot_number);
  MPI_Finalize();
  
}
