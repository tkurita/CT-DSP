enum addresult {
	notFull,
	full
};

int avg_initialize(short n_avg, int n_cycle);
enum addresult avg_add(short index, double val);
void avg_output();
