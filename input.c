#include <stdio.h>
#include <stdlib.h>


int main(int argc, char **argv) {
    int rows = 1000;
    int cols = 1000;
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

    for(i = 100; i < 200; i++){
        for(j = 150; j < 160; j++){
            matrix[i][j]= 1;
        }
    }

    int diag_r_start1 = 800;
    int diag_r_end1 = 1000;
    int diag_c_start1 = 200;
    //int diag_c_end1 = 420;
    int color_start = 0;
    int color_end = 20;

    for(i = diag_r_start1; i < diag_r_end1; i++){
            for(j = diag_c_start1+color_start; j < diag_c_start1+color_end; j++){
                matrix[i][j]= 1;
            }
        color_start ++;
        color_end ++;
    }

    int diag_r_start2 = 0;
    int diag_r_end2 = 200;
    int diag_c_start2 = 500;
    //int diag_c_end2 = 720;
    color_start = 0;
    color_end = 20;

    for(i = diag_r_end2; i > diag_r_start2 -1; i--){
            for(j = diag_c_start2+color_start; j < diag_c_start2+color_end; j++){
                matrix[i][j]= 1;
            }
        color_start ++;
        color_end ++;
    }

     for(i = 550; i < 600; i++){
        for(j = 800; j < 1000; j++){
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