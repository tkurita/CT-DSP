#include "main.h"
//static far volatile int data[MAX_N_AD]; //magnetic field signal. subtracted from current signal
#include "leak_field.h"
#include "DO_controller.h"
#include "setting_loader.h"

/*define global variables*/

//static volatile int Triggered; //set to 1 if triggered.
volatile int N_AD; //incremented with AD_DONE interruput. rest to 0 when OUT_TRIG intterupt
static volatile int ShouldStoreData; //when button is pushed, this flag raise
static volatile int InStoreMode;
static volatile int ShouldClearData;

static volatile double CurrOffset; //offset, obtained when first ad_get after triggered

static far volatile int curr_in_buff[MAX_N_AD];
static far volatile int freq_in_buff[MAX_N_AD];
 
static volatile double Offset0 = 0;
static volatile double OffsetSlope = 0;

//volatile struct fir_queue lpr_queue;
//static struct fir_queue lpr_queue;
//static struct fir_queue bsf_queue;


/*-------------------------------------------------------
		interrupt function (Trigger)
-------------------------------------------------------*/
void update_trigger_led()
{
	//printf("N_AD %d\n", N_AD);
	if (N_AD == 20) {
		DO_off_for_ch(0);;
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
	//curr_f = apply_fir(&lpr_queue, (double)curr_in);
	//curr_f = apply_fir(&bsf_queue, curr_f);
	//curr_f = apply_fir(&bsf_queue, (double)curr_in);
	if (N_AD == OFFSET_RESET_DELAY) {
		CurrOffset = curr_f - cancel_data;
	}
		
	current = curr_f - cancel_data - CurrOffset;
	//current_filterd = apply_fir(&bsf_queue, current);
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
	int stored_data_origin, stored_data_end;
	DO_on_for_ch(0);
	//puts("triggered.");
	N_AD = 0;

	if (ShouldStoreData) {
		sbox_IntUnSet(EINT5);
		if( sbox_IntSet( AD_DONE, EINT5, c_int_ad_done_to_store ) != SBOX_OK ) {
			puts("[sbox_IntSet] error for c_int_ad_done_to_store\n");
			exit( -1 );
		}
		ShouldStoreData = 0;
		InStoreMode = 1;
		ready_to_store();
	}
	
	if (InStoreMode) {
		//puts("store mode");
		if (ShouldClearData || (triggered_to_store())) {
			sbox_IntUnSet(EINT5);
			if( sbox_IntSet( AD_DONE, EINT5, c_int_ad_done ) != SBOX_OK ) {
				puts("[sbox_IntSet] error for c_int_ad_done_to_store\n");
				exit( -1 );
			}

			if (ShouldClearData) {
				clear_stored_data();
				ShouldClearData = 0;
				DO_off_for_ch(1);
			}
			
			InStoreMode = 0;
		}
	} else {
		stored_data_origin = stored_data_at(OFFSET_RESET_DELAY);
		stored_data_end = stored_data_at(MAX_N_AD-1);

		OffsetSlope = ((float)(curr_in_buff[MAX_N_AD-1]
							-stored_data_end-curr_in_buff[OFFSET_RESET_DELAY]+stored_data_origin))
					/((float)(MAX_N_AD-OFFSET_RESET_DELAY-1));
					
		Offset0 = (float)(curr_in_buff[OFFSET_RESET_DELAY]-stored_data_origin)
				-OffsetSlope*OFFSET_RESET_DELAY;
	}
} 

/* ----------------------- 
intterupt functoin for initial trigger 
--------------------------*/
interrupt void c_int_start_ad_da()
{
	//puts("will start ad_da");
	DO_on_for_ch(0);
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
	if (InStoreMode) {
		ShouldClearData = di_in&0x01;
	} else {
		ShouldStoreData = di_in&0x01;
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


///////////// Set Trigger Mode ////////////////
	//setup interrupt
//	Triggered=0;
	if( sbox_IntSet( OUT_TRG, EINT4, c_int_start_ad_da ) != SBOX_OK ) {
		puts("[sbox_IntSet] error \n");
		exit( -1 );
	}
	
	ShouldStoreData = 0;
	InStoreMode = 0;
	ShouldClearData = 0;
	while(1) {
		if ((!ShouldStoreData) && (!ShouldClearData)) {
			check_di();
		}	
	}
}


