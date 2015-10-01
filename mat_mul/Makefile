# 	test_mm.c gen_matrix.c my_malloc.c gen_matrix.h my_malloc.h
debugPerg = 0
testNumber = 0 
matrixDim = 10000
numberOfCores = 20 
test_mm: *.c
	mpicc mat_mul.c gen_matrix.c my_malloc.c -o mat_mul.o
	gcc -g -DDEBUG test_mm.c gen_matrix.c my_malloc.c -o test_mm.o

run_seq:
	time ./test_mm.o $(debugPerg) $(testNumber) $(matrixDim)
#	echo "----------------------------------------------"
	#mpirun mat_mul 0 0 6
#
run_debug:
	time mpirun -np $(numberOfCores) mat_mul.o $(debugPerg) $(testNumber) $(matrixDim)
#time mpirun --mca orte_base_help_aggregate 0 -np $(numberOfCores) mat_mul $(debugPerg) $(testNumber) $(matrixDim)
#	echo "----------------------------------------------"
	#mpirun mat_mul 0 0 6
# 	mpi_run  0 0 10


run_performance:
	./test_mm 1 0 100

clean:*.o
	rm *.o
