#include <stdio.h>
#include <stdlib.h>


int main(int argc, char **argv) {
    int rows = 100;
    int cols = 100;
    int i, j;
    int ** matrix = NULL;
    matrix = malloc(rows * sizeof(int *));
        for (j = 0; j < cols; j++) {
            matrix[j] = malloc(cols * sizeof(int));
        }

    for(i = 0; i < rows; i++){
        for(j = 0; j < cols; j++){
            matrix[i][j]= 0;
        }
    }

    // set the obstacles in the simulation domain

    for(i = rows/10; i < rows/5; i++){
        for(j = cols*15/100; j < 16*cols/100; j++){
            matrix[i][j]= 1;
        }
    }

    int diag_r_start1 = 8*rows/10;
    int diag_r_end1 = rows;
    int diag_c_start1 = cols/5;
    //int diag_c_end1 = 420;
    int color_start = 0;
    int color_end = rows/50;

    for(i = diag_r_start1; i < diag_r_end1; i++){
            for(j = diag_c_start1+color_start; j < diag_c_start1+color_end; j++){
                matrix[i][j]= 1;
            }
        color_start ++;
        color_end ++;
    }

    int diag_r_start2 = 0;
    int diag_r_end2 = rows/5;
    int diag_c_start2 = cols/2;
    //int diag_c_end2 = 720;
    color_start = 0;
    color_end = rows/50;

    for(i = diag_r_end2; i > diag_r_start2 -1; i--){
            for(j = diag_c_start2+color_start; j < diag_c_start2+color_end; j++){
                matrix[i][j]= 1;
            }
        color_start ++;
        color_end ++;
    }

     for(i = 55*rows/100; i < 6*rows/10; i++){
        for(j = 4*cols/5; j < cols; j++){
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
    free(matrix);
}