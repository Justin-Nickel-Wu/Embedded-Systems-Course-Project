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
#include "chess.h"
#include "lcd.h"
#include "EXTI.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_ICInitTypeDef TIM_ICInitStructure;
TIM_OCInitTypeDef TIM_OCInitStructure;

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
extern u16 FrameFlag;

void Init() {
    SystemInit(); // 初始化系统时钟为72M
    EXTIX_Init();
    RS232_Configuration();
    NVIC_Configuration();
    systick_init();
    LCD_Configuration();
    Touch_Init();
    LCD_Init();
    Touch_Check();

    FrameFlag = 0;
    ShowTouchFlag = 0; // 显示触摸点
    GameResetFlag = 1; // 初始化棋盘
}

int main(void) {
    Init();
    while (1) {
        if (GameResetFlag) {
            u8 buf[5] = {'R', 'E', 'S', 'E', 'T'};
            RS232_SendData(buf, 5); // 发送RESET指令
            drawChessboard();
            GameResetFlag = 0;
        }
        if (WinFlag != 0) continue; // 已经分出胜负，停止一切操作
        selectPiece();
        movePiece();
        changeTurn();
        checkTable();
        showTouch();
        if (FrameFlag != 0) {
            RS232_FrameHandle();
        }
    }
}

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
