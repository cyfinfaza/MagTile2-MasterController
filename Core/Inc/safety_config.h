/*
 * safety_config.h
 *
 *  Created on: May 22, 2025
 *      Author: cyfin
 */

#ifndef INC_SAFETY_CONFIG_H_
#define INC_SAFETY_CONFIG_H_

// 5V Rail
#define TH_OV_5 5.2f // V - 5V overvoltage threshold
#define TH_UV_5 4.8f // V - 5V undervoltage threshold
#define TH_OC_5 2.0f // A - 5V overcurrent threshold

// 12V Rail
#define TH_OV_12 15.0f // V - 12V overvoltage threshold
#define TH_UV_12 10.5f // V - 12V undervoltage threshold
#define TH_OC_12 2.0f // A - 12V overcurrent threshold

// HV Rail
#define TH_OV_HV 52.0f // V - HV overvoltage threshold
#define TH_UV_HV -1.0f // V - HV undervoltage threshold
#define TH_OC_HV 5.0f // A - HV overcurrent threshold

// Precharge
#define TH_PRECHG_VDROP 0.05f // *V_SENSE_HV_IN - Max voltage drop across precharge resistor relative to input voltage
#define TH_PRECHG_TIMEOUT 400 // ms - Max time allowed for precharging

// Temperature
#define TH_OVERTEMP_MCU 100.0f // deg C - Max temperature threshold

#endif /* INC_SAFETY_CONFIG_H_ */
