#include <stdlib.h>
#include <stdio.h>
#include "setting_loader.h"
#include "main.h"

#define BUFFSIZE 1024

double CURRENT_FACTOR;
double CHARGE_FACTOR;
unsigned int HARMONICS;
double T1;
double T2;
unsigned int AVERAGE_FLAG;
unsigned int N1;
unsigned int N2;

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
		fprintf(stderr, "Can't open %s\n", settingsfile);
		exit(1);
	}
	/* CURRENT_FACTOR */
	fgets(buff, BUFFSIZE, in);
	CURRENT_FACTOR = atof(buff);

	/* CHARGE_FACTOR */
	fgets(buff, BUFFSIZE, in);
	CHARGE_FACTOR = atof(buff);
	
	/* HARMONICS */
	fgets(buff, BUFFSIZE, in);
	HARMONICS = atoi(buff);

	/* */
	fgets(buff, BUFFSIZE, in);
	T1 = atof(buff);
	fgets(buff, BUFFSIZE, in);
	T2 = atof(buff);
	fgets(buff, BUFFSIZE, in);
	AVERAGE_FLAG = atoi(buff);

	fclose(in);
	N1 = T1*SAMPLE_FREQ/1000;
	N2 = T2*SAMPLE_FREQ/1000;
	//show_settings();
}
