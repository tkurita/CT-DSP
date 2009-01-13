#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "main.h"
#include "DO_controller.h"
#include "
/* define globals*/
#define MAX_CYCLE 16

static far volatile int StoredData[MAX_N_AD]; //magnetic field signal. subtracted from current signal 

static volatile int CurrentCycle;
extern volatile int N_AD;
 
void read_cancel_data()
{
	FILE *fp;
	int i,x;
	
	/* load stored data */
	for (i=0; i<MAX_N_AD; i++) {
		StoredData[i] = 0;
	}
	
#if USE_FIELD_CANCEL
#if FILE_ACCESS
	//if ((fp = fopen("..\\trial1\\Debug\\storedwave.txt", "r")) == NULL) {
	if ((fp = fopen("storedwave.txt", "r")) == NULL) {
		puts("can't open file");
		exit(-1);
	}
	else {
		puts("success to open file");
		//fclose(fp);
		//exit(-1);
	} 
		
	for (i=0; i<MAX_N_AD; i++) {
		fscanf(fp, "%d %d", &x, &StoredData[i]);
		//printf("%d %d\n",x, data[i]);
	}
	fclose(fp);
	puts("end of reading storedwave");
#endif
#endif

}

void output_stored_data()
{
	int i;
	int first_val, last_val;
	double slope;

#if FILE_ACCESS	 
	FILE *fp;
	if ((fp = fopen("storedwave.txt", "w")) == NULL) {
		puts("can't open file");
		exit(1);
	}
#endif

	
	first_val = StoredData[OFFSET_RESET_DELAY];
	last_val = StoredData[MAX_N_AD-1];
	slope = (double)(last_val-first_val)/((double)MAX_N_AD);
	for (i=0; i<MAX_N_AD; i++) {
		//fprintf(fp, "%d %d %d\n", i, data[i]/N_CYCLE, (int)((data[i]- slope*i - first_val)/N_CYCLE));
		StoredData[i] = (int)((StoredData[i]- slope*i - first_val)/MAX_CYCLE);
#if FILE_ACCESS
		fprintf(fp, "%d %d\n", i, StoredData[i]);
#endif
	}

#if FILE_ACCESS	
	fclose(fp);
	printf("end of output\n");
#endif

}

void clear_stored_data()
{
	int n;
	for (n=0; n<MAX_N_AD;n++) {
		StoredData[n] = 0;
	}
}

int triggered_to_store()
{
	int is_end_store;
	CurrentCycle ++;
	N_AD = 0;
	is_end_store = (CurrentCycle > MAX_CYCLE);
	if (is_end_store) {
		output_stored_data();
		DO_off_for_ch(1);
	}
	
	return is_end_store;
}

void ready_to_store()
{
	//puts("ready to store");
	CurrentCycle = 0;
	DO_on_for_ch(1);
	triggered_to_store();
}

interrupt void c_int_ad_done_to_store() 
{
	int x;
	update_trigger_led();
	if (N_AD >= MAX_N_AD) {
		return;
	}
	x = sbox_AdGet(CURR_CH);
	StoredData[N_AD]+=x;
	sbox_DaPut(CURR_DA_CH, StoredData[N_AD]/CurrentCycle);
	N_AD++;
}

int stored_data_at(int index)
{
	if (index < MAX_N_AD) {
		return (StoredData[index]);
	}
	return 0;
}
