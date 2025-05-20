/*
 * adc.c
 *
 *  Created on: Apr 13, 2025
 *      Author: cyfin
 */


#include "adc.h"
#include "stm32h5xx_hal.h"

uint16_t ADC_DMA_BUFFER[ADC_CHANNELS*2];

// raw readings
uint16_t V_SENSE_HV = 0;
uint16_t I_SENSE_HV = 0;
uint16_t V_SENSE_12 = 0;
uint16_t I_SENSE_12 = 0;
uint16_t V_SENSE_5 = 0;
uint16_t I_SENSE_5 = 0;
uint16_t V_SENSE_HV_IN = 0;
uint16_t V_SENSE_12_IN = 0;
uint16_t TEMP_SENSE = 0;

// calculated values
float v_sense_hv = 0;
float v_sense_12 = 0;
float v_sense_5 = 0;
float i_sense_hv = 0;
float i_sense_12 = 0;
float i_sense_5 = 0;
float v_sense_hv_in = 0;
float v_sense_12_in = 0;
float master_temperature = 0;

extern uint8_t HV_RELAY;

void ADC_Init(ADC_HandleTypeDef* hadc) {
	HAL_ADCEx_Calibration_Start(hadc, ADC_SINGLE_ENDED);
	HAL_ADC_Start_DMA(hadc, (uint32_t*)ADC_DMA_BUFFER, ADC_CHANNELS*2);
}

void ADC_ProcessBuffer(uint16_t* buffer) {
	// read the values from the ADC buffer
	V_SENSE_HV = buffer[0];
	V_SENSE_12 = buffer[1];
	V_SENSE_5 = buffer[2];
	I_SENSE_HV = buffer[3];
	I_SENSE_12 = buffer[4];
	I_SENSE_5 = buffer[5];
	V_SENSE_HV_IN = buffer[6];
	V_SENSE_12_IN = buffer[7];

	// calculate measurements
	v_sense_hv = V_SENSE_HV / 65535.0f * 3.0 * (53.0f / 2.0f);
	v_sense_12 = V_SENSE_12 / 65535.0f * 3.0 * 11.0f;
	v_sense_5 = V_SENSE_5 / 65535.0f * 3.0 * 2.0f;
	i_sense_hv = (I_SENSE_HV / 65535.0f * 3.0 - 1.5) / 0.088f + (HV_RELAY ? 0.6f : 0.0f);
	i_sense_12 = I_SENSE_12 / 65535.0f * 3.0 ;
	i_sense_5 = I_SENSE_5 / 65535.0f * 3.0 / 0.12f;
	v_sense_hv_in = V_SENSE_HV_IN / 65535.0f * 3.0 * (53.0f / 2.0f);
	v_sense_12_in = V_SENSE_12_IN / 65535.0f * 3.0 * 11.0f;
}

void HAL_ADC_HalfConvCpltCallback(ADC_HandleTypeDef* hadc) {
	if (hadc->Instance == ADC1) {
		ADC_ProcessBuffer(&ADC_DMA_BUFFER[0]);
	}
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
	if (hadc->Instance == ADC1) {
		ADC_ProcessBuffer(&ADC_DMA_BUFFER[ADC_CHANNELS]);
	}
}

