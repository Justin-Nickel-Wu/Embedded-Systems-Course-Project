#ifndef UART_H
#define UART_H

extern u16 FrameFlag;
extern u16 RecvTimeOver;
extern u16 RecieceFlag;

void RS232_Configuration(void);
void NVIC_Configuration(void);
void USART1_IRQHandler(void);
void RS232_test(void);
void uart1_init(void);

#endif
