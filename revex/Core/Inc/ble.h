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
	uint8_t buffer[32];
	uint8_t length;
	uint8_t dataRdy;
} bleData;

bleState BLE_Init_IT();

void BLE_OTA();

void BLE_transmit(uint8_t* data, uint16_t length);

bleState BLE_awaitState(bleState state);

bleData* BLE_getData();

#endif	/* __BLE_H__ */
