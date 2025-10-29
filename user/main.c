/**
  ******************************************************************************
  * @file InputCaptureMode/main.c 
  * @author   MCD Application Team
  * @version  V3.0.0
  * @date     04/27/2009
  * @brief    Main program body
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  */ 

/** @addtogroup InputCaptureMode
  * @{
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
extern void LED_Init(void);
extern void LED_Turn(int id);
extern void KEY_Init(void);
extern u8 KEY_Scan(void);
extern void KEYBOARD_Init(void);
extern char KEYBOARD_Scan(void);
extern void DIGIT_Init(void);
extern void DIGIT_display(char digit,int position);

void BoardInit()
{
  /* System Clocks Configuration */
	SystemInit();
}

int main(void)
{

  int tmp, i;
  int pos = 0, cnt = 0;
  int digit[4] = {0,1,2,3};

	LED_Init();
	KEY_Init();
	KEYBOARD_Init();
	DIGIT_Init();
	

	while (1){
		tmp = KEYBOARD_Scan();
    	if (tmp != 0xff){
        digit[3] = digit[2];
        digit[2] = digit[1];
        digit[1] = digit[0];
        digit[0] = tmp;
      }
		if ((++cnt) > 3){
			pos = (pos + 1) % 4;
			cnt = 0;
		}
		DIGIT_display(digit[pos], pos);
	}
	return 1;
}



/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
