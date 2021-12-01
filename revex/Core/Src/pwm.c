#include "tim.h"

void set_freq(int freq)
{
	uint32_t scaled = SystemCoreClock/ (TIM2->PSC + 1) - 1;
	TIM2->ARR = scaled;
}

void set_duty(int duty)
{
	uint32_t value = duty * TIM2->ARR;
	TIM2->CCR2 = value;
}
