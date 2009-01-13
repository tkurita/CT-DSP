#include <stdlib.h>
#include <stdio.h>
#include "setting_loader.h"

#define BUFFSIZE 1024

float SAMPLE_FREQ;  //[Hz] sampling frequency of ADDA
float WORKTIME;
unsigned int MAX_N_AD;
double CURRENT_FACTOR;
double CHARGE_FACTOR;
unsigned int HARMONICS;
double T1;
double T2;
unsigned int AVERAGE_FLAG;


void read_data(FILE* in, char *buff)
{
	int n;
	fgets(buff, BUFFSIZE, in);
	for (n=0; n<BUFFSIZE; n++) {
		if (*(buff+n) == '#') {
			*(buff+n) = '\0';
			break;
		}
	}
}

void show_settings()
{
	printf("CURRENT_FACTOR : %f\n", CURRENT_FACTOR);
	printf("CHARGE_FACTOR : %f\n", CHARGE_FACTOR);
	printf("HARMONICS : %d\n", HARMONICS);
	printf("Timming 1 : %f\n", T1);
	printf("Timming 2 : %f\n", T2);
	printf("AVERAGE_FLAG : %d\n", AVERAGE_FLAG);
}

void load_settings()
{
	char buff[BUFFSIZE];
	FILE *in;
	in = fopen(settingsfile, "r");
	if (!in) {
		fprintf(stderr, "Can't open %s", settingsfile);
		exit(1);
	}
	//read_data(in, buff);
	fgets(buff, BUFFSIZE, in);
	SAMPLE_FREQ = atof(buff);
	fgets(buff, BUFFSIZE, in);
	WORKTIME = atof();
	MAX_N_AD = SAMPLE_FREQ*WORKTIME;
	fgets(buff, BUFFSIZE, in);
	CURRENT_FACTOR = atof(buff);
	fgets(buff, BUFFSIZE, in);
	CHARGE_FACTOR = atof(buff);
	fgets(buff, BUFFSIZE, in);
	HARMONICS = atoi(buff);
	fgets(buff, BUFFSIZE, in);
	T1 = atof(buff);
	fgets(buff, BUFFSIZE, in);
	T2 = atof(buff);
	fgets(buff, BUFFSIZE, in);
	AVERAGE_FLAG = atoi(buff);

	fclose(in);
	show_settings();
}
