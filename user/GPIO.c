/***********************************************************************
�ļ����ƣ�LED.C
��    �ܣ�led  IO��ʼ��
��дʱ�䣺2013.4.25
�� д �ˣ�
ע    �⣺���´�������ο�
***********************************************************************/
#include "GPIO.h"

#define RCC_APB2ENR         (*((volatile unsigned int*)0x40021018))   // APB2 ����ʱ��ʹ�ܼĴ���
#define GPIOE_CRH           (*((volatile unsigned int*)0x40011804))   // �˿����ø߼Ĵ���
#define GPIOE_BSRR          (*((volatile unsigned int*)0x40011810))   // �˿�λ����/��λ�Ĵ���
#define GPIOE_IDR           (*((volatile unsigned int*)0x40011808))   // �˿��������ݼĴ���	
#define GPIOE_ODR           (*((volatile unsigned int*)0x4001180C))   // �˿�������ݼĴ���
	
#define GPIOB_CRL           (*((volatile unsigned int*)0x40010C00))   // �˿����õͼĴ���
#define GPIOB_IDR           (*((volatile unsigned int*)0x40010C08))   // �˿��������ݼĴ���	
#define GPIOB_BSRR          (*((volatile unsigned int*)0x40010C10))   // �˿�λ����/��λ�Ĵ���	

void LED_Init(void)
{
	RCC_APB2ENR |= 1<<6;          //ʹ��PORTEʱ��	
	
	GPIOE_CRH &=0XFFFFFFF0;       //���PE8����ԭ������  
	GPIOE_CRH |=0x3;			        //����CNF8[1:0]Ϊ0x00��ͨ���������ģʽ��MODE8[1:0]Ϊ0x11�����ģʽ

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
		if (id < 4){ //����
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
		RCC_APB2ENR |=1<<3;    //ʹ��PORTBʱ��	
	  GPIOB_CRL &=~(0xf<<24);// PB.6��������	
		GPIOB_CRL |=(0x08<<24);// PB.6��������	
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
		delay_ms(100);//ȥ���� 
		if(GPIOB_IDR & 0x40)
			key_1=0;

			
		if(key_1==1)
			return 1;
	} 
 	return 0;// �ް�������
}

//********************�������*************************/
int key_map[16];
/*
	{'A','B','C','D'},
	{'1','2','3','E'},
	{'4','5','6','F'},
	{'7','8','9','0'}
*/

int input_key;
bool input_key_flag = false;

// ��ʼ���������
void KEYBOARD_Init(void){
	GPIO_InitTypeDef Keyboard_Init;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE); //ʹ�� PORTEʱ��
	
	Keyboard_Init.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	Keyboard_Init.GPIO_Mode = GPIO_Mode_IPU;
	Keyboard_Init.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &Keyboard_Init); // PE0~3 ��������
	
	Keyboard_Init.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	Keyboard_Init.GPIO_Mode = GPIO_Mode_Out_PP;
	Keyboard_Init.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &Keyboard_Init); // PE4~7 �������

	// ��ʼ������ӳ���
	key_map[0] = 231; 
	key_map[1] = 190;
	key_map[2] = 189;
	key_map[3] = 187;
	key_map[4] = 222;
	key_map[5] = 221;
	key_map[6] = 219;
	key_map[7] = 238;
	key_map[8] = 237; 
	key_map[9] = 235;

	key_map[10] = 126; //A
	key_map[11] = 125; //B
	key_map[12] = 123; //C
	key_map[13] = 119; //D
	key_map[14] = 183; //E
	key_map[15] = 215; //F
}

int which_key(int high,int low){
	int x, i;
	x = (high << 4) | low;
	for (i=0; i<16; i++)
		if (key_map[i] == x)
			return i;
	return -1;
}

int KEYBOARD_cnt = 0;

char KEYBOARD_Scan(void){
	uint32_t PE0_PE3_state;

	if (KEYBOARD_cnt > 0){
		KEYBOARD_cnt--;
		return (u8)-1;
	}

	GPIO_ResetBits(GPIOE, GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7); // 0x0000
	delay_ms(1);
	PE0_PE3_state =  GPIO_ReadInputData(GPIOE) & (GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3); // ������
	if (PE0_PE3_state == 0xf)
		return (u8)-1; // �ް�������

	KEYBOARD_cnt = 50000; // һ��ʱ���ڲ���������

	GPIO_SetBits(GPIOE, GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7); // 0x1110
	delay_ms(1);
	PE0_PE3_state = GPIO_ReadInputData(GPIOE) & (GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3); // ������
	if (PE0_PE3_state != 0xf)
		return which_key(14, PE0_PE3_state);

	GPIO_SetBits(GPIOE, GPIO_Pin_4);
	GPIO_ResetBits(GPIOE, GPIO_Pin_5); // 0x1101
	delay_ms(1);
	PE0_PE3_state = GPIO_ReadInputData(GPIOE) & (GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3); // ������
	if (PE0_PE3_state != 0xf)
		return which_key(13, PE0_PE3_state);

	GPIO_SetBits(GPIOE, GPIO_Pin_5);
	GPIO_ResetBits(GPIOE, GPIO_Pin_6); // 0x1011
	delay_ms(1);
	PE0_PE3_state = GPIO_ReadInputData(GPIOE) & (GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3); // ������
	if (PE0_PE3_state != 0xf)
		return which_key(11,PE0_PE3_state);

	GPIO_SetBits(GPIOE, GPIO_Pin_6);
	GPIO_ResetBits(GPIOE, GPIO_Pin_7); // 0x0111
	delay_ms(1);
	PE0_PE3_state = GPIO_ReadInputData(GPIOE) & (GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3); // ������
	if (PE0_PE3_state != 0xf)
		return which_key(7,PE0_PE3_state);

	return (u8)-1; // ����
}

//********************�����*************************/
int digit_map[17];
int digit_pos = 0;
int digit[4] = {0,1,2,3};
// ��ʼ�������
void DIGIT_Init(void){
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       // �������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;      // ����ٶ�50MHz
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 |
                                 GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       // �������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;      // ����ٶ�50MHz
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    GPIO_SetBits(GPIOC, GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13);
    GPIO_SetBits(GPIOD, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 |
                       GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);

	// ��ʼ������ܱ���ӳ���
	digit_map[0] = 192;
	digit_map[1] = 249; 
	digit_map[2] = 164;
	digit_map[3] = 176;
	digit_map[4] = 153;
	digit_map[5] = 146;
	digit_map[6] = 130;
	digit_map[7] = 248;
	digit_map[8] = 128;
	digit_map[9] = 144;
	digit_map[10] = 136;
	digit_map[11] = 131;
	digit_map[12] = 198;
	digit_map[13] = 161;
	digit_map[14] = 134;
	digit_map[15] = 142;
	digit_map[16] = 127; // ������㣬��ʾ����
}

void DIGIT_display(int digit,int position){
	// �ȹر�����λѡ
    GPIO_SetBits(GPIOC, GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13);
	// ���ö�ѡ
	GPIO_Write(GPIOD, digit_map[digit == -1  ? 16 : digit] & 0xFF);
	// �򿪶�Ӧλѡ
	GPIO_ResetBits(GPIOC, 1 << (10 + position));
}
