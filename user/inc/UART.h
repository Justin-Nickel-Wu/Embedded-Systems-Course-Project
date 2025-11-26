#ifndef UART_H
#define UART_H

extern u16 FrameFlag;
extern u16 RecvTimeOver;
extern u16 RecieceFlag;

void RS232_Configuration(void);
void NVIC_Configuration(void);
void USART1_IRQHandler(void);
u8 RS232_test(void);
void Err(u8 func, u8 info);
u16 Modbus_CRC16(u8 *buf, u16 len);
void Send(void);
void uart1_init(void);

#endif
