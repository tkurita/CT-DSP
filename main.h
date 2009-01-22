#include <stdlib.h>
#include <stdio.h>
#include "sbox.h"
#include <math.h>

/* AD channel */
#define CURR_CH 0 //CT Current input AD
#define FREV_CH 1 //revolution frequency AD

/* DA channel */
#define CURR_DA_CH 0 // output of (- field error - offset)
#define PARTC_DA_CH 1 //output of particle number
#define THROUGH_DA_CH 2 //through output of AD input with out modification
#define CANCEL_DA_CH 3 // output of field error
#define CURR_DELAY_DA_CH 4 //delayed (linear pedestal cancel) current out
#define PARTC_DELAY_DA_CH 5 //delayed (linear pedestal cancel) particle number out

#define SAMPLE_FREQ 5000.0 //[Hz] sampling frequency of ADDA
#define WORKTIME 1.85
#define MAX_N_AD 9250 // must be SAMPLE_FREQ*WORKTIME

/* customize behaivor */
#define USE_FIELD_CANCEL 1
#define FILE_ACCESS 0


//#define CALIB_RATIO 1
#define CALIB_RATIO CALIB_RATIO_PROTON
#define CALIB_RATIO_PROTON 10000*1.20455 //proton 2007.0907
#define CALIB_RATIO_CARBON 100000.0*1.20455*2 //carbon
#define CALIB_RARIO_CARBON2 100000.0*1.20455*4 // carbon 2008.04.04 current out of front module decrease to the half.
#define OFFSET_RESET_DELAY 10 //[sample]


/*
int val_before;
int val_last;
int last_val;
*/
