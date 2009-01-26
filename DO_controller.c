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

void DO_on_with_bits(int target_ch)
{
	CurrentStatus = CurrentStatus | target_ch;
	sbox_DoPut(CurrentStatus);
}

void DO_on_for_ch(int ch_num)
{
	int target_ch;
	target_ch = 0x01<<ch_num;
	DO_on_with_bits(target_ch);
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
