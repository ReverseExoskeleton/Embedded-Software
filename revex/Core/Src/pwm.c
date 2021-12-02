#include "tim.h"

void set_freq(int freq)
{
	uint32_t value = SystemCoreClock/ (TIM2->PSC + 1) / freq - 1;
	TIM2->ARR = value;
}

void set_duty(int duty)
{
	uint32_t value = TIM2->ARR * duty / 100;
	TIM2->CCR2 = value;
}
