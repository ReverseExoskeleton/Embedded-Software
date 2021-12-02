/*
 * eeprom.h
 *
 *  Created on: Dec 2, 2021
 *      Author: rumple0
 */

#ifndef INC_EEPROM_H_
#define INC_EEPROM_H_

#include <stdint.h>

#define EEPROM_baseAddress 0x08080000
#define EEPROM_imuCalib 0x0	// 12 Bytes of data for Accel and Gyro

// EEPROM Keys
#define FLASH_PEKEY1 0x89ABCDEF
#define FLASH_PEKEY2 0x02030405

void EEPROM_unlock();
uint8_t EEPROM_writeToNVM(uint16_t offset, uint8_t* in_buffer, uint16_t size);
uint8_t EEPROM_readfromNVM(uint16_t offset, uint8_t* out_buffer, uint16_t size);

#endif /* INC_EEPROM_H_ */
