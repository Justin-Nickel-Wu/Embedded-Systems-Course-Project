#ifndef __SYSTICK_H__
#define __SYSTICK_H__

#include "GPIO.h"
#include "stm32f10x.h"

extern int TimeSecond;
extern int Time1msConunt;
extern volatile int time1ms ;

#define digit_display_fps 60

void time_handle(void);
void digit_display_switch(void);
void SysTick_Handler(void);
void systick_init(void);

#endif
