/***********************************************************************
�ļ����ƣ�LED.C
��    �ܣ�led  IO��ʼ��
��дʱ�䣺2013.4.25
�� д �ˣ�
ע    �⣺
***********************************************************************/
#include "UART.h"
#include "stm32f10x_conf.h"

void RS232_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
 	USART_InitTypeDef USART_InitStructure; 
	//����ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	//����ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);


  /*
  *  USART1_TX -> PA9 , USART1_RX ->	PA10
  */				
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	         
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_Init(GPIOA, &GPIO_InitStructure);		   

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	        
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
		//����1��ʼ��
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART1, USART_IT_TC, ENABLE);//��������ж�
	USART_ClearITPendingBit(USART1, USART_IT_TC);//����ж�TCλ
	USART_Cmd(USART1, ENABLE);
	
}

void NVIC_Configuration(void)
{
  NVIC_InitTypeDef   NVIC_InitStructure;

  /* Set the Vector Table base location at 0x08000000 */
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);

  /* 2 bit for pre-emption priority, 2 bits for subpriority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
  
  /* Enable the Ethernet global Interrupt */
  
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	//������������жϣ������������Ƶ����Ӽ���
//	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;	  
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure); 
}
/***********************************************************************
�������ƣ�void USART1_IRQHandler(void) 
��    �ܣ����SCI�����ݵĽ��գ�������ʶ
���������
���������
��дʱ�䣺2012.11.22
�� д �ˣ�
ע    ��  RS485�õ���USART3.
***********************************************************************/


u8 RS232InData;
#define USART_BUF_LEN  			200  	//�����������ֽ��� 200
u8 USART_Rxbuf[USART_BUF_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
u8 USART_Txbuf[USART_BUF_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
u16 RXPos=0;
u16 FrameFlag = 0;
u16 RecvTimeOver=0;
u16 RecieceFlag = 0;
u16 SendPos,SendBufLen;

void USART1_IRQHandler(void)  
{
		if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(
		{
			RecieceFlag = 1;
			if(RXPos<=USART_BUF_LEN)//������δ��
			{
				USART_Rxbuf[RXPos]=USART_ReceiveData(USART1);//(USART1->DR);	//��ȡ���յ������� ;
				RXPos++;
			}
			RecvTimeOver = 10 ;  //ÿ�ν��յ����ݣ���ʱ���ʱ��10ms
        } 	
		
		if (USART_GetITStatus(USART1, USART_IT_TC) != RESET) 
		{
				USART_ClearITPendingBit(USART1, USART_IT_TC);           /* Clear the USART transmit interrupt       */
			  if ( SendPos < SendBufLen )
				{
					USART_SendData(USART1,USART_Txbuf[SendPos]);
					SendPos ++ ;
				}

		}	
}


void RS232_test(void)
{
		u16 i;
		if ( FrameFlag != 0 )
		{

				//�Խ��յ������ݽ��д���
				//���Զ��巢������֡�����������͡������ʼ����ʱ����USART_IT_TC�жϣ���û����һ���ֽڽ����ж�һ��
				//ͨ�������жϵļ�������ȷ���÷��ͷ���֡�е��ĸ��ֽڣ�ֱ������Ϊֹ
				RecieceFlag = 0;
				FrameFlag = 0 ;

				SendBufLen = RXPos;
				for (i = 0; i < RXPos; i ++ ){
					USART_Txbuf[i] = USART_Rxbuf[i];
				}
				SendPos = 0 ;
				USART_SendData(USART1, USART_Txbuf[0]);
        		SendPos = 1;

				RXPos = 0 ;
		}
}


void uart1_init(void)	
{
	RS232_Configuration();		
	NVIC_Configuration();

}
