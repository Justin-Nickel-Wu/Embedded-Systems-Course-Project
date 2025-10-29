/***********************************************************************
文件名称：LED.C
功    能：led  IO初始化
编写时间：2013.4.25
编 写 人：
注    意：以下代码仅供参考
***********************************************************************/
#include "stm32f10x.h"
#include "stm32f10x_gpio.h" 

#define RCC_APB2ENR         (*((volatile unsigned int*)0x40021018))   // APB2 外设时钟使能寄存器
#define GPIOE_CRH           (*((volatile unsigned int*)0x40011804))   // 端口配置高寄存器
#define GPIOE_BSRR          (*((volatile unsigned int*)0x40011810))   // 端口位设置/复位寄存器
#define GPIOE_IDR           (*((volatile unsigned int*)0x40011808))   // 端口输入数据寄存器	
#define GPIOE_ODR           (*((volatile unsigned int*)0x4001180C))   // 端口输出数据寄存器
	
#define GPIOB_CRL           (*((volatile unsigned int*)0x40010C00))   // 端口配置低寄存器
#define GPIOB_IDR           (*((volatile unsigned int*)0x40010C08))   // 端口输入数据寄存器	
#define GPIOB_BSRR          (*((volatile unsigned int*)0x40010C10))   // 端口位设置/复位寄存器	

#define GPIO_PIN_0          (1 << 0)
#define GPIO_PIN_1          (1 << 1)
#define GPIO_PIN_2          (1 << 2)
#define GPIO_PIN_3          (1 << 3)
#define GPIO_PIN_4          (1 << 4)
#define GPIO_PIN_5          (1 << 5)
#define GPIO_PIN_6          (1 << 6)
#define GPIO_PIN_7          (1 << 7)

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
int key_map[16];
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
	key_map[0] = 237; 
	key_map[1] = 126;
	key_map[2] = 125;
	key_map[3] = 123;
	key_map[4] = 190;
	key_map[5] = 189;
	key_map[6] = 187;
	key_map[7] = 222;
	key_map[8] = 221; // 0b01101011
	key_map[9] = 219;  // 0b01011011

	key_map[10] = 126; // 0b01111110 A
	key_map[11] = 123; // 0b01111011 B
	key_map[12] = 57;  // 0b00111001 C
	key_map[13] = 94;  // 0b01011110 D
	key_map[14] = 237; // 0b11101101 *
	key_map[15] = 125; // 0b01111101 #

	// key_map[0b0111][0b0111] = '1';
	// key_map[0b0111][0b1011] = '2';
	// key_map[0b0111][0b1101] = '3';
	// key_map[0b0111][0b1110] = 'A';
	// key_map[0b1011][0b0111] = '4';
	// key_map[0b1011][0b1011] = '5';
	// key_map[0b1011][0b1101] = '6';
	// key_map[0b1011][0b1110] = 'B';
	// key_map[0b1101][0b0111] = '7';
	// key_map[0b1101][0b1011] = '8';
	// key_map[0b1101][0b1101] = '9';
	// key_map[0b1101][0b1110] = 'C';
	// key_map[0b1110][0b0111] = '*';
	// key_map[0b1110][0b1011] = '0';
	// key_map[0b1110][0b1101] = '#';
	// key_map[0b1110][0b1110] = 'D';
}

char which_key(int high,int low){
	int x, ret, i;
	x = high << 4 | low;
	for (i=0; i<16; i++)
		if (key_map[i] == x){
			ret = i;
			break;
		}
	if (i < 10)
		return '0' + ret;
	else
		return 'A' + (ret - 10);
}

char KEYBOARD_Scan(){
	uint32_t PE0_PE3_state;
	GPIOE->ODR &= ~(GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7); // 行置0
	PE0_PE3_state = GPIOE->IDR & (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3); // 读出列
	if (PE0_PE3_state != 0xf)
		return 0; // 无按键按下

	GPIOE->ODR |= (GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7); // 0x1110
	PE0_PE3_state = GPIOE->IDR & (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3); // 读出列
	if (PE0_PE3_state != 0xf)
		return which_key(14, PE0_PE3_state); // 无按键按下
	GPIOE->ODR &= ~(GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7); // 行置0

	GPIOE->ODR |= (GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_7); // 0x1101
	PE0_PE3_state = GPIOE->IDR & (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3); // 读出列
	if (PE0_PE3_state != 0xf)
		return which_key(13, PE0_PE3_state); // 无按键按下
	GPIOE->ODR &= ~(GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_7); // 行置0

	GPIOE->ODR |= (GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7); // 0x1011
	PE0_PE3_state = GPIOE->IDR & (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3); // 读出列
	if (PE0_PE3_state != 0xf)
		return which_key(11,PE0_PE3_state); // 无按键按下
	GPIOE->ODR &= ~(GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7); // 行置0

	GPIOE->ODR |= (GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6); // 0x0111
	PE0_PE3_state = GPIOE->IDR & (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3); // 读出列
	if (PE0_PE3_state != 0xf)
		return which_key(7,PE0_PE3_state); // 无按键按下
	GPIOE->ODR &= ~(GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6); // 行置0

	return 0;
}

//********************数码管*************************/
int digit_map[256];
// 初始化数码管
void DIGIT_Init(void){
	RCC->APB2ENR|=3<<4; //使能 PORTC 和 PORTD 时钟
	GPIOC->CRH&=0xFF0000FF;
	GPIOC->CRH|=0x00333300; //PC.10 11 12 13 推挽输出
	GPIOC->ODR|=15<<10; //PC.10 11 12 13 输出高

	GPIOD->CRL&=0X00000000;
	GPIOD->CRL|=0X33333333;//PD.0 1 2 3 4 5 6 7 推挽输出
	GPIOD->ODR|= 0xFF; //PD.0 1 2 3 4 5 6 7 输出高

	GPIOD->BSRR = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3; //输出高电平
	GPIOD->BSRR = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIOC->BSRR = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;

	// 初始化数码管编码映射表
	digit_map['0'] = 192;  //0b11000000
	digit_map['1'] = 249;  //0b11111001
	digit_map['2'] = 91 ^ 0xff;  //0b01011011
	digit_map['3'] = 79 ^ 0xff;  //0b01001111
	digit_map['4'] = 102 ^ 0xff; //0b01100110
	digit_map['5'] = 109 ^ 0xff; //0b01101101
	digit_map['6'] = 125 ^ 0xff; //0b01111101
	digit_map['7'] = 7 ^ 0xff;   //0b00000111
	digit_map['8'] = 127 ^ 0xff; //0b01111111
	digit_map['9'] = 111 ^ 0xff; //0b01101111

	digit_map['A'] = 119; //0b01110111
	digit_map['B'] = 124; //0b01111100
	digit_map['C'] = 57;  //0b00111001
	digit_map['D'] = 118; //0b01011110
}

void DIGIT_display(char digit,int position){
	// 先关闭所有位选
	GPIOC->ODR|=15<<10; //PC.10 11 12 13 输出高

	// 设置段选
	GPIOD->ODR = (GPIOD->ODR & 0xFFFFFF00) | (digit_map[(int)digit] & 0xFF);

	// 打开对应位选
	GPIOC->ODR &= ~(1 << (10 + position));
}