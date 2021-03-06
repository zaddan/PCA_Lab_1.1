#include <stdio.h>
#include "gen_matrix.h"
#include "my_malloc.h"
#include <math.h>
#include <cilk/cilk.h>

void print_matrix(double *result, int numOfRows, int numOfCols) {
  int x, y;
  for (y = 0; y < numOfRows; ++y) {
    for (x = 0; x < numOfCols; ++x) {
      printf("%f ", result[y * numOfCols+ x]);
    }
    printf("\n");
  }
  printf("\n");
}

void print_matrix_2(double *result, int numOfRows, int numOfCols) {
  int x, y;
  for (y = 0; y < numOfRows; ++y) {
    for (x = 0; x < numOfCols; ++x) {
        printf("%f ", result[y * numOfCols+ x]);
    }
    printf("\n");
  }
  //printf("\n");
}



double *matmul (double *result, double **r, double** r2, int num_mats, int tag, int m, int n, int p, double * finalsum) {
    int MatrixDone = 0;
    double *a;
    double *b = r[1];
    while(1) {
        //--- calculatee the matrix 
        if(MatrixDone==0)
            a = &r[MatrixDone][tag*m*n];
        else {
            a = result;
            result = &r2[MatrixDone][tag*m*n];
        }
        int i,j,k;
        double sum;
        for (i=0; i<m; i++) {
            // for each row of c
            for (j=0; j<p; j++) {
                // for each column of c
                sum = 0.0f; // temporary value
                for (k=0; k<n; k++) {
                    // dot product of row from a and column from b
                    sum += a[i*n+ k]*b[k*p+j];
                }
                result[i*n + j] = sum;
            }

        }
        MatrixDone++;
//        if (tag == 0) {
//        //    print_matrix(result, m, n);
//        }
        if (MatrixDone == (num_mats -1)) {
            break;
        }
        b = r[MatrixDone+1];
    }
    int i;
    double temp=0;
    for(i=0;i<m*n;i++){
        temp+=result[i];
    } 
    finalsum[tag] = temp;
    return result;
}
 
main(int argc, char *argv[])  {
  //int numCilkThread= 16; 
    
  double **r;
  int i;
  int num_arg_matrices;
  int numtasks, dest, source, rc, count, tag=1;  
  numtasks =1; 
  char inmsg, outmsg='x';
  int debug_perf = atoi(argv[1]);
  int test_set = atoi(argv[2]);
  matrix_dimension_size = atoi(argv[3]);
  //MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
  int counter; 
  for (counter = 15;  counter >0; counter--) {
      if (matrix_dimension_size%counter == 0) {
          numtasks = counter;
          break; 
      }
  }
  /*printf("matrix_dimension_size %d\n", matrix_dimension_size);*/
  /*printf("numtasks%d\n", numtasks);*/
          
   num_arg_matrices = init_gen_sub_matrix(test_set);
  //--- allocating the space for matrix  
  r = (double **)my_malloc(sizeof(double *) * num_arg_matrices);
  double **r2 = (double **)my_malloc(sizeof(double *) * num_arg_matrices);
  int resultColumnSize =  matrix_dimension_size;
  int resultRowSize =  matrix_dimension_size;
  double** result = (double **)my_malloc(sizeof(double) * numtasks);
  double *sum = (double*) my_malloc(sizeof(double) * numtasks);
  for(i=0;i<numtasks;i++) {
      result [i] = (double *)my_malloc(sizeof(double) * resultRowSize* resultColumnSize/numtasks);
  }
  // ---- generating two sub_matrices
  for (i = 0; i < num_arg_matrices; ++i) {
      r[i] = (double *)my_malloc(sizeof(double) * resultRowSize* resultColumnSize);
      r2[i] = (double *)my_malloc(sizeof(double) * resultRowSize* resultColumnSize);
      if (gen_sub_matrix(0, test_set, i, r[i], 0, matrix_dimension_size-1, 1, 0, matrix_dimension_size-1, 1, 1) == NULL) {
              printf("inconsistency in gen_sub_matrix\n");
              exit(1);
          }
  } 
  if (debug_perf == 0) {
      for (i = 0; i < num_arg_matrices; ++i) {
    ; 
      printf("argument matrix %d\n", i);
      print_matrix(r[i], matrix_dimension_size, matrix_dimension_size);
    }
   ;
  }
//  for(i=0; i<numtasks; i++)
//     print_matrix(r[i], matrix_dimension_size/numtasks, resultColumnSize);
//  exit(0);

  // ---- calculate the partial sum (multiplication stage)
  int   j;
  int   k;
  int m = matrix_dimension_size/numtasks; 
  int n = matrix_dimension_size; 
  int p = matrix_dimension_size;
  double *a;
  double *b = r[1];
  tag = 0; //possible change later 
  while(tag<numtasks) { 
      result[tag] = cilk_spawn matmul(result[tag], r, r2, num_arg_matrices, tag, m, n, p, sum);
      tag++;
  }
  cilk_sync;


  /// print the result 
 
  int printed = 0;
  if (debug_perf == 0) { 
      for(i=0; i<numtasks; i++){
          print_matrix_2(result[i], matrix_dimension_size/numtasks, matrix_dimension_size);
      } 

  }else{
      double *finalSum = (double*)my_malloc(sizeof(double));
      *finalSum = 0;
      tag=0; 
      while(tag<numtasks) { 
          *finalSum+=sum[tag];
          tag++;
      }
      printf("%f\n",*finalSum);
  }

}
