#ifndef __ICU_H__
#define	__ICU_H__

#include "icm20948.h"

#define TIMEOUT 10

void IMU_Init(uint8_t loadBias);

void print_imu_raw();

void IMU_read_all_raw();

void print_imu();

void IMU_read_all();

int16_t get_magX();

int16_t get_magY();

int16_t get_magZ();

int16_t get_gyroX();

int16_t get_gyroY();

int16_t get_gyroZ();

int16_t get_accelX();

int16_t get_accelY();

int16_t get_accelZ();

#endif	/* __ICU_H__ */
