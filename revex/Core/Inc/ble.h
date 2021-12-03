#ifndef __BLE_H__
#define	__BLE_H__

typedef enum bleState{
	BLE_AOK,
	BLE_CMD,
	BLE_REBOOT,
	BLE_ERR,
	BLE_FREE
} bleState;

typedef struct {
	bleState currentState;
	bleState awaitingState;
	uint8_t init;
} bleInfo;

typedef struct {
	uint8_t buffer[20];
	uint8_t length;
	uint8_t dataRdy;
} bleData;

void BLE_Init();

void BLE_OTA();

void BLE_transmit(uint8_t * data1, uint8_t * data2, uint8_t * data3);

bleState BLE_awaitState(bleState state);

bleData* BLE_getData();

#endif	/* __BLE_H__ */
