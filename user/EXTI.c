#include "EXTI.h"

#include "stm32f10x_exti.h"

int EXTI_input;
bool EXTI_flag = 0;

void EXTIX_Init(void)
{
    EXTI_InitTypeDef EXTI_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    
    // 1. ���� PE0-PE3 �� EXTI ӳ��
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource0);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource1);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource2);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource3);

    // 2. ͳһ���� EXTI ����
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;

    // 3. ��ʼ�� EXTI0 - EXTI3
    EXTI_InitStruct.EXTI_Line = EXTI_Line0;
    EXTI_Init(&EXTI_InitStruct);

    EXTI_InitStruct.EXTI_Line = EXTI_Line1;
    EXTI_Init(&EXTI_InitStruct);

    EXTI_InitStruct.EXTI_Line = EXTI_Line2;
    EXTI_Init(&EXTI_InitStruct);

    EXTI_InitStruct.EXTI_Line = EXTI_Line3;
    EXTI_Init(&EXTI_InitStruct);

    // 4. NVIC ���ã�EXTI0-3 �����Լ��� IRQ ͨ����
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;

    NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_Init(&NVIC_InitStruct);

    NVIC_InitStruct.NVIC_IRQChannel = EXTI1_IRQn;
    NVIC_Init(&NVIC_InitStruct);

    NVIC_InitStruct.NVIC_IRQChannel = EXTI2_IRQn;
    NVIC_Init(&NVIC_InitStruct);

    NVIC_InitStruct.NVIC_IRQChannel = EXTI3_IRQn;
    NVIC_Init(&NVIC_InitStruct);
}

void update_input_key(int high,int low){
	int x, i;
	x = (high << 4) | low;
	for (i=0; i<16; i++)
		if (key_map[i] == x){
			input_key = i;
			input_key_flag = true;
			return;
		}
}

void ResetPE4_7(void){
	GPIO_ResetBits(GPIOE, GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);
}

void EXTI_KeyBoard_Scan(int input){
	int reg_state;
	GPIO_SetBits(GPIOE, GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7); // 0x1110
	delay_ms(1);
	reg_state = GPIO_ReadInputDataBit(GPIOE, input);
	if (!reg_state){
		update_input_key(14, 15 ^ input);
		ResetPE4_7();
		return;
	}

	GPIO_SetBits(GPIOE, GPIO_Pin_4);
	GPIO_ResetBits(GPIOE, GPIO_Pin_5); // 0x1101
	delay_ms(1);
	reg_state = GPIO_ReadInputDataBit(GPIOE, input);
	if (!reg_state){
		update_input_key(13, 15 ^ input);
		ResetPE4_7();
		return;
	}

	GPIO_SetBits(GPIOE, GPIO_Pin_5);
	GPIO_ResetBits(GPIOE, GPIO_Pin_6); // 0x1011
	delay_ms(1);
	reg_state = GPIO_ReadInputDataBit(GPIOE, input);
	if (!reg_state){
		update_input_key(11, 15 ^ input);
		ResetPE4_7();
		return;
	}

	GPIO_SetBits(GPIOE, GPIO_Pin_6);
	GPIO_ResetBits(GPIOE, GPIO_Pin_7); // 0x0111
	delay_ms(1);
	reg_state = GPIO_ReadInputDataBit(GPIOE, input);
	if (!reg_state){
		update_input_key(7, 15 ^ input);
		ResetPE4_7();
		return;
	}
	ResetPE4_7();
}

void EXTI0_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line0) != RESET)
    {
        EXTI_input = GPIO_Pin_0;
        EXTI_flag = true;
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}

void EXTI1_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line1) != RESET)
    {
        EXTI_input = GPIO_Pin_1;
        EXTI_flag = true;
        EXTI_ClearITPendingBit(EXTI_Line1);
    }
}

void EXTI2_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line2) != RESET)
    {
        EXTI_input = GPIO_Pin_2;
        EXTI_flag = true;
        EXTI_ClearITPendingBit(EXTI_Line2);
    }
}

void EXTI3_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line3) != RESET)
    {
        EXTI_input = GPIO_Pin_3;
        EXTI_flag = true;
        EXTI_ClearITPendingBit(EXTI_Line3);
    }
}
