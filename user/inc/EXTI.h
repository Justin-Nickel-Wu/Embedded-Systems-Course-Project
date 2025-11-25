#ifndef __EXTI_H__
#define __EXTI_H__

extern int EXTI_input;
extern bool EXTI_flag;

void EXTIX_Init(void);
void update_input_key(int high,int low);
void ResetPE4_7(void);
void EXTI_KeyBoard_Scan(int input);
void EXTI0_IRQHandler(void);
void EXTI1_IRQHandler(void);
void EXTI2_IRQHandler(void);
void EXTI3_IRQHandler(void);

#endif
