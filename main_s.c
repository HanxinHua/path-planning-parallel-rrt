#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//#include <omp.h>
#include <mpi.h>
//#include <sstream>

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

    int * matrix ;
    matrix = malloc(rows * cols* sizeof(int));

    i=0;
    //int leap=0;
    int load_number =0;

    while((len = getline(&line, &sz, f)) >0 && i<rows){      // read in the obstacles info
      printf("%d number of len is %zu\n",i,len);
      
      for(j=0;j<len-1;j++){
        if(line[j]=='1'){
            matrix[i*rows+j]=-1;
            printf("find one -1\n");
        }
        else{
            matrix[i*rows+j]=0;
            }
        }
      printf("row %d\n",i);
      i++;
      if(i==rows){
        printf("heihei");
        break;
        printf("haha");
      }
    }

    printf("finished loading");

    free(line);
    
    int robot_number =1000;
    time_t t;
    int domain_size = rows * cols;

    /* Intializes random number generator */
   srand((unsigned) time(&t));

   /* Print 100 random numbers from 0 to 1000,000 */
   for( i = 0 ; i < robot_number ; i++ ) {
      int index = rand() % domain_size;
      while(matrix[index]==-1){
        int index = rand() % domain_size;
      }
      matrix[index] ++;
   }

   for( i = 0 ; i < rows * cols ; i++ ) {
       //printf("%d",matrix[i] );
   }


}