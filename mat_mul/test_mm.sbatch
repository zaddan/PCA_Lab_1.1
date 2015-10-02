#!/bin/bash
#SBATCH -J tet_mm           # job name
#SBATCH -o test_mm.batchOut%j       # output and error file name (%j expands to jobID)
#SBATCH -n 10              # numOfCores total number of mpi tasks requested
#SBATCH -p normal-mic     # queue (partition) -- normal, development, etc.
#SBATCH -t 01:30:00        # run(hh:mm:ss) - 1.5 hours
#SBATCH --mail-user=behzadboro@utexas.edu
#SBATCH --mail-type=begin  # email me when the job starts
#SBATCH --mail-type=end    # email me when the job finishes

time ./test_mm.o 0 1 1000
