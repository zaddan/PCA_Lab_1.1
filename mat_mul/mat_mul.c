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
  //printf("\n");
}

void print_matrix2(double *result, int numOfRows, int numOfCols) {
  int x, y;
  for (y = 0; y < numOfRows; ++y) {
    for (x = 0; x < numOfCols; ++x) {
      printf("%f ", result[y * numOfCols+ x]);
    }
  }
  //printf("\n");
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
  //printf("here we go%d\n", numtasks); 
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   
  int rank_x = rank/sqrt(numtasks);
  int rank_y = rank%(int)sqrt(numtasks);
   
  int debug_perf = atoi(argv[1]);
  int test_set = atoi(argv[2]);
  matrix_dimension_size = atoi(argv[3]);
  
  /*printf("matrix_dimension_size %d\n", matrix_dimension_size);*/
  /*printf("numtasks%d\n", numtasks);*/
          
   num_arg_matrices = init_gen_sub_matrix(test_set);
  //--- allocating the space for matrix  
  r = (double **)my_malloc(sizeof(double *) * num_arg_matrices);
  int resultColumnSize =  matrix_dimension_size;
  int resultRowSize =  matrix_dimension_size/numtasks;
  int rowLow = rank*matrix_dimension_size/numtasks;
  int rowHigh = rowLow + matrix_dimension_size/numtasks - 1;
  int colLow = 0;
  int colHigh = matrix_dimension_size - 1;
  double* result = (double *)my_malloc(sizeof(double) * resultRowSize*resultColumnSize);
  
  double *matrixToPrint = (double *)my_malloc(sizeof(double) * resultRowSize* resultColumnSize);
  // ---- generating two sub_matrices
  for (i = 0; i < num_arg_matrices; ++i) {
      r[i] = (double *)my_malloc(sizeof(double) * resultRowSize* resultColumnSize);
      if (i == 0) { //generate rows
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
  if (debug_perf == 0) { //the followign block is only for printing the argument matrices
      int printed = 0;
      for (i = 0; i < num_arg_matrices; ++i) {
          if (i == 0) { //generate rows
              if (rank != 0) { 
                  MPI_Send(r[i],matrix_dimension_size*matrix_dimension_size/numtasks,MPI_DOUBLE, 0, rank,MPI_COMM_WORLD); 
              } 
              else{
                  if (i !=0) {
                      printf("\n");
                  }
                  printf("argument matrix %d\n", i);
                  print_matrix(r[0], matrix_dimension_size/numtasks, matrix_dimension_size);
                  int k; 
                  for (k =1; k<numtasks; k++){
                      MPI_Recv(matrixToPrint,matrix_dimension_size*matrix_dimension_size/numtasks,MPI_DOUBLE, k, k,MPI_COMM_WORLD, stats); 
                      print_matrix(matrixToPrint, matrix_dimension_size/numtasks, matrix_dimension_size);
                  }
                  //printf("\n"); 
              }
          }else{
              int j; 
              printed = 0;
              for (j =0; j < matrix_dimension_size; j++) {
                  if (rank != 0) { 
                      MPI_Send(r[i]+j*matrix_dimension_size/numtasks, matrix_dimension_size/numtasks,MPI_DOUBLE, 0, rank, MPI_COMM_WORLD); 
                  } else{
                      if (j ==0) {
                          if (i !=1) {
                            printf("\n"); 
                          } 
                          printf("\n"); 
                          fflush(stdout); 
                          printf("argument matrix %d\n", i);
                      
                      }
                      if (j!=0) { 
                          printf("\n"); 
                      } 
                      int k =0; 
                      print_matrix2(r[i]+j*matrix_dimension_size/numtasks, 1, matrix_dimension_size/numtasks);
                      for (k =1; k<numtasks; k++){
                          MPI_Recv(matrixToPrint,matrix_dimension_size/numtasks,MPI_DOUBLE, k, k,MPI_COMM_WORLD, stats); 
                          print_matrix2(matrixToPrint, matrix_dimension_size/numtasks, 1);
                          
                      }
                  }

              }
          } 
  
      }
   fflush(stdout); 
  } 
  // --- print what you generated
  int printGenerated = 0;
  int printFull = 0;
  /*
  if (rank != 0) { 
      MPI_Recv(&printGenerated,1,MPI_INT, (rank - 1), tag,MPI_COMM_WORLD, stats); 
  } 
//  print_matrix(r[0], resultRowSize, resultColumnSize);
//  print_matrix(r[1], resultRowSize, resultColumnSize);
  if (rank != numtasks - 1) { 
      MPI_Send(&printGenerated,1,MPI_INT, (rank + 1), tag,MPI_COMM_WORLD); 
  }*/
  //printf("*************************************\n");
  //printf("done with printing the peices\n");
  
  //---------guide:::  recieve all and calculate for debugging
//  printf("sending the pieces to rank 0 for debugging purpuses\n");
//  if (rank != 0) { 
//      MPI_Send(r[0],matrix_dimension_size*matrix_dimension_size/numtasks,MPI_DOUBLE, rank, rank,MPI_COMM_WORLD); 
//  } 
//  int counter; 
//  if (rank == 0) { 
//      double *fullMatrix= (double *)my_malloc(sizeof(double) * matrix_dimension_size* matrix_dimension_size);
//      double *subMatrixMultiplicand = (double *)my_malloc(sizeof(double) * matrix_dimension_size* matrix_dimension_size/numtasks);
//      
//      int counter; 
//      for(counter = 1; counter< numtasks; counter++) {
//          MPI_Recv(subMatrixMultiplicand,matrix_dimension_size*matrix_dimension_size/numtasks,MPI_DOUBLE,counter, counter,MPI_COMM_WORLD, stats); 
//          printf("od\n"); 
//          //copy to the result 
//          int counter2;
//          for(counter2 = 0; counter2< matrix_dimension_size*matrix_dimension_size/numtasks; counter2++) {
//              fullMatrix[counter2*tag*numtasks*matrix_dimension_size] = subMatrixMultiplicand[counter2];
//          }
//
//      }
//  
//    print_matrix(fullMatrix, matrix_dimension_size, matrix_dimension_size);
//    printf("*****************printing the full matrix\n"); 
//     
//    for(counter = 1; counter< numtasks; counter++) {
//        MPI_Send(&printFull,1,MPI_INT, counter, 0,MPI_COMM_WORLD); 
//    } 
//     
//  } 
//  
//  if (rank != 0) { 
//    MPI_Recv(&printFull,1,MPI_INT,0, 0,MPI_COMM_WORLD, stats); 
//  } 
//  printf("debugging done\n"); 
//  printf("*****************Done\n"); 
//  
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
  while(1) {
	  tag =0; 
      while(1) { 
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
		  tag++;
		  if(tag>=numtasks){
              break; 
		  }else {

			  //           printf("tage is %d\n", tag); 
		  }

		  MPI_Isend(b,m*n,MPI_DOUBLE, (rank + numtasks - 1)%numtasks,0,MPI_COMM_WORLD,&reqs[0]);
		  MPI_Irecv(rcvBuffer,m*n,MPI_DOUBLE, (rank+1)%numtasks,0,MPI_COMM_WORLD, &reqs[1]); 
		  MPI_Waitall(2, reqs, stats);
		  double *temp = b; 
          b = rcvBuffer;
	      rcvBuffer = temp; 
      }
      MatrixDone++;
	  if (MatrixDone == (num_arg_matrices -1)) {
          break;
      }
      
      a = result;
	  result = r[MatrixDone - 1];
      b = r[MatrixDone+1];
  }
 

  if (debug_perf == 0) {
      int printed = 0;
      if (rank != 0) { 
          MPI_Send(result,matrix_dimension_size*matrix_dimension_size/numtasks,MPI_DOUBLE, 0, rank,MPI_COMM_WORLD); 
      } 
      else{
          printf("\n"); 
          printf("\n"); 

//          if (i !=0) {
//              printf("\n");
//          }
          printf("result matrix\n");
          print_matrix(result, matrix_dimension_size/numtasks, matrix_dimension_size);
          int k; 
          for (k =1; k<numtasks; k++){
              MPI_Recv(matrixToPrint,matrix_dimension_size*matrix_dimension_size/numtasks,MPI_DOUBLE, k, k,MPI_COMM_WORLD, stats); 
              print_matrix(matrixToPrint, matrix_dimension_size/numtasks, matrix_dimension_size);
          }
          printf("\n"); 
      }

      
//      
//      
//      
//      if (rank != 0) { 
//          MPI_Recv(&printed,1,MPI_INT, (rank - 1), tag,MPI_COMM_WORLD, stats); 
//      }else{
//          printf("result matrix\n");
//      }
//      print_matrix(result, resultRowSize, resultColumnSize);
//
//      if (rank != numtasks - 1) { 
//          MPI_Send(&printed,1,MPI_INT, (rank + 1), tag,MPI_COMM_WORLD); 
//      } 
//      if (rank == numtasks - 1) { 
//          printf("\n"); 
//      }
      MPI_Barrier(MPI_COMM_WORLD); 
  }     
  else { 
      ;
      double *finalSum = (double*)my_malloc(sizeof(double) * 1);
      *finalSum = 0;
      for (count =0 ;count < matrix_dimension_size*matrix_dimension_size/numtasks; count++){
          *finalSum +=result[count];
      }

      MPI_Barrier(MPI_COMM_WORLD); 
      double *totalFinalSum = (double*)my_malloc(sizeof(double) * 1);
      *totalFinalSum = 0; 
      //printf("here %f\n rank: %d", *finalSum, rank) ;
      MPI_Reduce(finalSum, totalFinalSum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
      MPI_Barrier(MPI_COMM_WORLD); 
      if(rank ==0){
          printf("%f\n", *totalFinalSum);
      }
  }
  MPI_Finalize();
}
