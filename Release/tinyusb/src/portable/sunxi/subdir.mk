################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../tinyusb/src/portable/sunxi/dcd_sunxi_musb.c 

OBJS += \
./tinyusb/src/portable/sunxi/dcd_sunxi_musb.o 

C_DEPS += \
./tinyusb/src/portable/sunxi/dcd_sunxi_musb.d 


# Each subdirectory must supply rules for building sources it contributes
tinyusb/src/portable/sunxi/%.o tinyusb/src/portable/sunxi/%.su tinyusb/src/portable/sunxi/%.cyclo: ../tinyusb/src/portable/sunxi/%.c tinyusb/src/portable/sunxi/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32H503xx -c -I../Core/Inc -I"C:/Users/cyfin/Documents/stm32/MagTile2-MasterController/tinyusb/src" -I"C:/Users/cyfin/Documents/stm32/MagTile2-MasterController/tinyusb/hw" -I../Drivers/STM32H5xx_HAL_Driver/Inc -I../Drivers/STM32H5xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H5xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-tinyusb-2f-src-2f-portable-2f-sunxi

clean-tinyusb-2f-src-2f-portable-2f-sunxi:
	-$(RM) ./tinyusb/src/portable/sunxi/dcd_sunxi_musb.cyclo ./tinyusb/src/portable/sunxi/dcd_sunxi_musb.d ./tinyusb/src/portable/sunxi/dcd_sunxi_musb.o ./tinyusb/src/portable/sunxi/dcd_sunxi_musb.su

.PHONY: clean-tinyusb-2f-src-2f-portable-2f-sunxi

