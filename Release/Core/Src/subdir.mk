################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/adc.c \
../Core/Src/can.c \
../Core/Src/cobs.c \
../Core/Src/i2c_master.c \
../Core/Src/main.c \
../Core/Src/registry.c \
../Core/Src/registry_map.c \
../Core/Src/reporter.c \
../Core/Src/serial_terminal.c \
../Core/Src/stm32h5xx_hal_msp.c \
../Core/Src/stm32h5xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32h5xx.c \
../Core/Src/tile_data.c \
../Core/Src/uart.c \
../Core/Src/usb_descriptors.c \
../Core/Src/usb_rx.c 

OBJS += \
./Core/Src/adc.o \
./Core/Src/can.o \
./Core/Src/cobs.o \
./Core/Src/i2c_master.o \
./Core/Src/main.o \
./Core/Src/registry.o \
./Core/Src/registry_map.o \
./Core/Src/reporter.o \
./Core/Src/serial_terminal.o \
./Core/Src/stm32h5xx_hal_msp.o \
./Core/Src/stm32h5xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32h5xx.o \
./Core/Src/tile_data.o \
./Core/Src/uart.o \
./Core/Src/usb_descriptors.o \
./Core/Src/usb_rx.o 

C_DEPS += \
./Core/Src/adc.d \
./Core/Src/can.d \
./Core/Src/cobs.d \
./Core/Src/i2c_master.d \
./Core/Src/main.d \
./Core/Src/registry.d \
./Core/Src/registry_map.d \
./Core/Src/reporter.d \
./Core/Src/serial_terminal.d \
./Core/Src/stm32h5xx_hal_msp.d \
./Core/Src/stm32h5xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32h5xx.d \
./Core/Src/tile_data.d \
./Core/Src/uart.d \
./Core/Src/usb_descriptors.d \
./Core/Src/usb_rx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32H503xx -c -I../Core/Inc -I"C:/Users/cyfin/Documents/stm32/MagTile2-MasterController/tinyusb/src" -I"C:/Users/cyfin/Documents/stm32/MagTile2-MasterController/tinyusb/hw" -I../Drivers/STM32H5xx_HAL_Driver/Inc -I../Drivers/STM32H5xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H5xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/adc.cyclo ./Core/Src/adc.d ./Core/Src/adc.o ./Core/Src/adc.su ./Core/Src/can.cyclo ./Core/Src/can.d ./Core/Src/can.o ./Core/Src/can.su ./Core/Src/cobs.cyclo ./Core/Src/cobs.d ./Core/Src/cobs.o ./Core/Src/cobs.su ./Core/Src/i2c_master.cyclo ./Core/Src/i2c_master.d ./Core/Src/i2c_master.o ./Core/Src/i2c_master.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/registry.cyclo ./Core/Src/registry.d ./Core/Src/registry.o ./Core/Src/registry.su ./Core/Src/registry_map.cyclo ./Core/Src/registry_map.d ./Core/Src/registry_map.o ./Core/Src/registry_map.su ./Core/Src/reporter.cyclo ./Core/Src/reporter.d ./Core/Src/reporter.o ./Core/Src/reporter.su ./Core/Src/serial_terminal.cyclo ./Core/Src/serial_terminal.d ./Core/Src/serial_terminal.o ./Core/Src/serial_terminal.su ./Core/Src/stm32h5xx_hal_msp.cyclo ./Core/Src/stm32h5xx_hal_msp.d ./Core/Src/stm32h5xx_hal_msp.o ./Core/Src/stm32h5xx_hal_msp.su ./Core/Src/stm32h5xx_it.cyclo ./Core/Src/stm32h5xx_it.d ./Core/Src/stm32h5xx_it.o ./Core/Src/stm32h5xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32h5xx.cyclo ./Core/Src/system_stm32h5xx.d ./Core/Src/system_stm32h5xx.o ./Core/Src/system_stm32h5xx.su ./Core/Src/tile_data.cyclo ./Core/Src/tile_data.d ./Core/Src/tile_data.o ./Core/Src/tile_data.su ./Core/Src/uart.cyclo ./Core/Src/uart.d ./Core/Src/uart.o ./Core/Src/uart.su ./Core/Src/usb_descriptors.cyclo ./Core/Src/usb_descriptors.d ./Core/Src/usb_descriptors.o ./Core/Src/usb_descriptors.su ./Core/Src/usb_rx.cyclo ./Core/Src/usb_rx.d ./Core/Src/usb_rx.o ./Core/Src/usb_rx.su

.PHONY: clean-Core-2f-Src

