build: ms_seq.c ms_pthread.c ms_omp.c ms_mpi.c
	gcc ms_seq.c helpers.c -o ms_seq -lm -Wall -Wextra
	gcc ms_omp.c helpers.c -o ms_omp -lm -fopenmp -Wall -Wextra
	gcc ms_pthread.c helpers.c -o ms_pthread -lm -lpthread -Wall -Wextra
	mpicc ms_mpi.c helpers.c -o ms_mpi -lm -Wall -Wextra -lpthread
	mpicc ms_mpi_openmp.c helpers.c -o ms_mpi_openmp -lm -fopenmp -Wall -Wextra -lpthread
	mpicc ms_mpi_pthreads.c helpers.c -o ms_mpi_pthreads -lm -fopenmp -Wall -Wextra -lpthread
clean:
	rm -rf ms_seq ms_pthread ms_omp
	rm -rf ms_seq ms_pthread ms_seq
	rm -rf ms_seq ms_pthread ms_pthread
	rm -rf ms_seq ms_pthread ms_mpi
	rm -rf ms_seq ms_pthread ms_mpi_openmp ms_mpi_pthreads