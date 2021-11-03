#include "usart.h"
#include "ble.h"
#include "icu.h"
#include <stdio.h>

char reset[6] = "SF,2\r\n";
char config1[13] = "SR,30000000\r\n";
char config2[13] = "SS,80000001\r\n";
char name[8] = "SN,Rev\r\n";
char reboot[5] = "R,1\r\n";
char MLDP[3] = "I\r\n";
char reset2[4] = "PZ\r\n";
char Service[37] = "PS,123456789012345678901234567890FF\r\n";
char Characteristic[43] = "PC,12345678901234567890123456789011,12,12\r\n";
char cmd[9] = "SHW,0018,";
char ret[2] = "\n\r";

void BLE_Init()
{
	HAL_Delay(5000);
	HAL_UART_Transmit(&huart1, (uint8_t*)reboot, 5, 10);
	HAL_Delay(500);
	HAL_UART_Transmit(&huart1, (uint8_t*)reset, 6, 10);
	HAL_Delay(500);
	HAL_UART_Transmit(&huart1, (uint8_t*)reset2, 4, 10);
	HAL_Delay(500);
	HAL_UART_Transmit(&huart1, (uint8_t*)reboot, 5, 10);
	HAL_Delay(1000);
	HAL_UART_Transmit(&huart1, (uint8_t*)config2, 13, 10);
	HAL_Delay(500);
	HAL_UART_Transmit(&huart1, (uint8_t*)config1, 13, 10);
	HAL_Delay(500);
	HAL_UART_Transmit(&huart1, (uint8_t*)name, 8, 10);
	HAL_Delay(500);
	HAL_UART_Transmit(&huart1, (uint8_t*)reboot, 5, 10);
	HAL_Delay(1000);
	HAL_UART_Transmit(&huart1, (uint8_t*)Service, 37, 10);
	HAL_Delay(500);
	HAL_UART_Transmit(&huart1, (uint8_t*)Characteristic, 43, 10);
	HAL_Delay(500);
	HAL_UART_Transmit(&huart1, (uint8_t*)reboot, 5, 10);
	HAL_Delay(1000);
}

void BLE_transmit(uint8_t * data1, uint8_t * data2, uint8_t * data3)
{
	char buffer1[50];
	char buffer2[50];
	char buffer3[50];
	int l = sprintf(buffer1, "%x%x%x%x%x%x%x%x", data1[0], data1[1], data1[2], data1[3], data1[4], data1[5]);
	int m = sprintf(buffer2, "%x%x%x%x%x%x%x%x", data2[0], data2[1], data2[2], data2[3], data2[4], data2[5]);
	int n = sprintf(buffer3, "%x%x%x%x%x%x%x%x", data3[0], data3[1], data3[2], data3[3], data3[4], data3[5]);
	//HAL_UART_Transmit(&huart1, (uint8_t*)cmd, 9, 10);
	//HAL_UART_Transmit(&huart1, (uint8_t*)buffer1, l, 100);
	//HAL_UART_Transmit(&huart1, (uint8_t*)buffer2, m, 100);
	//HAL_UART_Transmit(&huart1, (uint8_t*)buffer3, n, 100);
	//HAL_UART_Transmit(&huart1, (uint8_t*)ret, 2, 10);

}
