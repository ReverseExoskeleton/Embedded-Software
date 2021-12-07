#include "icm20948.h"
#include "usart.h"
#include "eeprom.h"
#include <stdio.h>

static float gyro_scale_factor;
static float accel_scale_factor;
char tp [50] = "world1\r\n";
char buff[50] = {0};

static void     cs_high();
static void     cs_low();

static void     select_user_bank(userbank ub);

static uint8_t  read_single_icm20948_reg(userbank ub, uint8_t reg);
static void     write_single_icm20948_reg(userbank ub, uint8_t reg, uint8_t val);
//static uint8_t* read_multiple_icm20948_reg(userbank ub, uint8_t reg, uint8_t len);
static void     write_multiple_icm20948_reg(userbank ub, uint8_t reg, uint8_t* val, uint8_t len);

static uint8_t  read_single_ak09916_reg(uint8_t reg);
static void     write_single_ak09916_reg(uint8_t reg, uint8_t val);
//static uint8_t* read_multiple_ak09916_reg(uint8_t reg, uint8_t len);

void icm20948_gyro_read_raw(raw_axises * data)
{
	uint8_t * temp = read_multiple_icm20948_reg(ub_0, B0_GYRO_XOUT_H, 6);
	data->x = (int16_t)(temp[0] << 8 | temp[1]);
	data->y = (int16_t)(temp[2] << 8 | temp[3]);
	data->z = (int16_t)(temp[4] << 8 | temp[5]);
}

void icm20948_accel_read_raw(raw_axises * data)
{
	uint8_t * temp = read_multiple_icm20948_reg(ub_0, B0_ACCEL_XOUT_H, 6);
	data->x = (int16_t)(temp[0] << 8 | temp[1]);
	data->y = (int16_t)(temp[2] << 8 | temp[3]);
	data->z = (int16_t)(temp[4] << 8 | temp[5]);
}

bool ak09916_mag_read_raw(raw_axises * data)
{
	uint8_t drdy, hofl;	// data ready, overflow

	drdy = read_single_ak09916_reg(MAG_ST1) & 0x01;
	if(!drdy)	return false;

	uint8_t * temp = read_multiple_ak09916_reg(MAG_HXL, 6);

	hofl = read_single_ak09916_reg(MAG_ST2) & 0x08;
	if(hofl)	return false;

	data->x = (int16_t)(temp[1] << 8 | temp[0]);
	data->y = (int16_t)(temp[3] << 8 | temp[2]);
	data->z = (int16_t)(temp[5] << 8 | temp[4]);

	return true;
}

void icm20948_gyro_read(axises* data)
{
	uint8_t* temp = read_multiple_icm20948_reg(ub_0, B0_GYRO_XOUT_H, 6);

	data->x = (int16_t)(temp[0] << 8 | temp[1]);
	data->y = (int16_t)(temp[2] << 8 | temp[3]);
	data->z = (int16_t)(temp[4] << 8 | temp[5]);
}

void icm20948_accel_read(axises* data)
{
	uint8_t* temp = read_multiple_icm20948_reg(ub_0, B0_ACCEL_XOUT_H, 6);

	data->x = (int16_t)(temp[0] << 8 | temp[1]);
	data->y = (int16_t)(temp[2] << 8 | temp[3]);
	data->z = (int16_t)(temp[4] << 8 | temp[5]) + accel_scale_factor;
}

bool ak09916_mag_read(axises* data)
{
	uint8_t* temp;
	uint8_t drdy, hofl;	// data ready, overflow

	drdy = read_single_ak09916_reg(MAG_ST1) & 0x01;
	if(!drdy)	return false;

	temp = read_multiple_ak09916_reg(MAG_HXL, 6);

	hofl = read_single_ak09916_reg(MAG_ST2) & 0x08;
	if(hofl)	return false;

	data->x = (int16_t)(temp[1] << 8 | temp[0]);
	data->y = (int16_t)(temp[3] << 8 | temp[2]);
	data->z = (int16_t)(temp[5] << 8 | temp[4]);

	return true;
}

void icm20948_gyro_read_dps(axises* data)
{
	icm20948_gyro_read(data);

	data->x /= gyro_scale_factor;
	data->y /= gyro_scale_factor;
	data->z /= gyro_scale_factor;
}

void icm20948_accel_read_g(axises* data)
{
	icm20948_accel_read(data);

	data->x /= accel_scale_factor;
	data->y /= accel_scale_factor;
	data->z /= accel_scale_factor;
}

bool ak09916_mag_read_uT(axises* data)
{
	axises temp;
	bool new_data = ak09916_mag_read(&temp);
	if(!new_data)	return false;

	data->x = (float)(temp.x * 0.15);
	data->y = (float)(temp.y * 0.15);
	data->z = (float)(temp.z * 0.15);

	return true;
}

void setup_wom(uint8_t thresh)
{
	write_single_icm20948_reg(ub_2, B2_ACCEL_WOM_THR, thresh);
	write_single_icm20948_reg(ub_0, B0_INT_ENABLE, 0x08);
}

bool icm20948_who_am_i()
{
	uint8_t icm20948_id = read_single_icm20948_reg(ub_0, B0_WHO_AM_I);

	if(icm20948_id == ICM20948_ID)
		return true;
	else
		return false;
}

bool ak09916_who_am_i()
{
	uint8_t ak09916_id = read_single_ak09916_reg(MAG_WIA2);

	if(ak09916_id == AK09916_ID)
		return true;
	else
		return false;
}

void icm20948_device_reset()
{
	write_single_icm20948_reg(ub_0, B0_PWR_MGMT_1, 0x80 | 0x41);
	HAL_Delay(100);
}

void ak09916_soft_reset()
{
	write_single_ak09916_reg(MAG_CNTL3, 0x01);
	HAL_Delay(100);
}

void icm20948_wakeup()
{
	uint8_t new_val = read_single_icm20948_reg(ub_0, B0_PWR_MGMT_1);
	new_val &= 0xBF;

	write_single_icm20948_reg(ub_0, B0_PWR_MGMT_1, new_val);
	HAL_Delay(100);
}

void icm20948_sleep()
{
	uint8_t new_val = read_single_icm20948_reg(ub_0, B0_PWR_MGMT_1);
	new_val |= 0x40;

	write_single_icm20948_reg(ub_0, B0_PWR_MGMT_1, new_val);
	HAL_Delay(100);
}

void icm20948_spi_slave_enable()
{
	uint8_t new_val = read_single_icm20948_reg(ub_0, B0_USER_CTRL);
	new_val |= 0x10;

	write_single_icm20948_reg(ub_0, B0_USER_CTRL, new_val);
}

void icm20948_i2c_master_reset()
{
	uint8_t new_val = read_single_icm20948_reg(ub_0, B0_USER_CTRL);
	new_val |= 0x02;

	write_single_icm20948_reg(ub_0, B0_USER_CTRL, new_val);
}

void icm20948_i2c_master_enable()
{
	uint8_t new_val = read_single_icm20948_reg(ub_0, B0_USER_CTRL);
	new_val |= 0x20;

	write_single_icm20948_reg(ub_0, B0_USER_CTRL, new_val);
	HAL_Delay(100);
}

void icm20948_i2c_master_clk_frq(uint8_t config)
{
	uint8_t new_val = read_single_icm20948_reg(ub_3, B3_I2C_MST_CTRL);
	new_val |= config;

	write_single_icm20948_reg(ub_3, B3_I2C_MST_CTRL, new_val);
}

void icm20948_clock_source(uint8_t source)
{
	uint8_t new_val = read_single_icm20948_reg(ub_0, B0_PWR_MGMT_1);
	new_val |= source;

	write_single_icm20948_reg(ub_0, B0_PWR_MGMT_1, new_val);
}

void icm20948_odr_align_enable()
{
	write_single_icm20948_reg(ub_2, B2_ODR_ALIGN_EN, 0x01);
}

void icm20948_gyro_low_pass_filter(uint8_t config)
{
	uint8_t new_val = read_single_icm20948_reg(ub_2, B2_GYRO_CONFIG_1);
	new_val |= config << 3;

	write_single_icm20948_reg(ub_2, B2_GYRO_CONFIG_1, new_val);
}

void icm20948_accel_low_pass_filter(uint8_t config)
{
	uint8_t new_val = read_single_icm20948_reg(ub_2, B2_ACCEL_CONFIG);
	new_val |= config << 3;

	write_single_icm20948_reg(ub_2, B2_ACCEL_CONFIG, new_val);
}

void icm20948_gyro_fchoice(uint8_t enable)
{
	uint8_t new_val = read_single_icm20948_reg(ub_2, B2_GYRO_CONFIG_1);
	new_val |= (enable & 0x1);

	write_single_icm20948_reg(ub_2, B2_GYRO_CONFIG_1, new_val);
}

void icm20948_accel_fchoice(uint8_t enable)
{
	uint8_t new_val = read_single_icm20948_reg(ub_2, B2_ACCEL_CONFIG);
	new_val |= (enable & 0x1);

	write_single_icm20948_reg(ub_2, B2_ACCEL_CONFIG, new_val);
}

void icm20948_gyro_dutyCycle(uint8_t enable)
{
	uint8_t new_val = read_single_icm20948_reg(ub_0, B0_LP_CONFIG);
	new_val |= (enable & 0x1) << 4;

	write_single_icm20948_reg(ub_0, B0_LP_CONFIG, new_val);

}

void icm20948_accel_dutyCycle(uint8_t enable)
{
	uint8_t new_val = read_single_icm20948_reg(ub_0, B0_LP_CONFIG);
	new_val |= (enable & 0x1) << 5;

	write_single_icm20948_reg(ub_0, B0_LP_CONFIG, new_val);
}

icm20948_set_wakeOnMotion(uint8_t enable)
{
	write_single_icm20948_reg(ub_2, B2_ACCEL_WOM_THR, 80);	// Sets threshold to 320mg

	// Enables interrupt to propagate to pin 1
	uint8_t new_val = read_single_icm20948_reg(ub_0, B0_INT_ENABLE);
	new_val |= (enable & 0x1) << 3;

	write_single_icm20948_reg(ub_0, B0_INT_ENABLE, new_val);
}

void icm20948_gyro_sample_rate_divider(uint8_t divider)
{
	write_single_icm20948_reg(ub_2, B2_GYRO_SMPLRT_DIV, divider);
}

void icm20948_accel_sample_rate_divider(uint16_t divider)
{
	uint8_t divider_1 = (uint8_t)(divider >> 8);
	uint8_t divider_2 = (uint8_t)(0x0F & divider);

	write_single_icm20948_reg(ub_2, B2_ACCEL_SMPLRT_DIV_1, divider_1);
	write_single_icm20948_reg(ub_2, B2_ACCEL_SMPLRT_DIV_2, divider_2);
}

void ak09916_operation_mode_setting(operation_mode mode)
{
	write_single_ak09916_reg(MAG_CNTL2, mode);
	HAL_Delay(100);
}

void icm20948_gyro_calibration(uint8_t loadBias)
{
	axises temp;
	int32_t gyro_bias[3] = {0};
	uint8_t gyro_offset[6] = {0};

	if (!loadBias) {
		for(int i = 0; i < 100; i++)
		{
			icm20948_gyro_read(&temp);
			gyro_bias[0] += temp.x;
			gyro_bias[1] += temp.y;
			gyro_bias[2] += temp.z;
		}

		gyro_bias[0] /= 100;
		gyro_bias[1] /= 100;
		gyro_bias[2] /= 100;

		gyro_offset[0] = (-gyro_bias[0] / 4  >> 8) & 0xFF;
		gyro_offset[1] = (-gyro_bias[0] / 4)       & 0xFF;
		gyro_offset[2] = (-gyro_bias[1] / 4  >> 8) & 0xFF;
		gyro_offset[3] = (-gyro_bias[1] / 4)       & 0xFF;
		gyro_offset[4] = (-gyro_bias[2] / 4  >> 8) & 0xFF;
		gyro_offset[5] = (-gyro_bias[2] / 4)       & 0xFF;

		EEPROM_writeToNVM(EEPROM_gyroCalib, gyro_offset, 6);	// Save offset data
	}
	else {
		EEPROM_readfromNVM(EEPROM_gyroCalib, gyro_offset, 6);	// Load offset data
	}

	write_multiple_icm20948_reg(ub_2, B2_XG_OFFS_USRH, gyro_offset, 6);
}

void icm20948_accel_calibration(uint8_t loadBias)
{
	axises temp;
	uint8_t* temp2;
	uint8_t* temp3;
	uint8_t* temp4;

	int32_t accel_bias[3] = {0};
	int32_t accel_bias_reg[3] = {0};
	uint8_t accel_offset[6] = {0};

	if (!loadBias) {
		for(int i = 0; i < 100; i++)
		{
			icm20948_accel_read(&temp);
			accel_bias[0] += temp.x;
			accel_bias[1] += temp.y;
			accel_bias[2] -= temp.z;
		}

		accel_bias[0] /= 100;
		accel_bias[1] /= 100;
		accel_bias[2] /= 100;

		// Subtract 1 true G from bias
		accel_bias[2] -= 0xffff >> 2;

		uint8_t mask_bit[3] = {0, 0, 0};

		temp2 = read_multiple_icm20948_reg(ub_1, B1_XA_OFFS_H, 2);
		accel_bias_reg[0] = (int32_t)(temp2[0] << 8 | temp2[1]);
		mask_bit[0] = temp2[1] & 0x01;

		temp3 = read_multiple_icm20948_reg(ub_1, B1_YA_OFFS_H, 2);
		accel_bias_reg[1] = (int32_t)(temp3[0] << 8 | temp3[1]);
		mask_bit[1] = temp3[1] & 0x01;

		temp4 = read_multiple_icm20948_reg(ub_1, B1_ZA_OFFS_H, 2);
		accel_bias_reg[2] = (int32_t)(temp4[0] << 8 | temp4[1]);
		mask_bit[2] = temp4[1] & 0x01;

		accel_bias_reg[0] -= (accel_bias[0] / 8);
		accel_bias_reg[1] -= (accel_bias[1] / 8);
		accel_bias_reg[2] -= (accel_bias[2] / 8);

		accel_offset[0] = (accel_bias_reg[0] >> 8) & 0xFF;
		accel_offset[1] = (accel_bias_reg[0])      & 0xFE;
		accel_offset[1] = accel_offset[1] | mask_bit[0];

		accel_offset[2] = (accel_bias_reg[1] >> 8) & 0xFF;
		accel_offset[3] = (accel_bias_reg[1])      & 0xFE;
		accel_offset[3] = accel_offset[3] | mask_bit[1];

		accel_offset[4] = (accel_bias_reg[2] >> 8) & 0xFF;
		accel_offset[5] = (accel_bias_reg[2])      & 0xFE;
		accel_offset[5] = accel_offset[5] | mask_bit[2];

		EEPROM_writeToNVM(EEPROM_acclCalib, accel_offset, 6);	// Save offset data
	}
	else {
		EEPROM_readfromNVM(EEPROM_acclCalib, accel_offset, 6);	// Load offset data
	}



	write_multiple_icm20948_reg(ub_1, B1_XA_OFFS_H, &accel_offset[0], 2);
	write_multiple_icm20948_reg(ub_1, B1_YA_OFFS_H, &accel_offset[2], 2);
	write_multiple_icm20948_reg(ub_1, B1_ZA_OFFS_H, &accel_offset[4], 2);
}

void icm20948_gyro_full_scale_select(gyro_full_scale full_scale)
{
	uint8_t new_val = read_single_icm20948_reg(ub_2, B2_GYRO_CONFIG_1);

	switch(full_scale)
	{
		case _250dps :
			new_val |= 0x00;
			gyro_scale_factor = 131.0;
			break;
		case _500dps :
			new_val |= 0x02;
			gyro_scale_factor = 65.5;
			break;
		case _1000dps :
			new_val |= 0x04;
			gyro_scale_factor = 32.8;
			break;
		case _2000dps :
			new_val |= 0x06;
			gyro_scale_factor = 16.4;
			break;
	}

	write_single_icm20948_reg(ub_2, B2_GYRO_CONFIG_1, new_val);
}

void icm20948_accel_full_scale_select(accel_full_scale full_scale)
{
	uint8_t new_val = read_single_icm20948_reg(ub_2, B2_ACCEL_CONFIG);

	switch(full_scale)
	{
		case _2g :
			new_val |= 0x00;
			accel_scale_factor = 16384;
			break;
		case _4g :
			new_val |= 0x02;
			accel_scale_factor = 8192;
			break;
		case _8g :
			new_val |= 0x04;
			accel_scale_factor = 4096;
			break;
		case _16g :
			new_val |= 0x06;
			accel_scale_factor = 2048;
			break;
	}

	write_single_icm20948_reg(ub_2, B2_ACCEL_CONFIG, new_val);
}


static void cs_high()
{
	HAL_GPIO_WritePin(ICM20948_SPI_CS_PIN_PORT, ICM20948_SPI_CS_PIN_NUMBER, SET);
}

static void cs_low()
{
	HAL_GPIO_WritePin(ICM20948_SPI_CS_PIN_PORT, ICM20948_SPI_CS_PIN_NUMBER, RESET);
}

static void select_user_bank(userbank ub)
{
	uint8_t write_reg[2];
	write_reg[0] = WRITE | REG_BANK_SEL;
	write_reg[1] = ub;

	cs_low();
	HAL_SPI_Transmit(ICM20948_SPI, write_reg, 2, 10);
	cs_high();
}

static uint8_t read_single_icm20948_reg(userbank ub, uint8_t reg)
{
	uint8_t read_reg = READ | reg;
	uint8_t reg_val;
	select_user_bank(ub);

	cs_low();
	HAL_SPI_Transmit(ICM20948_SPI, &read_reg, 1, 1000);
	HAL_SPI_Receive(ICM20948_SPI, &reg_val, 1, 1000);
	cs_high();

	return reg_val;
}

static void write_single_icm20948_reg(userbank ub, uint8_t reg, uint8_t val)
{
	uint8_t write_reg[2];
	write_reg[0] = WRITE | reg;
	write_reg[1] = val;

	select_user_bank(ub);

	cs_low();
	HAL_SPI_Transmit(ICM20948_SPI, write_reg, 2, 1000);
	cs_high();
}

uint8_t* read_multiple_icm20948_reg(userbank ub, uint8_t reg, uint8_t len)
{
	uint8_t read_reg = READ | reg;
	static uint8_t reg_val[6];
	select_user_bank(ub);

	cs_low();
	HAL_SPI_Transmit(ICM20948_SPI, &read_reg, 1, 1000);
	HAL_SPI_Receive(ICM20948_SPI, reg_val, len, 1000);
	cs_high();

	return reg_val;
}

static void write_multiple_icm20948_reg(userbank ub, uint8_t reg, uint8_t* val, uint8_t len)
{
	uint8_t write_reg = WRITE | reg;
	select_user_bank(ub);

	cs_low();
	HAL_SPI_Transmit(ICM20948_SPI, &write_reg, 1, 1000);
	HAL_SPI_Transmit(ICM20948_SPI, val, len, 1000);
	cs_high();
}

static uint8_t read_single_ak09916_reg(uint8_t reg)
{
	write_single_icm20948_reg(ub_3, B3_I2C_SLV0_ADDR, READ | MAG_SLAVE_ADDR);
	write_single_icm20948_reg(ub_3, B3_I2C_SLV0_REG, reg);
	write_single_icm20948_reg(ub_3, B3_I2C_SLV0_CTRL, 0x81);

	HAL_Delay(1);
	return read_single_icm20948_reg(ub_0, B0_EXT_SLV_SENS_DATA_00);
}

static void write_single_ak09916_reg(uint8_t reg, uint8_t val)
{
	write_single_icm20948_reg(ub_3, B3_I2C_SLV0_ADDR, WRITE | MAG_SLAVE_ADDR);
	write_single_icm20948_reg(ub_3, B3_I2C_SLV0_REG, reg);
	write_single_icm20948_reg(ub_3, B3_I2C_SLV0_DO, val);
	write_single_icm20948_reg(ub_3, B3_I2C_SLV0_CTRL, 0x81);
}

uint8_t* read_multiple_ak09916_reg(uint8_t reg, uint8_t len)
{
	write_single_icm20948_reg(ub_3, B3_I2C_SLV0_ADDR, READ | MAG_SLAVE_ADDR);
	write_single_icm20948_reg(ub_3, B3_I2C_SLV0_REG, reg);
	write_single_icm20948_reg(ub_3, B3_I2C_SLV0_CTRL, 0x80 | len);

	HAL_Delay(1);
	return read_multiple_icm20948_reg(ub_0, B0_EXT_SLV_SENS_DATA_00, len);
}
