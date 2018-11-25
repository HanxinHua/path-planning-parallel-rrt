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



int rrt(int robot, int dest, int col, int row, double prob) {
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
		myqueue.push(costCalculation(neighbor, dest, col));
	}
	int best = myqueue.top();
	srand(time(0));
    double p = rand()%1000/(double)1000;
	if (p < prob) return best;
	else {
		int pick = rand()%stencil.size();
		return (robot+stencil[pick]);
	}	
}