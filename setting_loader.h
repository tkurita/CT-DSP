#define settingsfile "settings.txt"

extern float SAMPLE_FREQ;  //[Hz] sampling frequency of ADDA
extern float WORKTIME;
extern unsigned int MAX_N_AD;  // must be SAMPLE_FREQ*WORKTIME
extern double CURRENT_FACTOR;
extern double CHARGE_FACTOR;
extern unsigned int HARMONICS;
extern double T1;
extern double T2;
extern unsigned int AVERAGE_FLAG;

void load_settings();
