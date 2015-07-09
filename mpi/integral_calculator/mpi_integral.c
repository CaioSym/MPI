/**
 *	porgram: MPI_Integral
 *	description: Calculates integral of function f from point a to point b
 * 				using n trapezoids
 *	author: Caio F. Sym
 *	2015-04-16
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <time.h>

//the function f(x)
double f(double x);

//method for getting inputs a, b and n
void get_input(int my_rank, int comm_sz, double* a_p, double* b_p, int* n_p);

/****************************************************************************************************
 *	main
 ****************************************************************************************************/
int main(int argc, char **argv) {
	//time stamps
	struct timeval start	;	//timer initial time
	struct timeval end	;	//timer end time

	//MPI variables
	int comm_sz			;	//num of processes
	int my_rank			;	//process id

	//globals
	double global_a	;	//initial value of x
	double global_b	;	//final value of x
	int	 global_n	;	//total trapezoids
	double global_h	;	//unit step for trapezoid
	double global_sum	;	//final value of integral
	
	//locals
	double local_a		;	//initial value of x for process
	double local_b		;	//final value of x for process
	int	 local_n		;	//number of trapezoids for process
	double local_sum	;	//value of integral for process
	
	//aux
	int i					;	// counter
	
	//begin
	MPI_Init(&argc, &argv);
	
	//get process info
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	//get inputs
	get_input(my_rank, comm_sz, &global_a, &global_b, &global_n);

	//if process is master start timer;
	if(my_rank == 0) {
		gettimeofday(&start, NULL);
	}
	
	//make global data
	global_h = (global_b-global_a)/global_n;
	
	//make local data
	local_n = global_n/comm_sz;
	local_a = global_a + my_rank * local_n * global_h;
	
	//if this is the last process it must treat all reamining
	//trapezoids so add modulus
	if(my_rank == comm_sz-1) {
		local_n += global_n%comm_sz;
	}
	local_b = local_a + local_n * global_h;
	
	//perform local sum calculation
	local_sum = (f(local_a) + f(local_b))/2.0;
	for(i=1;i < local_n;i++) {
		local_sum += f(local_a + i*global_h);
	}

	//reduce all local sums by adding them on global_sum in process 0
	MPI_Reduce(&local_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	
	//if process is master...
	if(my_rank == 0) {
		//perform final calcularion
		global_sum *= global_h;
		
		//stop timer
		gettimeofday(&end, NULL);
		
		//print results
		printf("%lf\n", global_sum);
		printf("%lu\n", ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)));
	}

	//end
	MPI_Finalize();
	return 0;
}

/****************************************************************************************************
 * f
 * funcion whose integral we want calculated.
 * replace with desired function
 * should be external reference or passed as pointer but who is looking...
 ****************************************************************************************************/
double f(double x) {
	double y;

	y = exp(x);

	return y;
}

/****************************************************************************************************
 * get_input
 * gets inputs with process zero and shares them with other processes
 ****************************************************************************************************/
void get_input(int my_rank, int comm_sz, double* a_p, double* b_p, int* n_p) {
	int i;
	//if process controls stdin read data and send it to others
	if(my_rank == 0) {
		scanf("%lf %lf %i",a_p, b_p, n_p);
		for(i = 0; i<comm_sz; i++) {
			MPI_Send(a_p, 1, MPI_DOUBLE	, i, 0, MPI_COMM_WORLD);
			MPI_Send(b_p, 1, MPI_DOUBLE	, i, 0, MPI_COMM_WORLD);
			MPI_Send(n_p, 1, MPI_INT	, i, 0, MPI_COMM_WORLD);
		}
	}
	//else process is slave and must receive the data;
	else {
		MPI_Recv(a_p, 1, MPI_DOUBLE	, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(b_p, 1, MPI_DOUBLE	, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(n_p, 1, MPI_INT	, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}
}
