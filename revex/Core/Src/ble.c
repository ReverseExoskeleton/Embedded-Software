#include "imu.h"
#include "usart.h"
#include "ble.h"
#include "main.h"
#include <stdio.h>
#include <string.h>

bleInfo BLE_Info;
bleData BLE_data;

uint8_t usart_flag = 0;
uint8_t waiting = 0;
uint8_t commandBuffer[30] = {0};
uint8_t txBuffer[64];
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
char StopAd[3] = "Y\r\n";
char reset2[4] = "PZ\r\n";
char Service[37] = "PS,123456789012345678901234567890FF\r\n";
char Characteristic1[43] = "PC,12345678901234567890123456789011,12,20\r\n";
char Characteristic2[43] = "PC,12345678901234567890123456789022,14,02\r\n";
char Characteristic3[43] = "PC,12345678901234567890123456789033,12,04\r\n";
char cmdData[9] = "SHW,0018,";
char ret[2] = "\n\r";


void process_response()
{
	if (BLE_data.dataRdy) {
		if (BLE_data.buffer[0] == 'C' && BLE_data.buffer[1] == 'M' && BLE_data.buffer[2] == 'D') {
			BLE_Info.currentState = BLE_CMD;
		}
		else if (BLE_data.buffer[0] == 'A' && BLE_data.buffer[1] == 'O' && BLE_data.buffer[2] == 'K') {
			BLE_Info.currentState = BLE_AOK;
		}
		else if (BLE_data.buffer[0] == 'R' && BLE_data.buffer[1] == 'e' && BLE_data.buffer[2] == 'b') {
			BLE_Info.currentState = BLE_REBOOT;
		}
		else if (BLE_data.buffer[0] == 'E' && BLE_data.buffer[0] == 'R' && BLE_data.buffer[0] == 'R') {
			BLE_Info.currentState = BLE_ERR;
		}

		BLE_data.dataRdy = 0;	// We are done with this information
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef * huart)
{
	BLE_data.dataRdy = 1;

	if (!BLE_Info.init)
	{
		process_response();
	}

	// Need to re-init the DMA
	HAL_UART_Receive_DMA(&huart1, BLE_data.buffer, 32);
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef* huart)
{
	if ((huart->ErrorCode & HAL_UART_ERROR_RTO) == HAL_UART_ERROR_RTO)
	{
		HAL_UART_RxCpltCallback(huart);
	}
}

static void DMA_Init(void)
{
	__HAL_RCC_DMA1_CLK_ENABLE();

	HAL_UART_EnableReceiverTimeout(&huart1);
	HAL_UART_ReceiverTimeout_Config(&huart1, 20);

	HAL_UART_MspInit(&huart1);

	HAL_NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);

	HAL_UART_Receive_DMA(&huart1, BLE_data.buffer, 32);
}

bleState BLE_Init_IT()
{
	// Init DMA
	DMA_Init();

	uint8_t retry = BLE_Info.currentState == BLE_ERR;

	// Setup information
	BLE_Info.currentState = BLE_FREE;
	BLE_Info.awaitingState = BLE_FREE;
	BLE_Info.init = 0;

	BLE_data.dataRdy = 0;
	BLE_data.length = 0;

	memcpy(txBuffer, cmdData, 9);

	if (!retry) {
		setup_gpio(GPIOA, 6, output, 0, 0);
		setup_gpio(GPIOA, 8, output, 0, 0);
	}

	toggle_off(GPIOA, 6);
	toggle_off(GPIOA, 8);
	HAL_Delay(4000);
	BLE_Info.currentState = BLE_FREE;
	toggle_on(GPIOA, 6);
	BLE_awaitState(BLE_CMD);

	// Reboot
	BLE_Info.currentState = BLE_FREE;
	HAL_UART_Transmit(&huart1, (uint8_t*)reboot, 5, 10);
	BLE_awaitState(BLE_REBOOT);
	if (BLE_awaitState(BLE_CMD) == BLE_ERR) {
		//toggle_off(GPIOA, 6);
		//return BLE_ERR;
	}

	// Reset
	BLE_Info.currentState = BLE_FREE;
	HAL_UART_Transmit(&huart1, (uint8_t*)reset, 6, 10);
	BLE_awaitState(BLE_AOK);

	// Reset 2
	BLE_Info.currentState = BLE_FREE;
	HAL_UART_Transmit(&huart1, (uint8_t*)reset2, 4, 10);
	BLE_awaitState(BLE_AOK);

	// Reboot
	BLE_Info.currentState = BLE_FREE;
	HAL_UART_Transmit(&huart1, (uint8_t*)reboot, 5, 10);
	BLE_awaitState(BLE_REBOOT);
	BLE_awaitState(BLE_CMD);

	// Config2
	BLE_Info.currentState = BLE_FREE;
	HAL_UART_Transmit(&huart1, (uint8_t*)config2, 13, 10);
	BLE_awaitState(BLE_AOK);

	// Config1
	BLE_Info.currentState = BLE_FREE;
	HAL_UART_Transmit(&huart1, (uint8_t*)config1, 13, 10);
	BLE_awaitState(BLE_AOK);

	// Name
	BLE_Info.currentState = BLE_FREE;
	HAL_UART_Transmit(&huart1, (uint8_t*)name, 10, 10);
	BLE_awaitState(BLE_AOK);

	// Service
	BLE_Info.currentState = BLE_FREE;
	HAL_UART_Transmit(&huart1, (uint8_t*)Service, 37, 10);
	BLE_awaitState(BLE_AOK);

	// Char1
	BLE_Info.currentState = BLE_FREE;
	HAL_UART_Transmit(&huart1, (uint8_t*)Characteristic1, 43, 10);
	BLE_awaitState(BLE_AOK);

	// Char2
	BLE_Info.currentState = BLE_FREE;
	HAL_UART_Transmit(&huart1, (uint8_t*)Characteristic2, 43, 10);
	BLE_awaitState(BLE_AOK);

	// Char3
	BLE_Info.currentState = BLE_FREE;
	HAL_UART_Transmit(&huart1, (uint8_t*)Characteristic3, 43, 10);
	BLE_awaitState(BLE_AOK);

	// Reboot
	BLE_Info.currentState = BLE_FREE;
	HAL_UART_Transmit(&huart1, (uint8_t*)reboot, 5, 10);
	BLE_awaitState(BLE_REBOOT);
	BLE_awaitState(BLE_CMD);

	// We want to wait to advertise until EVERYTHING is init

	BLE_Info.init = 1;

	return;
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

void BLE_lowPower(void)
{
	HAL_UART_Transmit(&huart1, (uint8_t*)StopAd, 3, 10);

	toggle_off(GPIOA, 6);	// Pull BT Line low
}

void BLE_adv(void) {
	HAL_UART_Transmit(&huart1, (uint8_t*)Ad, 3, 10);
}

void BLE_transmit(uint8_t* data, uint16_t length)
{
	memcpy(&(txBuffer[9]), data, length);

	memcpy(&(txBuffer[49]), ret, 2);

	HAL_UART_Transmit(&huart1, txBuffer, length + 11, 10);
//	HAL_UART_Transmit_DMA(&huart1, data, length);
}

bleState BLE_awaitState(bleState state) {
	/* Currently a blocking call */
	uint16_t err_ct = 0;

	if (state == BLE_ERR) {
		return BLE_ERR;
	}

	BLE_Info.awaitingState = state;

	while ((BLE_Info.currentState != BLE_Info.awaitingState || BLE_Info.currentState == BLE_ERR) && err_ct <= 2000) { err_ct++; HAL_Delay(1); }

	if (err_ct >= 2000) { BLE_Info.currentState = BLE_ERR; }

	return BLE_Info.currentState;
}

/* Returns null if data is not ready yet*/
bleData* BLE_getData() {
	if (!BLE_data.dataRdy) { return NULL; }

	BLE_data.dataRdy = 0;

	return &BLE_data;
}
