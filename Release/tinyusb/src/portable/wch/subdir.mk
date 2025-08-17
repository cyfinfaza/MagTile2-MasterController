################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../tinyusb/src/portable/wch/dcd_ch32_usbfs.c \
../tinyusb/src/portable/wch/dcd_ch32_usbhs.c 

OBJS += \
./tinyusb/src/portable/wch/dcd_ch32_usbfs.o \
./tinyusb/src/portable/wch/dcd_ch32_usbhs.o 

C_DEPS += \
./tinyusb/src/portable/wch/dcd_ch32_usbfs.d \
./tinyusb/src/portable/wch/dcd_ch32_usbhs.d 


# Each subdirectory must supply rules for building sources it contributes
tinyusb/src/portable/wch/%.o tinyusb/src/portable/wch/%.su tinyusb/src/portable/wch/%.cyclo: ../tinyusb/src/portable/wch/%.c tinyusb/src/portable/wch/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32H503xx -c -I../Core/Inc -I"C:/Users/cyfin/Documents/stm32/MagTile2-MasterController/tinyusb/src" -I"C:/Users/cyfin/Documents/stm32/MagTile2-MasterController/tinyusb/hw" -I../Drivers/STM32H5xx_HAL_Driver/Inc -I../Drivers/STM32H5xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H5xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-tinyusb-2f-src-2f-portable-2f-wch

clean-tinyusb-2f-src-2f-portable-2f-wch:
	-$(RM) ./tinyusb/src/portable/wch/dcd_ch32_usbfs.cyclo ./tinyusb/src/portable/wch/dcd_ch32_usbfs.d ./tinyusb/src/portable/wch/dcd_ch32_usbfs.o ./tinyusb/src/portable/wch/dcd_ch32_usbfs.su ./tinyusb/src/portable/wch/dcd_ch32_usbhs.cyclo ./tinyusb/src/portable/wch/dcd_ch32_usbhs.d ./tinyusb/src/portable/wch/dcd_ch32_usbhs.o ./tinyusb/src/portable/wch/dcd_ch32_usbhs.su

.PHONY: clean-tinyusb-2f-src-2f-portable-2f-wch

