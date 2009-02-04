#include "average_controller.h"
#include <stdlib.h>
#include <stdio.h>

#define useLog 0

static double **avg_buffer = NULL;
static int *buffer_ind = NULL;
static int N_AVG = 0;
static int N_CYCLE = 0;
static char *avg_comments[7] = {"current at timming 1",
								"charge at timming 1",
								"particles at timming 1",
								"current at timming 2",
								"charge at timming 2",
								"particles at timming 2",
								"BS Counts"};

static char *outformat[7] = {"%f\t# %s\n",
							 "%f\t# %s\n",
							 "%f\t# %s\n",
							 "%f\t# %s\n",
							 "%f\t# %s\n",
							 "%f\t# %s\n",
							 "%f\t# %s\n"};

void alloc_buffers(short n_avg, int n_cycle) {
	unsigned short n;
	avg_buffer = (double **)malloc(n_avg*sizeof(double*));
	if (avg_buffer == NULL) {
		fprintf(stderr, "Fail to malloc avg_buffer\n");
		exit(-1);
	}

	for (n = 0; n < n_avg; n++ ){
		avg_buffer[n] = (double *)malloc(n_cycle*sizeof(double));
		if (avg_buffer[n] == NULL) {
			fprintf(stderr, "Fail to malloc avg_buffer[%d]\n",n);
			exit(-1);
		}
	}

	buffer_ind = (int *)malloc(sizeof(int)*n_avg);
	if (buffer_ind == NULL) {
		fprintf(stderr, "Fail to malloc buffer_ind\n");
		exit(-1);
	}
}

void init_buffers() {
	short m,n;
	for (n = 0; n < N_AVG; n++ ){
		for (m = 0; m < N_CYCLE; m++) {
			avg_buffer[n][m] = 0;
		}
	}

	for (n = 0; n < N_AVG; n++) {
		buffer_ind[n] = 0;
	}
}

int avg_initialize(short n_avg, int n_cycle) {
	unsigned short n = 0;
	if (avg_buffer != NULL) {
		if ((n_avg !=  N_AVG)||(n_cycle != N_CYCLE)) {
			for (n=0; n < N_CYCLE; n++) {
				free(avg_buffer[n]);
			}
			free(avg_buffer);
			free(buffer_ind);
			alloc_buffers(n_avg, n_cycle);
		}
	} else {
		alloc_buffers(n_avg, n_cycle);
	}

	N_AVG = n_avg;
	N_CYCLE = n_cycle;
	init_buffers();
	return(1);
}

enum addresult avg_add(short index, double val) {
	if (buffer_ind[index] < N_CYCLE) {
		avg_buffer[index][buffer_ind[index]] = val;
		buffer_ind[index]++;
	}
	
	if (buffer_ind[index] < N_CYCLE) {
		return notFull;
	} else {
		return full;
	}
}

double avg_avgget(short index){
	int n;
	double sum = 0;
#if useLog
	printf("buffer_ind[%d] : %d\n", index, buffer_ind[index]);
#endif
	for (n=0; n < buffer_ind[index]; n++) {
#if useLog
		printf("%f\n", avg_buffer[index][n]);
#endif
		sum = sum + avg_buffer[index][n];
	}
#if useLog
	printf("sum: %f\n", sum);
#endif
	return sum/buffer_ind[index]; 
}

void avg_output() {
	int n;
	FILE *out;
	double avg_result;
	out = fopen("dspout.txt", "w");
	for (n=0; n < N_AVG; n++) {
		avg_result = avg_avgget(n);
#if useLog
		printf("%s\n", avg_comments[n]);
		printf("%f\n", avg_result);
#endif
		fprintf(out, outformat[n], avg_result, avg_comments[n]);	
	}
	fclose(out);
}
