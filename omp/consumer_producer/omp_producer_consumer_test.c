#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

//globals
int nt;	//The number of threads to run with 

void producer_consumer(int *buffer, int size, int *vec, int n) {
	long long unsigned int sum = 0;	//reduction variable
	int i, j;						//counters

	//create treads for loops, reduce on sum and define scopes
	#pragma omp parallel num_threads(nt) \
	reduction(+: sum) default(none) private(i, j,) \
	shared(buffer, vec, n, size)
	for(i=0;i<n;i++) {
		if(i % 2 == 0) {
			//run production in parallel
			#pragma omp for
			for(j=0;j<size;j++) {
				buffer[j] = vec[i] + j*vec[i+1];
			}
		}
		else {
			//run consumption in parallel
			#pragma omp for
			for(j=0;j<size;j++) {
				sum += buffer[j];
			}
		}
	}

	//print reducion
	printf("%llu\n",sum);
}

int main(int argc, char * argv[]) {
	double start, end;	//Time Stamps
	int *vec , n;		//array for data and its size
	int *buff, size;	//temp buffer and its size
	int i;				//counter

	//scan initial parameters
	scanf("%d %d %d",&nt,&n,&size);
	
	//mem alloc
	buff = (int *)malloc(size*sizeof(int));
	vec = (int *)malloc(n*sizeof(int));

	//read data
	for(i=0;i<n;i++)
		scanf("%d",&vec[i]);
	//begin
	start = omp_get_wtime();
	producer_consumer(buff, size, vec, n);
	end = omp_get_wtime();
	//end

	//print time ellapsed;
	printf("%lf\n",end-start);

	free(buff);
	free(vec);

	return 0;
}
