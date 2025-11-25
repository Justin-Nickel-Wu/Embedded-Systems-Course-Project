#ifndef __SYSTICK_H__
#define __SYSTICK_H__

#define digit_display_fps 60  // 数码管刷新频率

extern int TimeSecond;
extern int Time1msConunt;
extern volatile int time1ms ;

extern bool digit_switch_flag;

void time_handle(void);
void digit_display_switch(void);
void SysTick_Handler(void);
void systick_init(void);

#endif
