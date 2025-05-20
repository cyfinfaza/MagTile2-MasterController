/*
 * adc.h
 *
 *  Created on: Apr 13, 2025
 *      Author: cyfin
 */

#ifndef INC_ADC_H_
#define INC_ADC_H_

#include <stdint.h>
#include "stm32h5xx_hal.h"

#define ADC_CHANNELS 8

// raw readings
extern uint16_t V_SENSE_HV;
extern uint16_t I_SENSE_HV;
extern uint16_t V_SENSE_12;
extern uint16_t I_SENSE_12;
extern uint16_t V_SENSE_5;
extern uint16_t I_SENSE_5;
extern uint16_t V_SENSE_HV_IN;
extern uint16_t V_SENSE_12_IN;
extern uint16_t TEMP_SENSE;

// calculated values
extern float v_sense_hv;
extern float v_sense_12;
extern float v_sense_5;
extern float i_sense_hv;
extern float i_sense_12;
extern float i_sense_5;
extern float v_sense_hv_in;
extern float v_sense_12_in;
extern float master_temperature;

void ADC_Init(ADC_HandleTypeDef* hadc);

#endif /* INC_ADC_H_ */
