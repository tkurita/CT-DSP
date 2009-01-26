#include "average_controller.h"
#include <stdlib.h>
#include <stdio.h>

static double **avg_buffer = NULL;
static int *buffer_ind = NULL;
static int N_AVG = 0;
static int N_CYCLE = 0;

int avg_initialize(short n_avg, int n_cycle) {
	int n = 0;
	int m = 0;
	if (avg_buffer != NULL) {
		for (n=0; n < N_CYCLE; n++) {
			free(avg_buffer[n]);
		}
		free(avg_buffer);
		free(buffer_ind);	
	}
	avg_buffer = (double **)malloc(n_avg*sizeof(double*));
	for (n = 0; n < n_avg; n++ ){
		avg_buffer[n] = (double *)malloc(n_cycle*sizeof(double));
		for (m = 0; m<n_cycle; m++) {
			avg_buffer[n][m] = 0;
		}
	}

	buffer_ind = (int *)malloc(sizeof(int)*n_avg);
	for (n = 0; n < n_avg; n++) {
		buffer_ind[n] = 0;
	}

	N_AVG = n_avg;
	N_CYCLE = n_cycle;
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
	double sum;
	for (n=0; n < buffer_ind[index]; n++) {
		sum = sum + avg_buffer[index][n];
	}
	return sum/buffer_ind[index]; 
}

void avg_output() {
	int n;
	FILE *out;
	out = fopen("dspout.txt", "w");
	for (n=0; n < N_AVG; n++) {
		fprintf(out, "%f\n", avg_avgget(n));	
	}
	fclose(out);
}
