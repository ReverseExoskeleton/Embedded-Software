################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/adc.c \
../Core/Src/ble.c \
../Core/Src/dma.c \
../Core/Src/eeprom.c \
../Core/Src/gpio.c \
../Core/Src/i2c.c \
../Core/Src/icm20948.c \
../Core/Src/imu.c \
../Core/Src/main.c \
../Core/Src/pwm.c \
../Core/Src/spi.c \
../Core/Src/stm32l0xx_hal_msp.c \
../Core/Src/stm32l0xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32l0xx.c \
../Core/Src/tim.c \
../Core/Src/usart.c 

OBJS += \
./Core/Src/adc.o \
./Core/Src/ble.o \
./Core/Src/dma.o \
./Core/Src/eeprom.o \
./Core/Src/gpio.o \
./Core/Src/i2c.o \
./Core/Src/icm20948.o \
./Core/Src/imu.o \
./Core/Src/main.o \
./Core/Src/pwm.o \
./Core/Src/spi.o \
./Core/Src/stm32l0xx_hal_msp.o \
./Core/Src/stm32l0xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32l0xx.o \
./Core/Src/tim.o \
./Core/Src/usart.o 

C_DEPS += \
./Core/Src/adc.d \
./Core/Src/ble.d \
./Core/Src/dma.d \
./Core/Src/eeprom.d \
./Core/Src/gpio.d \
./Core/Src/i2c.d \
./Core/Src/icm20948.d \
./Core/Src/imu.d \
./Core/Src/main.d \
./Core/Src/pwm.d \
./Core/Src/spi.d \
./Core/Src/stm32l0xx_hal_msp.d \
./Core/Src/stm32l0xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32l0xx.d \
./Core/Src/tim.d \
./Core/Src/usart.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -DUSE_HAL_DRIVER -DSTM32L081xx -c -I../Core/Inc -I../Drivers/STM32L0xx_HAL_Driver/Inc -I../Drivers/STM32L0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L0xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/adc.d ./Core/Src/adc.o ./Core/Src/ble.d ./Core/Src/ble.o ./Core/Src/dma.d ./Core/Src/dma.o ./Core/Src/eeprom.d ./Core/Src/eeprom.o ./Core/Src/gpio.d ./Core/Src/gpio.o ./Core/Src/i2c.d ./Core/Src/i2c.o ./Core/Src/icm20948.d ./Core/Src/icm20948.o ./Core/Src/imu.d ./Core/Src/imu.o ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/pwm.d ./Core/Src/pwm.o ./Core/Src/spi.d ./Core/Src/spi.o ./Core/Src/stm32l0xx_hal_msp.d ./Core/Src/stm32l0xx_hal_msp.o ./Core/Src/stm32l0xx_it.d ./Core/Src/stm32l0xx_it.o ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/system_stm32l0xx.d ./Core/Src/system_stm32l0xx.o ./Core/Src/tim.d ./Core/Src/tim.o ./Core/Src/usart.d ./Core/Src/usart.o

.PHONY: clean-Core-2f-Src
