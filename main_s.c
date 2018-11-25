#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//#include <omp.h>
//#include <mpi.h>
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

    while((len = getline(&line, &sz, f))>0){
      printf("%d number of len is %zu\n",i,len);   // read in the obstacles info
      load_number=0;
      for(j=0;j<len-1;j++){
        if(line[j]=='1'){
            matrix[i*rows+j]=-1;
            printf("find one -1\n");
            load_number++;
        }
        else{
            matrix[i*rows+j]=0;
            load_number++;
            }
        }
      int int_row = (int) rows;
      if(i==int_row-1){
          len = len*0;
          printf("len is %zu\n",len);
          break;
      }
      printf("row %d loadnumber is %d\n",i,load_number);
      i++;
    }

    printf("cc\n");
    printf("finished loading\n");

    free(line);
    
    int robot_number =1000;
    time_t t;
    int domain_size = rows * cols;

    /* Intializes random number generator */
   srand((unsigned) time(&t));

   printf("cc2\n");
   
    i = 0;
    
   //Print 100 random numbers from 0 to 1000,000 
  
   

   for( i = 0 ; i < rows * cols ; i++ ) {
       printf("%d",matrix[i] );
   }

    for( int robot = 0 ; robot < robot_number ; robot++ ) {
    printf("I am in for loop\n");
    int index;
    index = rand() % domain_size;
    printf("index number is %d\n",index);
    while(matrix[index]==(-1)){
        index = rand() % domain_size;
    }
    matrix[index] ++;
    printf("robot number is %d\n",robot);
    }


}