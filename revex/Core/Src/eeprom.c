/*
 * eeprom.c
 *
 *  Created on: Dec 2, 2021
 *      Author: rumple0
 */
#include "eeprom.h"
#include "string.h"
#include "stm32l081xx.h"

void EEPROM_unlock() {
	while ((FLASH->SR & FLASH_SR_BSY) != 0){
		// Maybe perform timeout here?
	}

	if ((FLASH->PECR & FLASH_PECR_PELOCK) != 0) {
		FLASH->PEKEYR = FLASH_PEKEY1;
		FLASH->PEKEYR = FLASH_PEKEY2;
	}
}

uint8_t EEPROM_writeToNVM(uint16_t offset, uint8_t* in_buffer, uint16_t size) {
	uint8_t success = 1;

	memcpy((void*)(EEPROM_baseAddress + offset), (void*)in_buffer, size);

	return success;
}

uint8_t EEPROM_readfromNVM(uint16_t offset, uint8_t* out_buffer, uint16_t size) {
	uint8_t success = 1;

	memcpy((void*)out_buffer, (void*)(EEPROM_baseAddress + offset), size);

	return success;
}
