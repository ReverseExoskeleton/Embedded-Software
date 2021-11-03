#include "spi.h"
#include "icm20948.h"
#include "icu.h"
#include "usart.h"
#include <stdio.h>

axises my_gyro;
axises my_accel;
axises my_mag;
int attempt = 0;
char tmp [50] = "world\r\n";

static void ak09916_init();
static void icm20948_init();

void icm20948_init()
{
	while(!icm20948_who_am_i());

	icm20948_device_reset();
	icm20948_wakeup();

	icm20948_clock_source(1);
	icm20948_odr_align_enable();

	icm20948_spi_slave_enable();

	icm20948_gyro_low_pass_filter(0);
	icm20948_accel_low_pass_filter(0);

	icm20948_gyro_sample_rate_divider(0);
	icm20948_accel_sample_rate_divider(0);

	icm20948_gyro_calibration();
	icm20948_accel_calibration();

	icm20948_gyro_full_scale_select(_2000dps);
	icm20948_accel_full_scale_select(_16g);
}

void ak09916_init()
{
	icm20948_i2c_master_reset();
	icm20948_i2c_master_enable();
	icm20948_i2c_master_clk_frq(7);

	while(!ak09916_who_am_i());

	ak09916_soft_reset();
	ak09916_operation_mode_setting(continuous_measurement_100hz);
}

void IMU_Init()
{
	MX_SPI1_Init();
	icm20948_init();
	ak09916_init();
}

void IMU_read_all_raw(uint8_t ** buffer1, uint8_t ** buffer2, uint8_t ** buffer3)
{
	while(!ak09916_mag_read_raw(&buffer3))
	{
		HAL_UART_Transmit(&huart1, (uint8_t*)tmp, 7, 100);
		attempt++;
	}
	icm20948_gyro_read_raw(&buffer1);
	icm20948_accel_read_raw(&buffer2);
	char buffer4[50];
	char buffer5[50];
	char buffer6[50];
	int l = sprintf(buffer4, "%u%u%u%u%u%u%u%u", *buffer1[0], *buffer1[1], *buffer1[2], *buffer1[3], *buffer1[4], *buffer1[5]);
	int m = sprintf(buffer5, "%u%u%u%u%u%u%u%u", *buffer2[0], *buffer2[1], *buffer2[2], *buffer2[3], *buffer2[4], *buffer2[5]);
	int n = sprintf(buffer6, "%u%u%u%u%u%u%u%u\r\n", *buffer3[0], *buffer3[1], *buffer3[2], *buffer3[3], *buffer3[4], *buffer3[5]);
	HAL_UART_Transmit(&huart1, (uint8_t*)buffer4, l, 100);
	HAL_UART_Transmit(&huart1, (uint8_t*)buffer5, m, 100);
	HAL_UART_Transmit(&huart1, (uint8_t*)buffer6, n, 100);
}

void IMU_read_all()
{
	while(!ak09916_mag_read(&my_mag))
	{
		attempt++;
	}
	icm20948_gyro_read(&my_gyro);
	icm20948_accel_read(&my_accel);
}

int16_t get_magX()
{
	return (int16_t)my_mag.x;
}

int16_t get_magY()
{
	return (int16_t)my_mag.y;
}

int16_t get_magZ()
{
	return (int16_t)my_mag.z;
}

int16_t get_gyroX()
{
	return (int16_t)my_gyro.x;
}

int16_t get_gyroY()
{
	return (int16_t)my_gyro.y;
}

int16_t get_gyroZ()
{
	return (int16_t)my_gyro.z;
}

int16_t get_accelX()
{
	return (int16_t)my_accel.x;
}

int16_t get_accelY()
{
	return (int16_t)my_accel.y;
}

int16_t get_accelZ()
{
	return (int16_t)my_accel.z;
}
