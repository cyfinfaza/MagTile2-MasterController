################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../tinyusb/src/class/net/ecm_rndis_device.c \
../tinyusb/src/class/net/ncm_device.c 

OBJS += \
./tinyusb/src/class/net/ecm_rndis_device.o \
./tinyusb/src/class/net/ncm_device.o 

C_DEPS += \
./tinyusb/src/class/net/ecm_rndis_device.d \
./tinyusb/src/class/net/ncm_device.d 


# Each subdirectory must supply rules for building sources it contributes
tinyusb/src/class/net/%.o tinyusb/src/class/net/%.su tinyusb/src/class/net/%.cyclo: ../tinyusb/src/class/net/%.c tinyusb/src/class/net/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32H503xx -c -I../Core/Inc -I"C:/Users/cyfin/Documents/stm32/MagTile2-MasterController/tinyusb/src" -I"C:/Users/cyfin/Documents/stm32/MagTile2-MasterController/tinyusb/hw" -I../Drivers/STM32H5xx_HAL_Driver/Inc -I../Drivers/STM32H5xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H5xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-tinyusb-2f-src-2f-class-2f-net

clean-tinyusb-2f-src-2f-class-2f-net:
	-$(RM) ./tinyusb/src/class/net/ecm_rndis_device.cyclo ./tinyusb/src/class/net/ecm_rndis_device.d ./tinyusb/src/class/net/ecm_rndis_device.o ./tinyusb/src/class/net/ecm_rndis_device.su ./tinyusb/src/class/net/ncm_device.cyclo ./tinyusb/src/class/net/ncm_device.d ./tinyusb/src/class/net/ncm_device.o ./tinyusb/src/class/net/ncm_device.su

.PHONY: clean-tinyusb-2f-src-2f-class-2f-net

