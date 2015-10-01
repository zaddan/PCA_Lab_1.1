# 	test_mm.c gen_matrix.c my_malloc.c gen_matrix.h my_malloc.h
test_mm: *.c
	mpicc simple_mm.c gen_matrix.c my_malloc.c -o simple_mm
	mpicc test_mm.c gen_matrix.c my_malloc.c -o test_mm
#  	gcc -g -DDEBUG test_mm.c gen_matrix.c my_malloc.c -o test_mm

run_debug:
	mpirun --mca orte_base_help_aggregate 0 -np 4 simple_mm 0 0 6
	echo "----------------------------------------------"
	./test_mm 0 0 6
# 	mpi_run  0 0 10

run_performance:
	./test_mm 1 0 100

clean:*.o
	rm *.o
