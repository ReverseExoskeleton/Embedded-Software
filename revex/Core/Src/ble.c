#include "imu.h"
#include "usart.h"
#include "ble.h"
#include "main.h"
#include <stdio.h>
#include <string.h>

uint8_t usart_flag = 0;
uint8_t waiting = 0;
uint8_t commandBuffer[30] = {0};
uint8_t rxBuffer[2] = {0};
uint8_t init_seq = 0;
uint8_t command_ind = 0;
uint8_t command_done = 0;


char reset[6] = "SF,2\r\n";
char config1[13] = "SR,12200000\r\n";
char config2[13] = "SS,80000001\r\n";
char ota[13] = "SR,30008000\r\n";
char name[10] = "SN,RevEx\r\n";
char reboot[5] = "R,1\r\n";
char MLDP[3] = "I\r\n";
char Ad[3] = "A\r\n";
char reset2[4] = "PZ\r\n";
char Service[37] = "PS,123456789012345678901234567890FF\r\n";
char Characteristic1[43] = "PC,12345678901234567890123456789011,12,20\r\n";
char Characteristic2[43] = "PC,12345678901234567890123456789022,14,20\r\n";
char cmd[9] = "SHW,0018,";
char ret[2] = "\n\r";

void process_response()
{
	command_done = 0;
	HAL_UART_Transmit(&huart1, commandBuffer, 5, 10);
	if(!strcmp(&commandBuffer, "ERR\r\n")){
		usart_flag = 0;
	} else {
		usart_flag = 1;
	}
}

void process_command()
{
	command_done = 0;
	HAL_UART_Transmit(&huart1, commandBuffer, 30, 10);
	/*if(!strcmp(commandBuffer[0] == 'W' && commandBuffer[1] == 'V'){

	} else {
		usart_flag = 1;
	}*/
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef * huart)
{
	if (init_seq) {
		if (command_ind == 0) for (uint8_t i = 0; i < 30; i++) commandBuffer[i] = 0;

		if (command_done == 0 && rxBuffer[0] != '\n') {
			commandBuffer[command_ind++] = rxBuffer[0];
			//HAL_UART_Receive_IT(&huart1, rxBuffer, 1);
		} else if (rxBuffer[0] == '\n') {
			commandBuffer[command_ind++] = rxBuffer[0];
			command_ind = 0;
			command_done = 1;
		}
	} else {
		if (command_ind == 0) for (uint8_t i = 0; i < 30; i++) commandBuffer[i] = 0;

		if (command_done == 0 && rxBuffer[0] != '\n') {
			commandBuffer[command_ind++] = rxBuffer[0];
			//HAL_UART_Receive_IT(&huart1, rxBuffer, 1);
		} else if (rxBuffer[0] == '\n') {
			commandBuffer[command_ind++] = rxBuffer[0];
			command_ind = 0;
			command_done = 1;
			process_command();
		}
	}
	HAL_UART_Receive_IT(&huart1, rxBuffer, 1);
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef* huart)
{
	HAL_UART_Receive_IT(&huart1, rxBuffer, 1);
}


void BLE_Init()
{
	setup_gpio(GPIOA, 6, output, 0, 0);
	setup_gpio(GPIOA, 8, output, 0, 0);
	toggle_off(GPIOA, 6);
	HAL_Delay(2000);
	toggle_off(GPIOA, 8);
	HAL_Delay(2000);
	toggle_on(GPIOA, 6);
	HAL_Delay(1000);
	HAL_UART_Transmit(&huart1, (uint8_t*)reboot, 5, 10);
	HAL_Delay(1500);
	HAL_UART_Transmit(&huart1, (uint8_t*)reset, 6, 10);
	HAL_Delay(1000);
	HAL_UART_Transmit(&huart1, (uint8_t*)reset2, 4, 10);
	HAL_Delay(1000);
	HAL_UART_Transmit(&huart1, (uint8_t*)reboot, 5, 10);
	HAL_Delay(1500);
	HAL_UART_Transmit(&huart1, (uint8_t*)config2, 13, 10);
	HAL_Delay(1000);
	HAL_UART_Transmit(&huart1, (uint8_t*)config1, 13, 10);
	HAL_Delay(1000);
	HAL_UART_Transmit(&huart1, (uint8_t*)name, 10, 10);
	HAL_Delay(1000);
	HAL_UART_Transmit(&huart1, (uint8_t*)Service, 37, 10);
	HAL_Delay(1000);
	HAL_UART_Transmit(&huart1, (uint8_t*)Characteristic1, 43, 10);
	HAL_Delay(1000);
	HAL_UART_Transmit(&huart1, (uint8_t*)Characteristic2, 43, 10);
	HAL_Delay(1000);
	HAL_UART_Transmit(&huart1, (uint8_t*)reboot, 5, 10);
	HAL_Delay(1500);
	HAL_UART_Transmit(&huart1, (uint8_t*)Ad, 3, 10);
}

void BLE_Init_IT()
{
	setup_gpio(GPIOA, 6, output, 0, 0);
	setup_gpio(GPIOA, 8, output, 0, 0);
	toggle_off(GPIOA, 6);
	HAL_Delay(2000);
	toggle_off(GPIOA, 8);
	HAL_Delay(2000);
	HAL_UART_Receive_IT(&huart1, rxBuffer, 1);
	toggle_on(GPIOA, 6);
	//HAL_UART_Receive(&huart1, rx_buffer, 5, 100);
	//HAL_UART_Transmit(&huart1, rx_buffer, 5, 100);
	while(!command_done){}
	process_response();
	while(!usart_flag)
	{
		command_done = 0;
		HAL_UART_Transmit(&huart1, (uint8_t*)reboot, 5, 10);
		while(!command_done){}
		process_response();
	}
	HAL_UART_Receive_IT(&huart1, rxBuffer, 1);
	while(!command_done){}
	command_done = 0;
	HAL_UART_Receive_IT(&huart1, rxBuffer, 1);
	while(!command_done){}
	command_done = 0;
	usart_flag = 0;
	while(!usart_flag)
	{
		HAL_UART_Transmit(&huart1, (uint8_t*)reset, 6, 10);
		HAL_UART_Receive_IT(&huart1, rxBuffer, 1);
		while(!command_done){}
		process_response();
	}
	usart_flag = 0;
	//HAL_Delay(1000);
	while(!usart_flag)
	{
		HAL_UART_Transmit(&huart1, (uint8_t*)reset2, 4, 10);
	}
	usart_flag = 0;
	//HAL_Delay(1000);
	while(!usart_flag)
	{
		HAL_UART_Transmit(&huart1, (uint8_t*)reboot, 5, 10);
	}
	usart_flag = 0;
	//HAL_Delay(1500);
	while(!usart_flag)
	{
		HAL_UART_Transmit_IT(&huart1, (uint8_t*)config2, 13);
	}
	usart_flag = 0;
	//HAL_Delay(1000);
	while(!usart_flag)
	{
		HAL_UART_Transmit_IT(&huart1, (uint8_t*)config1, 13);
	}
	usart_flag = 0;
	//HAL_Delay(1000);
	while(!usart_flag)
	{
		HAL_UART_Transmit_IT(&huart1, (uint8_t*)name, 10);
	}
	usart_flag = 0;
	//HAL_Delay(1000);
	while(!usart_flag)
	{
		HAL_UART_Transmit_IT(&huart1, (uint8_t*)reboot, 5);
	}
	usart_flag = 0;
	//HAL_Delay(1500);
	/*HAL_UART_Transmit(&huart1, (uint8_t*)Service, 37);
	HAL_Delay(1000);
	HAL_UART_Transmit(&huart1, (uint8_t*)Characteristic, 43);
	HAL_Delay(1000);
	HAL_UART_Transmit(&huart1, (uint8_t*)reboot, 5, 10);*/
	//HAL_Delay(8000);
	//HAL_UART_Transmit_IT(&huart1, (uint8_t*)MLDP, 3, 10);
	//HAL_Delay(500);
	toggle_on(GPIOA, 8);
	HAL_Delay(5000);
}

void BLE_OTA()
{
	setup_gpio(GPIOA, 6, output, 0, 0);
	setup_gpio(GPIOA, 8, output, 0, 0);
	toggle_off(GPIOA, 6);
	HAL_Delay(2000);
	toggle_off(GPIOA, 8);
	HAL_Delay(2000);
	toggle_on(GPIOA, 6);
	HAL_Delay(10000);
	HAL_UART_Transmit(&huart1, (uint8_t*)reboot, 5, 10);
	HAL_Delay(1500);
	HAL_UART_Transmit(&huart1, (uint8_t*)reset, 6, 10);
	HAL_Delay(1000);
	HAL_UART_Transmit(&huart1, (uint8_t*)ota, 13, 10);
	HAL_Delay(1000);
	HAL_UART_Transmit(&huart1, (uint8_t*)reboot, 5, 10);
	HAL_Delay(1500);
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

void BLE_recieve()
{
	HAL_UART_Receive_IT(&huart1, rxBuffer, 1);
}
