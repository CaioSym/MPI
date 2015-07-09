#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

//Global
int nt; //Number of Threads

/* Count sort */
double count_sort(double a[], int n) {
	double *temp				;	//temporary array for storing sorted results
	double start, end, duracao	;	//time stamps
	int i, j, count				;	//counters and auxiliaries

	//mem allocation
	temp = (double *)malloc(n*sizeof(double));

	//start timer
	start = omp_get_wtime();

	//split count sort in nt chunks
	# pragma omp parallel for num_threads(nt) \
	default(none) private(i, j, count) shared(n, temp, a)
	//for each element...
	for (i = 0; i < n; i++) {
		count = 0;
		//...test against every other element and increase count
		//for larger and equal elements
		for (j = 0; j < n; j++)
			if (a[j] < a[i])
				count++;
			else if (a[j] == a[i] && j < i)
				count++;
		//use count value to know position in sorted array
		temp[count] = a[i];
	}
	//end timer
	end = omp_get_wtime();

	//compute time diference
	duracao = end - start;
	
	//replace array with ordered array
	memcpy(a, temp, n*sizeof(double));
	free(temp);

	return duracao;
}

int main(int argc, char * argv[]) {
	int n		;	//number of values
	double  *a	;	//array of values
	double	t_s	;	//program runtime
	int i		;	//counter

	//read thread count
	scanf("%d",&nt);
	
	//read number of values
	scanf("%d",&n);

	//mem allocation
	a = (double *)malloc(n*sizeof(double));

	//read values
	for(i=0;i<n;i++)
		scanf("%lf",&a[i]);
	
	//execute program
	t_s = count_sort(a,n);
	
	//print ordered values
	for(i=0;i<n;i++)
		printf("%.2lf ",a[i]);

	printf("\n");

	//print program runtime
	printf("%lf\n",t_s);

	return 0;
}
