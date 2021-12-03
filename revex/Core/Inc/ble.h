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

void BLE_transmit(uint16_t data, uint8_t new, uint8_t end);

void BLE_receive();

bleState BLE_awaitState(bleState state);

bleData* BLE_getData();

#endif	/* __BLE_H__ */
