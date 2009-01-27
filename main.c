#include "main.h"
//static far volatile int data[MAX_N_AD]; //magnetic field signal. subtracted from current signal
#include "leak_field.h"
#include "DO_controller.h"
#include "setting_loader.h"
#include "average_controller.h"

/*define global variables*/

//static volatile int Triggered; //set to 1 if triggered.
volatile int N_AD; //incremented with AD_DONE interruput. rest to 0 when OUT_TRIG intterupt
/*
static volatile int ShouldStoreData; //when button is pushed, this flag raise
static volatile int InStoreMode;
static volatile int ShouldClearData;
*/
static int NextTask;
static volatile enum avg_status AverageStatus;
static volatile enum store_status StoreStatus;

static volatile double CurrOffset; //offset, obtained when first ad_get after triggered

static far volatile int curr_in_buff[MAX_N_AD];
static far volatile int freq_in_buff[MAX_N_AD];
 
static volatile double Offset0 = 0;
static volatile double OffsetSlope = 0;


/*-------------------------------------------------------
		interrupt function (Trigger)
-------------------------------------------------------*/
void update_trigger_led()
{
	//puts("update_trigger_led");
	if (N_AD == 20) {
		DO_off_for_ch(do_trigger);
	}

}

void update_slopeoffset()
{
	int stored_data_origin, stored_data_end;
	stored_data_origin = stored_data_at(OFFSET_RESET_DELAY);
	stored_data_end = stored_data_at(MAX_N_AD-1);

	OffsetSlope = ((float)(curr_in_buff[MAX_N_AD-1]
							-stored_data_end-curr_in_buff[OFFSET_RESET_DELAY]
						   +stored_data_origin))
					/((float)(MAX_N_AD-OFFSET_RESET_DELAY-1));
					
	Offset0 = (float)(curr_in_buff[OFFSET_RESET_DELAY]-stored_data_origin)
				-OffsetSlope*OFFSET_RESET_DELAY;
}



void output_data()
{
	double slopedoffset, current, particles;
	int cancel_data;
	FILE *out;
	update_slopeoffset();
	cancel_data = stored_data_at(N1);
	slopedoffset = Offset0+OffsetSlope*(double)N1;
	current = (double)curr_in_buff[N1]- cancel_data - slopedoffset;
	particles = current/(double)freq_in_buff[N1];
	out = fopen("dspout.txt", "w");
	fprintf(out, "%f\n", current);
	fprintf(out, "%f\n", particles);

	cancel_data = stored_data_at(N2);
	slopedoffset = Offset0+OffsetSlope*(double)N2;
	current = (double)curr_in_buff[N2]- cancel_data - slopedoffset;
	particles = current/(double)freq_in_buff[N2];
	fprintf(out, "%f\n", current);
	fprintf(out, "%f\n", particles);

	fclose(out);
}

void process_avg()
{
	double slopedoffset, current, particles;
	int cancel_data;
	update_slopeoffset();
	cancel_data = stored_data_at(N1);
	slopedoffset = Offset0+OffsetSlope*(double)N1;
	current = (double)curr_in_buff[N1]- cancel_data - slopedoffset;
	particles = current/(double)freq_in_buff[N1];
	avg_add(0, current);
	avg_add(1, particles);

	cancel_data = stored_data_at(N2);
	slopedoffset = Offset0+OffsetSlope*(double)N2;
	current = (double)curr_in_buff[N2]- cancel_data - slopedoffset;
	particles = current/(double)freq_in_buff[N2];
	avg_add(2, current);
	if (avg_add(3, particles) == full) {
		AverageStatus = end_avg;
	}
}

interrupt void c_int_ad_done()
{
	int freq_in, curr_in, buffed_freq_in, buffed_curr_in;
	int cancel_data;
	double current;
	double particles, curr_f;
	double slopedoffset;

	update_trigger_led();
	curr_in=sbox_AdGet(CURR_CH);//Get AD-data of CH0
	freq_in=sbox_AdGet(FREV_CH);
	
	cancel_data = stored_data_at(N_AD);
 	
	/* process for real time output*/
 	curr_f = curr_in;
	if (N_AD == OFFSET_RESET_DELAY) {
		CurrOffset = curr_f - cancel_data;
	}
		
	current = curr_f - cancel_data - CurrOffset;
	particles = CALIB_RATIO*current/(double)freq_in;

	sbox_DaPut(CURR_DA_CH,(int)current);
	//sbox_DaPut(CURR_DA_CH, 0);
	sbox_DaPut(CANCEL_DA_CH, cancel_data);
	sbox_DaPut(PARTC_DA_CH, (int)particles);
	//sbox_DaPut(PARTC_DA_CH, (int)current_filterd);
	sbox_DaPut(THROUGH_DA_CH, curr_in);

	/* process for delayed output */
	if (N_AD < MAX_N_AD) {
		buffed_curr_in = curr_in_buff[N_AD];
		buffed_freq_in = freq_in_buff[N_AD];
		curr_in_buff[N_AD] = curr_in;
		freq_in_buff[N_AD] = freq_in;
	} else if (N_AD == MAX_N_AD) {
		if (AverageStatus == start_avg) {
			process_avg();
		}
		buffed_curr_in = curr_in;
		buffed_freq_in = freq_in;
	/*} else if (N_AD == MAX_N_AD) {
		output_data();
		buffed_curr_in = curr_in;
		buffed_freq_in = freq_in;
		sbox_DaPut(CURR_DELAY_DA_CH, 0);
		return;*/
	} else {		
		buffed_curr_in = curr_in;
		buffed_freq_in = freq_in;
	}
	
	slopedoffset = Offset0+OffsetSlope*(double)N_AD;
	current = (double)buffed_curr_in
						- cancel_data - slopedoffset;
	particles = CALIB_RATIO*current/(double)buffed_freq_in;
	
	sbox_DaPut(CURR_DELAY_DA_CH, (int)current);
	sbox_DaPut(PARTC_DELAY_DA_CH, (int)particles);
	
	N_AD++;
} 

/* ----------------------- 
intterupt functoin for trigger in running
--------------------------*/
interrupt void c_int_triggered()
{
	DO_on_for_ch(do_trigger);
	//puts("triggered.");
	N_AD = 0;

	if (StoreStatus == should_store) {
		sbox_IntUnSet(EINT5);
		if( sbox_IntSet( AD_DONE, EINT5, c_int_ad_done_to_store ) != SBOX_OK ) {
			puts("[sbox_IntSet] error for c_int_ad_done_to_store\n");
			exit( -1 );
		}
		StoreStatus = in_store;
		ready_to_store();
	}
	
	if (StoreStatus == in_store) {
		//puts("store mode");
		if ((StoreStatus == should_clear) || (triggered_to_store())) {
			sbox_IntUnSet(EINT5);
			if( sbox_IntSet( AD_DONE, EINT5, c_int_ad_done ) != SBOX_OK ) {
				puts("[sbox_IntSet] error for c_int_ad_done\n");
				exit( -1 );
			}

			if (StoreStatus == should_clear) {
				clear_stored_data();
				DO_off_for_ch(do_recordleakfield);
			}
			
			StoreStatus = end_store;
		}
	} 
}

/* ----------------------- 
intterupt functoin for initial trigger
this function is called by EINT4. EINT4 will be remapped to c_int_triggered. 
--------------------------*/
interrupt void c_int_start_ad_da()
{
	//puts("will start ad_da");
	DO_on_for_ch(do_trigger);
	sbox_IntUnSet(EINT4);
	if( sbox_IntSet( OUT_TRG, EINT4, c_int_triggered ) != SBOX_OK ) {
		puts("[sbox_IntSet] error for c_int_triggered\n");
		exit( -1 );
	}
	
	if( sbox_IntSet( AD_DONE, EINT5, c_int_ad_done ) != SBOX_OK ) {
		puts("[sbox_IntSet] error for c_int_ad_done\n");
		exit( -1 );
	}
	
	clock_start(TIMER_0);
	N_AD = 0;
}

void check_di()
{
	int di_in;
	di_in = sbox_DiGet() & 0xFF;
	//puts("start check_di");
	//if (di_in) printf("di_in %d\n", di_in);

	if (StoreStatus == in_store) {
		if (di_in & di_recordleakfield) StoreStatus = should_clear;
	} else {
		if (NextTask = di_in & di_readsetting){}
		else if (NextTask = di_in & di_recordleakfield){}
		else if (NextTask = di_in & di_average){}
		if (NextTask) {
			//printf("NextTask %d\n", NextTask);
			DO_on_with_bits(NextTask);
		}
	}
}

/*-------------------------------------------------------
		main routine
-------------------------------------------------------*/
void main()
{		
	load_settings();
	fprintf( stdout, "start CT-DSP\n");
	fflush( stdout );

	/* system initialize */
	sbox_Init();

	NextTask = -1;
	AverageStatus = none;
	StoreStatus = end_store;
	/* clear DO */
	DO_clear();
	read_cancel_data();
	
	/* AD setup */
	if( sbox_AdTrgSet( TRG_TIMER0 ) != SBOX_OK ) {
		puts("[sbox_AdTrgSet] error \n");
		exit( -1 );
	}
	
	/* DA setup */
	sbox_DaCtrl( ON );
	if( sbox_DaTrgSet( TRG_TIMER0 ) != SBOX_OK ) {
		puts("[sbox_DaTrgSet] error \n");
		exit( -1 );
	}
	
	/* Timer0 setup */
	clock_set(SAMPLE_FREQ, TIMER_0 );
	clock_stop(TIMER_0);

	//puts("start loop");
///////////// Set Trigger Mode ////////////////
	//setup interrupt
	if( sbox_IntSet( OUT_TRG, EINT4, c_int_start_ad_da ) != SBOX_OK ) {
		puts("[sbox_IntSet] error \n");
		exit( -1 );
	}

	while(1) {
		switch (NextTask) {
			case di_readsetting:
				if (N_AD == MAX_N_AD) {
					N_AD++;
					load_settings();
					DO_off_for_ch(do_readsetting);
					NextTask = -1;
				}
				break;
			case di_average:
				switch (AverageStatus) {
					case none:
						avg_initialize(5,10);
						AverageStatus = start_avg;
						break;
					case end_avg:
						avg_output();
						DO_off_for_ch(do_average);
						NextTask = -1;
						AverageStatus = none;
						break;
					default:
						break;
				}
				break;
			case di_recordleakfield:
				StoreStatus = should_store;
				NextTask = -1;
				break;
			default:
				if ((StoreStatus == end_store)||(StoreStatus == in_store))  {
					check_di();
				}

		}
	}
}
