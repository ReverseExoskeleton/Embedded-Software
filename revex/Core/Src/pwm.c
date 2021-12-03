#include "tim.h"

uint8_t get_freq(uint8_t data)
{
	return (data & 0x7);
}

uint8_t get_duty(uint8_t data)
{
	return (data >> 3);
}

void set_haptic(uint8_t data)
{
	uint8_t freq_raw = get_freq(data) + 5;
	uint8_t duty_raw = get_duty(data);

	uint32_t arr = SystemCoreClock / (TIM2->PSC + 1) / freq_raw * 15 / 7 + 4;
	uint32_t ccr = arr * duty_raw / 32;
	TIM2->ARR = arr;
	TIM2->CCR2 = ccr;
}
