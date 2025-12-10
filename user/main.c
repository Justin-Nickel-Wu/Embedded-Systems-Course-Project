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
#include "SPI2.h"

void BoardInit() {
    /* System Clocks Configuration */
    SystemInit();
}

int main(void) {
    u8 BUF[5];
    u8 changed = 0;
    u8 test[10];

    LED_Init();
    KEY_Init();
    KEYBOARD_Init();
    DIGIT_Init();
    EXTIX_Init();
    systick_init();
    uart1_init();
    IIC_Configuration();

    SPI2_Init();
    W25Q32_CS_LOW();
    W25Q32_ReadID(test);
    W25Q32_CS_HIGH();

    I2C_Master_BufferRead(I2C1, BUF, 4, 0xA0, 0);
    digit[0] = BUF[0], digit[1] = BUF[1], digit[2] = BUF[2], digit[3] = BUF[3];

    while (1) {
        // IIC_test();
        if (digit_switch_flag) {
            digit_switch_flag = false;
            digit_display_switch();
        }

        if (EXTI_flag) {
            EXTI_KeyBoard_Scan(EXTI_input);
            EXTI_flag = 0;
        }

        if (input_key_flag) {
            changed        = 1;
            digit[3]       = digit[2];
            digit[2]       = digit[1];
            digit[1]       = digit[0];
            digit[0]       = input_key;
            input_key_flag = 0;
        }
        changed |= RS232_test();

        if (changed) {
            BUF[0] = 0, BUF[1] = digit[0], BUF[2] = digit[1], BUF[3] = digit[2], BUF[4] = digit[3];
            I2C_Master_BufferWrite(I2C1, BUF, 5, 0xA0);
            changed = 0;
        }
    }
    // return 1;
}

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
