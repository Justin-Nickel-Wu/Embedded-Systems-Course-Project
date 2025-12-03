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
#include "stm32f10x_conf.h"
#include "EXTI.h"
#include "GPIO.h"
#include "systick.h"
#include "UART.h"
#include "IIC-lib.h"

void BoardInit() {
    /* System Clocks Configuration */
    SystemInit();
}

int main(void) {
    LED_Init();
    KEY_Init();
    KEYBOARD_Init();
    DIGIT_Init();
    EXTIX_Init();
    systick_init();
    uart1_init();
    IIC_Configuration();

    while (1) {
        IIC_test();

        if (digit_switch_flag) {
            digit_switch_flag = false;
            digit_display_switch();
        }

        if (EXTI_flag) {
            EXTI_KeyBoard_Scan(EXTI_input);
            EXTI_flag = 0;
        }

        if (input_key_flag) {
            digit[3] = digit[2];
            digit[2] = digit[1];
            digit[1] = digit[0];
            digit[0] = input_key;
            input_key_flag = 0;
        }
        RS232_test();
    }
    // return 1;
}

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
