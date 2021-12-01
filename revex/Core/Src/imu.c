#include "imu.h"
#include "spi.h"
#include "icm20948.h"
#include "usart.h"
#include <stdio.h>

axises my_gyro;
axises my_accel;
axises my_mag;
/*uint8_t gyro_buffer[6] = {0};
uint8_t accel_buffer[6] = {0};
uint8_t mag_buffer[6] = {0};*/
raw_axises gyro;
raw_axises accel;
raw_axises mag;
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

/*void print_imu_raw()
{
	char buffer1[50] = {0};
	char buffer2[50] = {0};
	char buffer3[50] = {0};
	int l = sprintf(buffer1, "%u%u %u%u %u%u ", gyro_buffer[0], gyro_buffer[1], gyro_buffer[2], gyro_buffer[3], gyro_buffer[4], gyro_buffer[5]);
	int m = sprintf(buffer2, "%u%u %u%u %u%u ", accel_buffer[0], accel_buffer[1], accel_buffer[2], accel_buffer[3], accel_buffer[4], accel_buffer[5]);
	int n = sprintf(buffer3, "%u%u %u%u %u%u\r\n", mag_buffer[0], mag_buffer[1], mag_buffer[2], mag_buffer[3], mag_buffer[4], mag_buffer[5]);
	HAL_UART_Transmit(&huart1, (uint8_t*)buffer1, l, 100);
	HAL_UART_Transmit(&huart1, (uint8_t*)buffer2, m, 100);
	HAL_UART_Transmit(&huart1, (uint8_t*)buffer3, n, 100);
}*/

void print_imu_raw()
{
	char buffer1[50] = {0};
	char buffer2[50] = {0};
	char buffer3[50] = {0};
	int l = sprintf(buffer1, "%02x%02x%02x%02x%02x%02x", ((gyro.x & 0xff00)>>8), (gyro.x & 0xff), ((gyro.y & 0xff00)>>8), (gyro.y & 0xff), ((gyro.z & 0xff00)>>8), (gyro.z & 0xff));
	int m = sprintf(buffer2, "%02x%02x%02x%02x%02x%02x", ((accel.x & 0xff00)>>8), (accel.x & 0xff), ((accel.y & 0xff00)>>8), (accel.y & 0xff), ((accel.z & 0xff00)>>8), (accel.z & 0xff));
	int n = sprintf(buffer3, "%02x%02x%02x%02x%02x%02x\r\n", ((mag.x & 0xff00)>>8), (mag.x & 0xff), ((mag.y & 0xff00)>>8), (mag.y & 0xff), ((mag.z & 0xff00)>>8), (mag.z & 0xff));
	HAL_UART_Transmit(&huart1, (uint8_t*)buffer1, l, 100);
	HAL_UART_Transmit(&huart1, (uint8_t*)buffer2, m, 100);
	HAL_UART_Transmit(&huart1, (uint8_t*)buffer3, n, 100);
}

void IMU_read_all_raw()
{
	while(!ak09916_mag_read_raw(&mag) && attempt < 500)
	{
		attempt++;
	}

	if(attempt < 500)
	{
		icm20948_gyro_read_raw(&gyro);
		icm20948_accel_read_raw(&accel);
	}

}

void print_imu()
{
	char buffer[100] = {0};
	int l = sprintf(buffer, "%d %d %d %d %d %d %d %d %d\r\n", (int)my_gyro.x, (int)my_gyro.y, (int)my_gyro.z, (int)my_accel.x, (int)my_accel.y, (int)my_accel.z, (int)my_mag.x, (int)my_mag.y, (int)my_mag.z);
	HAL_UART_Transmit(&huart1, (uint8_t*)buffer, l, 100);
}

void IMU_read_all()
{
	while(!ak09916_mag_read(&my_mag) && attempt < 500)
	{
		attempt++;
	}
	if(attempt < 500)
	{
		//ak09916_mag_read(&my_mag);
		icm20948_gyro_read(&my_gyro);
		icm20948_accel_read(&my_accel);
	}
	attempt = 0;
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
