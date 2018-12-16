#include <stdio.h>
#include <stdlib.h>


int main(int argc, char **argv) {
	
	if (argc!=3) {
		printf("input rows cols\n");
		exit(EXIT_FAILURE);
	}
    int rows = atoi(argv[1]);
    int cols = atoi(argv[2]);

    int i, j;
    int ** matrix = NULL;
    matrix = malloc(rows * sizeof(int *));
        for (j = 0; j < rows; j++) {
            matrix[j] = malloc(cols * sizeof(int));
        }

    for(i = 0; i < rows; i++){
        for(j = 0; j < cols; j++){
            matrix[i][j]= 0;
        }
    }

    // set the obstacles in the simulation domain

    for(i = rows/10; i < rows/5; i++){
        for(j = cols*15/100; j < 17*cols/100; j++){
            matrix[i][j]= 1;
        }
    }

    int diag_r_start1 = 8*rows/10;     // set up one diagonal obstacle
    int diag_r_end1 = rows;
    int diag_c_start1 = cols/5;
    
    int color_start = 0;
    int color_end = cols/50;

    for(i = diag_r_start1; i < diag_r_end1 && i<rows; i++){
            for(j = diag_c_start1+color_start; j < diag_c_start1+color_end&& j <cols; j++){
                matrix[i][j]= 1;
            }
        color_start ++;
        color_end ++;
        if((diag_c_start1 + color_end)>cols ){
            break;
        }
    }

    int diag_r_start3 = 8*rows/10;     // set up one diagonal and vertical obstacle
    int diag_r_end3 = 9*rows/10;
    int diag_c_start3 = 5*cols/10;
    
    color_start = 0;
    color_end = 2;

    for(i = diag_r_start3; i < diag_r_end3 && i<rows; i++){
            for(j = diag_c_start3+color_start; j < diag_c_start3+color_end&& j <cols; j++){
                matrix[i][j]= 1;
            }
        color_start ++;
        color_end ++;
        if((diag_c_start3 + color_end)>cols ){
            break;
        }
    }
    if(diag_c_start3+color_end<cols-2){
    for(j =diag_c_start3+color_start-1; j<diag_c_start3+color_end-1;j++){
        for(i= diag_r_end3-1;i<rows; i++){
            matrix[i][j]= 1;
        }
        }
    }


	

    int diag_r_start2 = 0;    // set up another diagonal obstacle
    int diag_r_end2 = rows/5;
    int diag_c_start2 = cols/2;
    
    color_start = 0;
    color_end = cols/50;

    for(i = diag_r_end2; i > diag_r_start2 -1 && i<rows; i--){
            for(j = diag_c_start2+color_start; j < diag_c_start2+color_end && j<cols; j++){
                matrix[i][j]= 1;
            }
        color_start ++;
        color_end ++;
         if((diag_c_start2 + color_end)>cols ){
            break;
        }
    }

     for(i = 55*rows/100; i < 6*rows/10; i++){
        for(j = 4*cols/5; j < cols; j++){
            matrix[i][j]= 1;
        }
    }

    int diag_r_start4 = 5*rows/10;    // set up another diagonal obstacle
    int diag_r_end4 = 78*rows/100;
    int diag_c_start4 = 0;
    
    color_start = 0;
    color_end = 3;

    for(i = diag_r_end4; i > diag_r_start4 -1 && i>0; i--){
            for(j = diag_c_start4+color_start; j < diag_c_start4+color_end && j<cols; j++){
                matrix[i][j]= 1;
            }
        color_start ++;
        color_end ++;
         if((diag_c_start4 + color_end)>cols ){
            break;
        }
    }



     for(i = 55*rows/100; i < 6*rows/10; i++){
        for(j = 4*cols/5; j < cols; j++){
            matrix[i][j]= 1;
        }
    }

    for(i = 35*rows/100; i < 38*rows/100; i++){
        for(j = 1*cols/5; j < 30*cols/50; j++){
            matrix[i][j]= 1;
        }
    }


    const char*input_name ="input.txt";
    FILE * f = fopen(input_name, "w");
    if (f == NULL){
      return 0;
    }
    fprintf(f, "%d\n", rows);
    fprintf(f, "%d\n", cols);
    for(i = 0; i < rows; i++){
        for(j = 0; j < cols; j++){
            fprintf(f, "%d", matrix[i][j]);
        }
        fprintf(f, "\n");
    }

    fclose(f);
	//for (int i=0; i<cols; i++) free(matrix[i]);
    free(matrix);
}