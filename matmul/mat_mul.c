#include "mpi.h"
#include <stdio.h>
#include "gen_matrix.h"
#include "my_malloc.h"
#include <math.h>



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


//void matmul(float **A, float **B, float **C) {
//  float sum;
//  int   i;
//  int   j;
//  int   k;
// 
//  for (i=0; i<M; i++) {
//    // for each row of C
//    for (j=0; j<N; j++) {
//      // for each column of C
//      sum = 0.0f; // temporary value
//      for (k=0; k<P; k++) {
//        // dot product of row from A and column from B
//        sum += A[i][k]*B[k][j];
//      }
//      C[i][j] = sum;
//    }
//  }
//}
 
main(int argc, char *argv[])  {
  MPI_Request reqs[2];
  MPI_Status stats[2];
  
  double **r;
  int i;
  int num_arg_matrices;
  int numtasks, rank, dest, source, rc, count, tag=1;  
  char inmsg, outmsg='x';
  MPI_Status Stat;

  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  int rank_x = rank/sqrt(numtasks);
  int rank_y = rank%(int)sqrt(numtasks);
  
  int debug_perf = atoi(argv[1]);
  int test_set = atoi(argv[2]);
  matrix_dimension_size = atoi(argv[3]);
  
//   num_arg_matrices = init_gen_sub_matrix(test_set);
  num_arg_matrices = 2;
  //--- allocating the space for matrix  
  r = (double **)my_malloc(sizeof(double *) * num_arg_matrices);
  int resultColumnSize =  matrix_dimension_size;
  int resultRowSize =  matrix_dimension_size/numtasks;
  int rowLow = rank*matrix_dimension_size/numtasks;
  int rowHigh = rowLow + matrix_dimension_size/numtasks - 1;
  int colLow = 0;
  int colHigh = matrix_dimension_size - 1;
  double* result = (double *)my_malloc(sizeof(double) * resultRowSize*resultColumnSize);
  
  // ---- generating two sub_matrices
  for (i = 0; i < 2; ++i) {
      r[i] = (double *)my_malloc(sizeof(double) * resultRowSize* resultColumnSize);
      if (i % 2 == 0) { //generate rows
          if (gen_sub_matrix(0, test_set, i, r[i], colLow, colHigh, 1, rowLow, rowHigh, 1, 1) == NULL) {
              printf("inconsistency in gen_sub_matrix\n");
              exit(1);
          }

      }else{ //generate columns
          if (gen_sub_matrix(0, test_set, i, r[i], rowLow, rowHigh, 1, colLow, colHigh, 1, 1) == NULL) {
              printf("inconsistency in gen_sub_matrix\n");
              exit(1);
          }


      }
  } 
 
  // --- print what you generated
  int printGenerated = 0;
  if (rank != 0) { 
      MPI_Recv(&printGenerated,1,MPI_INT, (rank - 1), tag,MPI_COMM_WORLD, stats); 
  } 
  print_matrix(r[0], resultRowSize, resultColumnSize);
  print_matrix(r[1], resultRowSize, resultColumnSize);
  if (rank != numtasks - 1) { 
      MPI_Send(&printGenerated,1,MPI_INT, (rank + 1), tag,MPI_COMM_WORLD); 
  }

  // ---- calculate the partial sum (multiplication stage)
  double sum;
  int   j;
  int   k;
  int m = matrix_dimension_size/numtasks; 
  int n = matrix_dimension_size; 
  int p =  m;
  double *a = r[0];
  double *b = r[1];
  double *rcvBuffer = (double *) my_malloc(sizeof(double) * m * n);
  tag = 0; //possible change later 
  int MatrixDone = 0;
  while(!MatrixDone) { 
      //--- calculatee the matrix 
      for (i=0; i<m; i++) {
          // for each row of c
          for (j=0; j<p; j++) {
              // for each column of c
              sum = 0.0f; // temporary value
              for (k=0; k<n; k++) {
                  // dot product of row from a and column from b
                  sum += a[i*n+ k]*b[k*p+j];
              }

              result[i*n + (j+((tag+rank)%numtasks)*m)] = sum;
          }

      }


      MPI_Isend(b,m*n,MPI_DOUBLE, (rank + numtasks - 1)%numtasks,0,MPI_COMM_WORLD,&reqs[0]);
      MPI_Irecv(rcvBuffer,m*n,MPI_DOUBLE, (rank+1)%numtasks,tag,MPI_COMM_WORLD, &reqs[1]); 
      MPI_Waitall(2, reqs, stats);
      tag++;
      b = rcvBuffer;
      if(tag>=numtasks)
          MatrixDone = 1;
  } 
 


  /// print the result 
  int printed = 0;
  if (rank != 0) { 
      MPI_Recv(&printed,1,MPI_INT, (rank - 1), tag,MPI_COMM_WORLD, stats); 
  } 
  print_matrix(result, resultRowSize, resultColumnSize);

  if (rank != numtasks - 1) { 
      MPI_Send(&printed,1,MPI_INT, (rank + 1), tag,MPI_COMM_WORLD); 
  } 


  // rc = MPI_Get_count(&Stat, MPI_CHAR, &count);
  //printf("Task %d: Received %d char(s) from task %d with tag %d \n",
  //       rank, count, Stat.MPI_SOURCE, Stat.MPI_TAG);
  //

  MPI_Finalize();
}
