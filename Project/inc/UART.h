#ifndef __UART_H
#define __UART_H

#include "stm32f10x.h"

extern u16 FrameFlag;
void RS232_SendData(u8 *buf, u16 len);

#endif