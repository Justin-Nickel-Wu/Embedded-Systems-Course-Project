#include "systick.h"

#include "stm32f10x.h"                  // Device header
#include "stm32f10x_dma.h"
#include "stm32f10x_exti.h"

int TimeSecond = 0;
int Time1msConunt = 0  ;
volatile int time1ms ;

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
	digit_display_switch();
}

void systick_init(void)
{
    RCC_ClocksTypeDef RCC_Clocks;
	
    RCC_GetClocksFreq(&RCC_Clocks);
    SysTick_Config(RCC_Clocks.HCLK_Frequency / digit_display_fps / 4); // 控制的是单个数码管的频率，所以还要除以4
    /* Configure Systick clock source as HCLK */
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
}
