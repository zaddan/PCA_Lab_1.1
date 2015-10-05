#!/bin/bash
#SBATCH -J mat_mul           # job name
#SBATCH -o mat_mul.batchOut%j       # output and error file name (%j expands to jobID)
#SBATCH -n 512# numOfCores total number of mpi tasks requested
#SBATCH -p normal-mic     # queue (partition) -- normal, development, etc.
#SBATCH -t 01:30:00        # run (hh:mm:ss) - 1.5 hours
#SBATCH --mail-user=behzadboro@utexas.edu
#SBATCH --mail-type=begin  # email me when the job starts
#SBATCH --mail-type=end    # email me when the job finishes

time ibrun ./test_mm.out 0 1 512
