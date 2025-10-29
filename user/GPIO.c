/***********************************************************************
文件名称：LED.C
功    能：led  IO初始化
编写时间：2013.4.25
编 写 人：
注    意：以下代码仅供参考
***********************************************************************/
#include "stm32f10x.h"

#define RCC_APB2ENR         (*((volatile unsigned int*)0x40021018))   // APB2 外设时钟使能寄存器
#define GPIOE_CRH           (*((volatile unsigned int*)0x40011804))   // 端口配置高寄存器
#define GPIOE_BSRR          (*((volatile unsigned int*)0x40011810))   // 端口位设置/复位寄存器
#define GPIOE_IDR           (*((volatile unsigned int*)0x40011808))   // 端口输入数据寄存器	
#define GPIOE_ODR           (*((volatile unsigned int*)0x4001180C))   // 端口输出数据寄存器
	
#define GPIOB_CRL           (*((volatile unsigned int*)0x40010C00))   // 端口配置低寄存器
#define GPIOB_IDR           (*((volatile unsigned int*)0x40010C08))   // 端口输入数据寄存器	
#define GPIOB_BSRR          (*((volatile unsigned int*)0x40010C10))   // 端口位设置/复位寄存器	

void LED_Init(void)
{
	RCC_APB2ENR |= 1<<6;          //使能PORTE时钟	
	
	GPIOE_CRH &=0XFFFFFFF0;       //清除PE8引脚原来设置  
	GPIOE_CRH |=0x3;			        //设置CNF8[1:0]为0x00：通用推挽输出模式，MODE8[1:0]为0x11：输出模式

	GPIOE_CRH &= 0xFFFFFF0F;
	GPIOE_CRH |= 0x00000030;

	GPIOE_CRH &= 0xFFFFF0FF;
	GPIOE_CRH |= 0x00000300;

	GPIOE_CRH &= 0xFFFF0FFF;
	GPIOE_CRH |= 0x00003000; 
	
	GPIOE->BSRR = (1<<8) | (1<<9) | (1<<10) | (1<<11);
}

void Delay(unsigned int nCount)
{ 
  while(nCount > 0)
  { 
  	  nCount --;   
  }
}

void LED_Turn(int id)
{
		if (id < 4){ //单灯
			GPIOE_ODR &= ~(1 << (8 + id));
			Delay(0xfffff);
			Delay(0xfffff);
			
			GPIOE_ODR |= (1 << (8 + id));
			Delay(0xfffff);
			Delay(0xfffff);
		} else if (id == 4){
			int i;
			for (i=0; i<4; i++){
				GPIOE_ODR &= ~(1 << (8 + i));
				Delay(0xfffff);
			
				GPIOE_ODR |= (1 << (8 + i));
				Delay(0xfffff);
			}
		}
}

void KEY_Init(void)
{
		RCC_APB2ENR |=1<<3;    //使能PORTB时钟	
	  GPIOB_CRL &=~(0xf<<24);// PB.6上拉输入	
		GPIOB_CRL |=(0x08<<24);// PB.6上拉输入	
	  //GPIOB_ODR |= (1<<6);
		GPIOB_BSRR = (1<< 6);
}

u8 KEY_Scan(void)
{	 

	u8 key_1= 0;
	if(GPIOB_IDR & 0x40)
			key_1=0;
	else 
  		key_1=1;
	if(key_1==1)
	{
		delay_ms(100);//去抖动 
		if(GPIOB_IDR & 0x40)
			key_1=0;

			
		if(key_1==1)
			return 1;
	} 
 	return 0;// 无按键按下
}

//********************矩阵键盘*************************/
char key_map[16][16];
/*
	{'1','2','3','A'},
	{'4','5','6','B'},
	{'7','8','9','C'},
	{'*','0','#','D'}
*/

// 初始化矩阵键盘
void KEYBOARD_Init(void){
	RCC->APB2ENR |= 1<<6; //使能 PORTE时钟
	GPIOE->CRL = 0X33338888; //PE.0 1 2 3 上拉输入 4 5 6 7 推挽输出

	// 初始化按键映射表
	key_map[0b0111][0b0111] = '1', key_map[0b0111][0b1011] = '2', key_map[0b0111][0b1101] = '3', key_map[0b0111][0b1110] = 'A';
	key_map[0b1011][0b0111] = '4', key_map[0b1011][0b1011] = '5', key_map[0b1011][0b1101] = '6', key_map[0b1011][0b1110] = 'B';
	key_map[0b1101][0b0111] = '7', key_map[0b1101][0b1011] = '8', key_map[0b1101][0b1101] = '9', key_map[0b1101][0b1110] = 'C';
	key_map[0b1110][0b0111] = '*', key_map[0b1110][0b1011] = '0', key_map[0b1110][0b1101] = '#', key_map[0b1110][0b1110] = 'D';
}

char which_key(int high,int low){
	return key_map[high][low];
}

char KEYBOARD_Scan(){
	uint32_t PE0_PE3_state;
	GPIOE->ODR &= ~(GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7); // 行置0
	PE0_PE3_state = GPIOE->IDR & (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3); // 读出列
	if (PE0_PE3_state != 0xf)
		return 0; // 无按键按下

	GPIOE->ODR |= (GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7); // 0x1110
	PE0_PE3_state = GPIOE->IDR & (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3); // 读出列
	if (PE0_PE3_state != 0b1111)
		return which_key(0b1110, PE0_PE3_state); // 无按键按下
	GPIOE->ODR &= ~(GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7); // 行置0

	GPIOE->ODR |= (GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_7); // 0x1101
	PE0_PE3_state = GPIOE->IDR & (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3); // 读出列
	if (PE0_PE3_state != 0b1111)
		return which_key(0b1101, PE0_PE3_state); // 无按键按下
	GPIOE->ODR &= ~(GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_7); // 行置0

	GPIOE->ODR |= (GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7); // 0x1011
	PE0_PE3_state = GPIOE->IDR & (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3); // 读出列
	if (PE0_PE3_state != 0b1111)
		return which_key(0b1011,PE0_PE3_state); // 无按键按下
	GPIOE->ODR &= ~(GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7); // 行置0

	GPIOE->ODR |= (GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6); // 0x0111
	PE0_PE3_state = GPIOE->IDR & (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3); // 读出列
	if (PE0_PE3_state != 0b1111)
		return which_key(0b0111,PE0_PE3_state); // 无按键按下
	GPIOE-ODR &= ~(GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6); // 行置0

	return 0;
}