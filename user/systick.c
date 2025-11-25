#include "systick.h"
#include "UART.h"
#include "GPIO.h"
#include "stm32f10x_conf.h"
#include <stdbool.h>

int TimeSecond = 0;
int Time1msConunt = 0  ;
volatile int time1ms ;

const int digit_switch_cnt_const =  1000 /  (digit_display_fps * 4);
int digit_switch_cnt =  digit_switch_cnt_const;
bool digit_switch_flag = false;

void time_handle(void)
{
		Time1msConunt ++ ;
		if ( Time1msConunt >= 1000)
		{ 
			Time1msConunt -= 1000;
			TimeSecond ++ ;
		}
}

void digit_display_switch(void)
{
  digit_pos = (digit_pos + 1) % 4;
  DIGIT_display(digit[digit_pos], digit_pos);
}

void SysTick_Handler(void)
{
	digit_switch_cnt--;
  if (digit_switch_cnt <= 0)
  {
	  digit_switch_cnt = digit_switch_cnt_const;
    digit_switch_flag = true;
  }

  RecvTimeOver--;
  if (RecvTimeOver <= 0){
      RecvTimeOver = 0;
      FrameFlag = 1;
  }
}

void systick_init(void)
{
    RCC_ClocksTypeDef RCC_Clocks;
	
    RCC_GetClocksFreq(&RCC_Clocks);
    SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000); // 1ms中断一次
    /* Configure Systick clock source as HCLK */
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
}
