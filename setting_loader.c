#include <stdlib.h>
#include <stdio.h>
#include "setting_loader.h"
#include "main.h"

#define BUFFSIZE 1024

double CURRENT_OUT_FACTOR = 1.0;
double PARTICLES_OUT_FACTOR = 6740.0;
double T1 = 32.0;
double T2 = 588.0;
unsigned int N1;
unsigned int N2;
double CURRENT_FACTOR = 0.008690;
double CHARGE_FACTOR = 45.175;
int CHARGE = 1;
unsigned int HARMONICS = 1;

extern int OFFSET_RESET_DELAY;

#define useLog 0

void load_primitive_settings()
{
#ifdef FLASHBOOT
#else
    char buff[BUFFSIZE];
    FILE *in;
    in = fopen(PRIMITIVE_SETTINGS_FILE, "r");
    if (!in) {
        goto bail;
    }
   
    while (fgets(buff, BUFFSIZE, in) ) {
        OFFSET_RESET_DELAY = atoi(buff);
        if (OFFSET_RESET_DELAY) break;
    }
 bail:
    printf("OFFSET_RESET_DELAY : %d [cycle]\n", OFFSET_RESET_DELAY);
#endif
}

void show_settings()
{
#ifdef FLASHBOOT
#else
	printf("CURRENT_OUT_FACTOR : %f\n", CURRENT_OUT_FACTOR);
	printf("PARTICLES_OUT_FACTOR : %f\n", PARTICLES_OUT_FACTOR);
	printf("Timming 1 : %f\n", T1);
	printf("Timming 2 : %f\n", T2);
	printf("CURRENT_FACTOR : %f\n", CURRENT_FACTOR);
	printf("CHARGE_FACTOR : %f\n", CHARGE_FACTOR);
	printf("CHARGE : %d\n", CHARGE);
	printf("HARMONICS : %d\n", HARMONICS);
#endif
}

void load_settings()
{
#ifdef FLASHBOOT
#else
	char buff[BUFFSIZE];
	FILE *in;
	puts("----- Reading settings... -----");
	in = fopen(settingsfile, "r");
	if (!in) {
		fprintf(stderr, "Can't open %s\n", settingsfile);
		exit(1);
	}
	/* CURRENT_OUT_FACTOR usually 1*/
	fgets(buff, BUFFSIZE, in);
	CURRENT_OUT_FACTOR = atof(buff);

	/* PARTICLES_OUT_FACTOR */
	fgets(buff, BUFFSIZE, in);
	PARTICLES_OUT_FACTOR = atof(buff);

	/* Timming 1*/
	fgets(buff, BUFFSIZE, in);
	T1 = atof(buff);
	
	/* Timming 2 */
	fgets(buff, BUFFSIZE, in);
	T2 = atof(buff);

	/* CURRENT_FACTOR */
	fgets(buff, BUFFSIZE, in);
	CURRENT_FACTOR = atof(buff);

	/* CHARGE_FACTOR */
	fgets(buff, BUFFSIZE, in);
	CHARGE_FACTOR = atof(buff);

	/* CHARGE */
	fgets(buff, BUFFSIZE, in);
	CHARGE = atoi(buff);

	/* HARMONICS */
	fgets(buff, BUFFSIZE, in);
	HARMONICS = atoi(buff);

	/* filename */
	fgets(buff, BUFFSIZE, in);
	printf("%s", buff);

	fclose(in);
	show_settings();
#endif
	N1 = T1*SAMPLE_FREQ/1000;
	N2 = T2*SAMPLE_FREQ/1000;

}
