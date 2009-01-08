#include <stdlib.h>
#include <stdio.h>
#include "sbox.h"
#include "DO_controller.h"

static volatile int CurrentStatus;

void DO_clear()
{
	sbox_DoPut(0);
	CurrentStatus = 0x00;
}

void DO_on_for_ch(int ch_num)
{
	int target_ch;
	//printf("target ch %d\n",ch_num);
	target_ch = 0x01<<ch_num;
	//puts("start DO_on_for_ch\n");
	CurrentStatus = CurrentStatus | target_ch;
	//printf("DO Status %X \n", CurrentStatus); 
	sbox_DoPut(CurrentStatus);	
}

void DO_keep()
{
	sbox_DoPut(CurrentStatus);
}

void DO_off_for_ch(int ch_num)
{
	int target_ch = 0x01<<ch_num;
	CurrentStatus = CurrentStatus & ~target_ch;
	sbox_DoPut(CurrentStatus);
}
