#include "usart.h"
#include "ble.h"
#include "icu.h"
#include <stdio.h>

char reset[6] = "SF,2\r\n";
char config[13] = "SR,30000000\r\n";
char name[10] = "S-,RevEx\r\n";
char reboot[5] = "R,1\r\n";
char MLDP[3] = "I\r\n";

void BLE_Init()
{
	MX_USART1_UART_Init();
	HAL_Delay(2000);
	HAL_UART_Transmit(&huart1, (uint8_t*)reboot, 5, 10);
	HAL_Delay(1000);
	HAL_UART_Transmit(&huart1, (uint8_t*)reset, 6, 10);
	HAL_Delay(1000);
	HAL_UART_Transmit(&huart1, (uint8_t*)reboot, 5, 10);
	HAL_Delay(1000);
	HAL_UART_Transmit(&huart1, (uint8_t*)config, 13, 10);
	HAL_Delay(1000);
	HAL_UART_Transmit(&huart1, (uint8_t*)name, 10, 10);
	HAL_Delay(1000);
	HAL_UART_Transmit(&huart1, (uint8_t*)reboot, 5, 10);
	HAL_Delay(1000);
	//HAL_UART_Transmit(&huart1, (uint8_t*)MLDP, 3, 10);
}

void BLE_transmit(uint8_t * data)
{
	//char buffer[50];
	//int n = sprintf(buffer, "%u%u %u%u %u%u\n\n", data[0], data[1], data[2], data[3], data[4], data[5]);
	HAL_UART_Transmit(&huart1, data, 6, 100);
}
