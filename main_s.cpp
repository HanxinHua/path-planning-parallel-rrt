#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//#include <omp.h>
//#include <mpi.h>
//#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <queue>
#include <vector>
#include <time.h>

using namespace std;

double costCalculation(int place, int dest, int col) {
	int xp, yp, xd, yd;
	xp = place / col;
	yp = place % col;
	xd = dest / col;
	yd = dest % col;
	return sqrt((xp-xd)*(xp - xd)+(yp-yd)*(yp - yd));
}



int rrt(int robot, int dest, int col, int row, double prob, int * puzzel) {
	priority_queue<double, vector<double>, greater<double> > myqueue;
	vector<int> stencil;
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
	for (int i = 0; i < stencil.size(); i++) {
		neighbor = robot + stencil[i];
		if (puzzel[neighbor] != -1) {
			if (neighbor == dest) return neighbor;
			myqueue.push(costCalculation(neighbor, dest, col));
		}
	}
	int best = myqueue.top();
	srand(time(0));
    double p = rand()%1000/(double)1000;
	if (p < prob) return best;
	else {
		int pick = rand()%stencil.size();
		while (puzzel[robot+stencil[pick]] == -1) {
			pick = rand()%stencil.size();
		}
		return (robot+stencil[pick]);
	}	
}


int main(int argc, char **argv) {

    FILE*f = fopen(argv[1], "r");
    if(f==NULL){
        return 1;
    }

    int i;
    int j;
    size_t sz = 0;
    ssize_t len = 0;
    char * line = NULL;
    int intputarray[2];
    int rows, cols;

    for (i = 0; i < 2; i++) {
            len = getline(&line, &sz, f);
            int value = atoi(line);
            intputarray[i] = value;
        }

    rows = intputarray[0];
    cols = intputarray[1];

    int * matrix = (int *)malloc(rows * cols* sizeof(int));
	int * matrix_new = (int *)malloc(rows * cols* sizeof(int));
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
      int int_row = (int) rows;
      i++;
    }

    free(line);
    for (i=0; i<rows*cols; i++) {
		matrix_new[i]=matrix[i];
	}
    int robot_number =100;
    time_t t;
    int domain_size = rows * cols;

    /* Intializes random number generator */
   srand((unsigned) time(&t));
    
   //Print 100 random numbers from 0 to 1000,000 

    for( int robot = 0 ; robot < robot_number ; robot++ ) {
		int index;
		index = rand() % domain_size;

		while(matrix[index]==(-1) && index == rows*cols-1){
			index = rand() % domain_size;
		}
		matrix[index] ++;
    }
	
	int newPlace;
	for (int step=0; step<10000; step++) {
		printf("step %d\n",step);
		for (int cor=0; cor<rows*cols-1; cor++) {
				while (matrix[cor]>0) {
					newPlace=rrt(cor, rows*cols-1, cols, rows, 0.5,matrix);
					matrix_new[newPlace]++;
					matrix[cor]--;
				}
		}
		for (int cor=0; cor<rows*cols; cor++) {
				matrix[cor]=matrix_new[cor];
		}
		
	}
	printf("The successful percentage is %f\n",(double)matrix[rows*cols-1]/robot_number);

}