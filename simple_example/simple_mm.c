#include "mpi.h"
#include <stdio.h>
#include "gen_matrix.h"
#include "my_malloc.h"
#include <math.h>
void print_matrix(double *result, int dim_size) {
  int x, y;
  for (y = 0; y < dim_size; ++y) {
    for (x = 0; x < dim_size; ++x) {
      printf("%f ", result[y * dim_size + x]);
    }
    printf("\n");
  }
  printf("\n");
}

main(int argc, char *argv[])  {

  double **r;
  double **result;
  int i;
  int num_arg_matrices;
  int numtasks, rank, dest, source, rc, count, tag=1;  
  char inmsg, outmsg='x';
  MPI_Status Stat;

  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);


  int debug_perf = atoi(argv[1]);
  int test_set = atoi(argv[2]);
  matrix_dimension_size = atoi(argv[3]);
  
  num_arg_matrices = init_gen_sub_matrix(test_set);
  //--- allocating the space for matrix  
  r = (double **)my_malloc(sizeof(double *) * num_arg_matrices);
  int columnSize =  matrix_dimension_size/sqrt(numtasks);
  int rowSize =  matrix_dimension_size/sqrt(numtasks);
  int columnLow = (int)(rank/sqrt(numtasks)) * rowSize;
  int columnHigh = columnLow + rowSize - 1;
  int rowLow = (rank%(int)sqrt(numtasks))*columnSize;
  int rowHigh = rowLow + columnSize - 1 ;
//  printf("xhigh %d\n", columnHigh);
//  printf("xlow%d\n", columnLow);
//  printf("yhigh %d\n", rowHigh);
//  printf("ylow%d\n", rowLow);

  for (i = 0; i < num_arg_matrices; ++i) {
      r[i] = (double *)my_malloc(sizeof(double) * rowSize * columnSize);
      if (gen_sub_matrix(0, test_set, i, r[i], rowLow, rowHigh, 1, columnLow, columnHigh, 1, 1) == NULL) {
          printf("inconsistency in gen_sub_matrix\n");
          exit(1);
      }
  }
  
  printf("initial values start************************\n"); 
  printf("randk#%d\n", rank); 
  print_matrix(r[0], rowSize);
  printf("initial values done************************\n"); 
   
//    dest = 1;
//  source = 1;
  tag = 1; 
  int numElement = columnSize*rowSize ;
  if (rank%2 == 1) {
      rc = MPI_Send(r[0], numElement, MPI_DOUBLE, rank - 1, tag, MPI_COMM_WORLD);
  }else {
      rc = MPI_Recv(r[0], numElement, MPI_DOUBLE, rank + 1, tag, MPI_COMM_WORLD, &Stat);
      printf("rcv start================\n");
      printf("randk#%d\n", rank); 
      print_matrix(r[0], rowSize);
      printf("rcv done================\n");
  }

  
  //  } 

//else if (rank == 1) {
//dest = 0;
//  source = 0;
//  rc = MPI_Recv(&inmsg, 1, MPI_CHAR, source, tag, MPI_COMM_WORLD, &Stat);
//  rc = MPI_Send(&outmsg, 1, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
//  }
//


rc = MPI_Get_count(&Stat, MPI_CHAR, &count);
//printf("Task %d: Received %d char(s) from task %d with tag %d \n",
//       rank, count, Stat.MPI_SOURCE, Stat.MPI_TAG);
//

MPI_Finalize();
}
