/*
 * serial_terminal.h
 *
 *  Created on: Jul 20, 2025
 *      Author: cyfin
 */

#ifndef INC_SERIAL_TERMINAL_H_
#define INC_SERIAL_TERMINAL_H_

#define MAX_CDC_MSG_LEN 128 // Max length for CDC messages

void SerialTerminal_HandleCommand(char* str);

void SerialTerminal_ReplyOk(char* msg);
void SerialTerminal_ReplyError(char* msg);

#endif /* INC_SERIAL_TERMINAL_H_ */
