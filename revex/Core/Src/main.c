/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "eeprom.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ble.h"
#include "imu.h"
#include "pwm.h"
#include "icm20948.h"
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
uint32_t AD_RES = 0;
uint32_t Period = 0;
uint32_t Duty_cycle = 1000;
uint32_t val = 0;
int gate = 0;
char temp [50] = "hello\r\n";
char sleep [16] = "Going to Sleep\r\n";
char d1 [6] = "dip1\r\n";
char d2 [6] = "dip2\r\n";
uint16_t adc_buf[4096];
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void initialize_gpioa(void)
{
	RCC->IOPRSTR |= RCC_IOPRSTR_GPIOARST;
	RCC->IOPRSTR &= ~RCC_IOPRSTR_GPIOARST;
	RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
}

void initialize_gpiob(void)
{
    RCC->IOPRSTR |= RCC_IOPRSTR_GPIOBRST;
    RCC->IOPRSTR &= ~RCC_IOPRSTR_GPIOBRST;
    RCC->IOPENR |= RCC_IOPENR_GPIOBEN;
}

void setup_gpio(GPIO_TypeDef  *GPIOx, int num, enum mode m, int pupd, int speed)
{
    if(num > 15)
    {
        return;
    }
    //input mode
    if(m == input)
    {
    	GPIOx->MODER &= ~(3<<(2*num));
    	GPIOx->PUPDR &= ~(3<<(2*num));
    	//pull up = 1; pull down = 0
    	if(pupd)
    	{
    		GPIOx->PUPDR |= (1<<(2*num));
    	}
    	else
    	{
    		GPIOx->PUPDR |= (2<<(2*num));
    	}
    	if(speed == 0)
    	{
    		GPIOx->OSPEEDR &= ~(3<<(2*num));
    	}
    	if(speed == 1)
    	{
    		GPIOx->OSPEEDR &= ~(3<<(2*num));
    		GPIOx->OSPEEDR |= (1<<(2*num));
    	}
    	if(speed == 2)
		{
			GPIOx->OSPEEDR &= ~(3<<(2*num));
			GPIOx->OSPEEDR |= (2<<(2*num));
		}
    	else
    	{
    		GPIOx->OSPEEDR |= (3<<(2*num));
    	}
    }
    //output mode
    if(m == output)
    {
    	GPIOx->MODER &= ~(3<<(2*num));
    	GPIOx->MODER |= (1<<(2*num));
    	if(speed == 0)
		{
			GPIOx->OSPEEDR &= ~(3<<(2*num));
		}
		if(speed == 1)
		{
			GPIOx->OSPEEDR &= ~(3<<(2*num));
			GPIOx->OSPEEDR |= (1<<(2*num));
		}
		if(speed == 2)
		{
			GPIOx->OSPEEDR &= ~(3<<(2*num));
			GPIOx->OSPEEDR |= (2<<(2*num));
		}
		else
		{
			GPIOx->OSPEEDR |= (3<<(2*num));
		}
    }
    //alt mode
    if(m == alternate)
    {
    	GPIOx->MODER &= ~(3<<(2*num));
    	GPIOx->MODER |= (2<<(2*num));
    	if(speed == 0)
		{
			GPIOx->OSPEEDR &= ~(3<<(2*num));
		}
		if(speed == 1)
		{
			GPIOx->OSPEEDR &= ~(3<<(2*num));
			GPIOx->OSPEEDR |= (1<<(2*num));
		}
		if(speed == 2)
		{
			GPIOx->OSPEEDR &= ~(3<<(2*num));
			GPIOx->OSPEEDR |= (2<<(2*num));
		}
		else
		{
			GPIOx->OSPEEDR |= (3<<(2*num));
		}
    }
    //analog mode
    if(m == analog)
    {
    	GPIOx->MODER |= (3<<(2*num));
    	if(speed == 0)
		{
			GPIOx->OSPEEDR &= ~(3<<(2*num));
		}
		if(speed == 1)
		{
			GPIOx->OSPEEDR &= ~(3<<(2*num));
			GPIOx->OSPEEDR |= (1<<(2*num));
		}
		if(speed == 2)
		{
			GPIOx->OSPEEDR &= ~(3<<(2*num));
			GPIOx->OSPEEDR |= (2<<(2*num));
		}
		else
		{
			GPIOx->OSPEEDR |= (3<<(2*num));
		}
    }
    else
    {
    	//error state
    }
}

void setup_alt(GPIO_TypeDef  *GPIOx, int num, int mode)
{
	if(num < 8)
	{
		GPIOx->AFR[0] &= ~(0xf<<(4*(num)));
		GPIOx->AFR[0] |= mode << (4*(num));
	}
	else
	{
		GPIOx->AFR[1] &= ~(0xf<<(4*(num-8)));
		GPIOx->AFR[1] |= mode << (4*(num-8));
	}
}

void toggle_on(GPIO_TypeDef  *GPIOx, int num)
{
    GPIOx->ODR |= (1<<num);
}

void toggle_off(GPIO_TypeDef  *GPIOx, int num)
{
    GPIOx->ODR &= ~(1<<num);
}

int get_gpio(GPIO_TypeDef  *GPIOx, int num)
{
    int val = GPIOx->IDR & (1<<num);
    if(val == 0)
    {
        return 0;
    }
    return 1;
}

void setup_tim2()
{
	RCC->APB1RSTR |= RCC_APB1RSTR_TIM2RST;
	RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM2RST;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    setup_gpio(GPIOA, 1, alternate, 0, 1);
    setup_alt(GPIOA, 1, 2);
    //TIM2->DIER |= TIM_DIER_UDE | TIM_DIER_CC2DE;
    TIM2->CR1 |= TIM_CR1_ARPE;
    //TIM2->EGR |= TIM_EGR_UG; // TIM_EGR_CC2G;
    TIM2->CCMR1 |= TIM_CCMR1_OC2PE + TIM_CCMR1_OC2M_2 + TIM_CCMR1_OC2M_1;
    TIM2->PSC = 16000;
    TIM2->ARR = 50;
    TIM2->CCR2 = 40;
    TIM2->CCER |= TIM_CCER_CC2E;
    TIM2->CR1 |= TIM_CR1_CEN;
    TIM2->EGR |= TIM_EGR_UG;
}

void setup_tim6()
{
	RCC->APB1RSTR |= RCC_APB1RSTR_TIM6RST;
	RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM6RST;
	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
	TIM6->PSC = 16000;
	TIM6->ARR = 100;
	TIM6->DIER |= TIM_DIER_UIE;
	NVIC_EnableIRQ(TIM6_DAC_IRQn); /* (1) */
	NVIC_SetPriority(TIM6_DAC_IRQn,0);
	TIM6->CR1 |= TIM_CR1_CEN;
	TIM6->EGR |= TIM_EGR_UG;
}


void PWM_config()
{
	RCC->AHBENR |= RCC_AHBENR_DMA1EN; /* (1) */
	DMA1_CSELR->CSELR |= 8 << (4 * (5-1)) | 8 << (4 * (7-1)); /* (2) */
	DMA1_Channel5->CPAR = (uint32_t) (&(TIM2->ARR)); /* (3) */
	DMA1_Channel5->CMAR = (uint32_t)(&Period); /* (4) */
	DMA1_Channel5->CNDTR = 1; /* (5) */
	DMA1_Channel5->CCR |= DMA_CCR_MSIZE_0 | DMA_CCR_PSIZE_0 \
	 | DMA_CCR_TEIE | DMA_CCR_CIRC; /* (6) */
	DMA1_Channel5->CCR |= DMA_CCR_EN; /* (7) */
	/* repeat (3) to (6) for channel 6 */
	DMA1_Channel7->CPAR = (uint32_t) (&(TIM2->CCR2)); /* (2) */
	DMA1_Channel7->CMAR = (uint32_t)(&Duty_cycle); /* (3) */
	DMA1_Channel7->CNDTR = 1; /* (4) */
	DMA1_Channel7->CCR |= DMA_CCR_MSIZE_0 | DMA_CCR_PSIZE_0 \
	 | DMA_CCR_TEIE | DMA_CCR_CIRC; /* (5) */
	DMA1_Channel7->CCR |= DMA_CCR_EN; /* (6) */
	/* Configure NVIC for DMA */
	/* (1) Enable Interrupt on DMA Channels x */
	/* (2) Set priority for DMA Channels x */
	NVIC_EnableIRQ(DMA1_Channel4_5_6_7_IRQn); /* (1) */
	NVIC_SetPriority(DMA1_Channel4_5_6_7_IRQn,3);
}

void ADC_config()
{
	RCC->AHBRSTR |= RCC_AHBRSTR_DMA1RST;
	RCC->APB2RSTR |= RCC_APB2RSTR_ADCRST;
	RCC->AHBRSTR &= ~RCC_AHBRSTR_DMA1RST;
	RCC->APB2RSTR &= ~RCC_APB2RSTR_ADCRST;
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;
	RCC->APB2ENR |= RCC_APB2ENR_ADCEN;
	setup_gpio(GPIOA, 7, analog, 0, 1);
	DMA1_CSELR->CSELR &= (uint32_t)(~DMA_CSELR_C1S);
	DMA1_Channel1->CPAR = (uint32_t) (&(ADC1->DR));
	DMA1_Channel1->CMAR = (uint32_t) (&AD_RES);
	DMA1_Channel1->CNDTR = 1;
	DMA1_Channel1->CCR |= DMA_CCR_MSIZE_0 | DMA_CCR_PSIZE_0 | DMA_CCR_TEIE | DMA_CCR_TCIE | DMA_CCR_CIRC;
	DMA1_Channel1->CCR |= DMA_CCR_EN;
	NVIC_EnableIRQ(DMA1_Channel1_IRQn);
	NVIC_SetPriority(DMA1_Channel1_IRQn,0);
	if ((ADC1->CR & ADC_CR_ADEN) != 0)
	{
		ADC1->CR |= ADC_CR_ADDIS;
	}
	ADC1->IER = ADC_IER_EOCALIE |ADC_IER_EOCIE | ADC_IER_EOSIE | ADC_IER_OVRIE | ADC_IER_ADRDYIE;
	ADC1->CR |= ADC_CR_ADCAL;
	while ((ADC1->ISR & ADC_ISR_EOCAL) == 0)
	{}
	ADC1->ISR |= ADC_ISR_EOCAL;

	ADC1->CFGR1 |= ADC_CFGR1_DMAEN | ADC_CFGR1_DMACFG;
	ADC1->CHSELR = ADC_CHSELR_CHSEL7;
	ADC1->SMPR |= ADC_SMPR_SMP_0 | ADC_SMPR_SMP_1 | ADC_SMPR_SMP_2;
	ADC1->ISR |= ADC_ISR_ADRDY;
	ADC1->CR |= ADC_CR_ADEN;
	if ((ADC1->CFGR1 & ADC_CFGR1_AUTOFF) == 0)
	{
		 while ((ADC1->ISR & ADC_ISR_ADRDY) == 0)
		 {}
	}
}

void print_adc(void)
{
	char buffer[20];
	uint8_t value = 0;
	if(AD_RES < 420 || AD_RES > 1598)
	{
		value = 255;
	}
	else
	{
		value = AD_RES * 10201/100000 - 13;
	}
	int l = sprintf(buffer, "SHW,0018,%04x", AD_RES);
	HAL_UART_Transmit(&huart1, (uint8_t*)buffer, l, 100);
}

void sample()
{
	HAL_ADC_Start_DMA(&hadc, &AD_RES, 1);
	print_adc();
	print_imu_raw();
	if(AD_RES > 2000)
	{
		set_freq(10);
		set_duty(20);
	}
	else
	{
		set_freq(20);
		set_duty(80);
	}
	/*ADC1->CR |= ADC_CR_ADSTP;
	ADC1->ISR |= 0xf;
	while(ADC1->CR == ADC_CR_ADSTART){}
	ADC1->CR |= ADC_CR_ADSTART;
	print_adc();*/
	//HAL_ADC_Start_IT(&hadc);
	//__asm__("cpsid if");
	//IMU_read_all_raw(&gyro_buffer, &accel_buffer, &mag_buffer);
	//HAL_UART_Transmit(&huart1, (uint8_t*)temp, 7, 100);
	//BLE_transmit(gyro_buffer, accel_buffer, mag_buffer);
	//__asm__("cpsie if");
}

void debug_imu()
{
	char buffer4[100] = {0};
	int l = sprintf(buffer4, "gyro.x gyro.y gyro.z accel.x accel.y accel.z mag.x mag.y mag.z\r\n");
	HAL_UART_Transmit(&huart1, (uint8_t*)buffer4, l, 100);
	while(1)
	{
		IMU_read_all();
	}
}

uint8_t go_goDipSwitch()
{
	setup_gpio(GPIOA, 2, input, 0, 0);
	setup_gpio(GPIOA, 3, input, 0, 0);
	HAL_Delay(50);
	uint8_t dip1 = get_gpio(GPIOA, 2);
	uint8_t dip2 = get_gpio(GPIOA, 3);
	if(dip1) HAL_UART_Transmit(&huart1, (uint8_t*)d1, 6, 100);
	if(dip2) HAL_UART_Transmit(&huart1, (uint8_t*)d2, 6, 100);
	uint8_t ret_val = (dip2 << 1) | dip1;
	return ret_val;
}

void reset_reg()
{
	RCC->AHBRSTR |= RCC_AHBRSTR_DMA1RST;
	RCC->APB2RSTR |= RCC_APB2RSTR_ADCRST;
	RCC->APB1RSTR |= RCC_APB1RSTR_TIM2RST;
	RCC->APB1RSTR |= RCC_APB1RSTR_TIM6RST;
	RCC->IOPRSTR |= RCC_IOPRSTR_GPIOARST;
	RCC->IOPRSTR |= RCC_IOPRSTR_GPIOBRST;
	RCC->IOPRSTR &= ~RCC_IOPRSTR_GPIOBRST;
	RCC->IOPRSTR &= ~RCC_IOPRSTR_GPIOARST;
	RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM6RST;
	RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM2RST;
	RCC->AHBRSTR &= ~RCC_AHBRSTR_DMA1RST;
	RCC->APB2RSTR &= ~RCC_APB2RSTR_ADCRST;
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	uint8_t dips;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  initialize_gpioa();
  initialize_gpiob();
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM3_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  MX_DMA_Init();
  MX_TIM6_Init();
  MX_ADC_Init();
  /* USER CODE BEGIN 2 */
  dips = go_goDipSwitch();
  HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
  setup_gpio(GPIOA, 1, output, 0, 0);
  setup_tim2();
  toggle_off(GPIOA, 1);
  if(dips == 0 || dips == 1) // NORMAL OPERATION
  {
	  // passes dips to determine if calibration is loaded or created
	  IMU_Init(!dips);
	  BLE_Init();
  }
  if(dips == 2)
  {
	  HAL_UART_Transmit(&huart1, (uint8_t*)sleep, 16, 100);
	  reset_reg();
	  HAL_PWR_EnterSTANDBYMode();

  }
  //ADC_config();
  HAL_Delay(3000);
  //setup_tim6();
  HAL_TIM_Base_Start_IT(&htim6);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  //debug_imu();
  while (1)
  {
	  IMU_read_all_raw();
	  asm("NOP");
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
