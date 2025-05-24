/*
 * uart.c
 *
 *  Created on: May 23, 2025
 *      Author: cyfin
 */


#include "stm32h5xx_hal.h"
#include "uart.h"

UART_HandleTypeDef *huart;

#define UART_BUFFER_SIZE 8

uint8_t uart_rx_buffer[UART_BUFFER_SIZE];

void UART_Init(UART_HandleTypeDef *uart_selection) {
	huart = uart_selection;
	HAL_UARTEx_ReceiveToIdle_IT(huart, uart_rx_buffer, UART_BUFFER_SIZE);
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *incoming_huart, uint16_t Size) {
	if (incoming_huart == huart) {
		// restart the receive process
		HAL_UARTEx_ReceiveToIdle_IT(huart, uart_rx_buffer, UART_BUFFER_SIZE);
	}
}
