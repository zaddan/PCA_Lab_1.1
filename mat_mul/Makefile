# 	test_mm.c gen_matrix.c my_malloc.c gen_matrix.h my_malloc.h
debugPerg = 0
testNumber = 0
matrixDim = 10
numberOfCores = 16
test_mm: *.c
	icc -O3 mat_mul_cilk.c gen_matrix.c my_malloc.c -o mat_mul_cilk.out
	mpicc -O3 mat_mul.c gen_matrix.c my_malloc.c -o mat_mul.out
	gcc -g -DDEBUG test_mm.c gen_matrix.c my_malloc.c -O3 -o test_mm.out

run_seq:
	time ./test_mm.out $(debugPerg) $(testNumber) $(matrixDim)
#	echo "----------------------------------------------"
	#mpirun mat_mul 0 0 6
#
run_debug:
	time mpirun -np $(numberOfCores) mat_mul.out $(debugPerg) $(testNumber) $(matrixDim)
#time mpirun --mca orte_base_help_aggregate 0 -np $(numberOfCores) mat_mul $(debugPerg) $(testNumber) $(matrixDim)
#	echo "----------------------------------------------"
	#mpirun mat_mul 0 0 6
# 	mpi_run  0 0 10
run_cilk:
	time ./mat_mul_cilk.out $(debugPerg) $(testNumber) $(matrixDim)

run_performance:
	./test_mm.out 1 0 100

clean:*.out
	rm *.out
