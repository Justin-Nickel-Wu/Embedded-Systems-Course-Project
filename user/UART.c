/***********************************************************************
文件名称：LED.C
功    能：led  IO初始化
编写时间：2013.4.25
编 写 人：
注    意：
***********************************************************************/
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "stdio.h"
#include "stdint.h"
#include "UART.h"

void RS232_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
 	USART_InitTypeDef USART_InitStructure; 
	//引脚时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	//串口时钟
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
	
		//串口1初始化
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART1, USART_IT_TC, ENABLE);//发送完成中断
	USART_ClearITPendingBit(USART1, USART_IT_TC);//清除中断TC位
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
	//如果还有其他中断，按照下面类似的增加即可
//	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;	  
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure); 
}
/***********************************************************************
函数名称：void USART1_IRQHandler(void) 
功    能：完成SCI的数据的接收，并做标识
输入参数：
输出参数：
编写时间：2012.11.22
编 写 人：
注    意  RS485用的是USART3.
***********************************************************************/


u8 RS232InData;
#define USART_BUF_LEN  			200  	//定义最大接收字节数 200
u8 USART_Rxbuf[USART_BUF_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
u8 USART_Txbuf[USART_BUF_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
u16 RXPos=0;
u16 FrameFlag = 0;
u16 RecvTimeOver=0;
u16 RecieceFlag = 0;
u16 SendPos,SendBufLen;

void USART1_IRQHandler(void)  
{
		if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(
		{
			RecieceFlag = 1;
			if(RXPos<=USART_BUF_LEN)//缓冲区未满
			{
				USART_Rxbuf[RXPos]=USART_ReceiveData(USART1);//(USART1->DR);	//读取接收到的数据 ;
				RXPos++;
			}
			RecvTimeOver = 10 ;  //每次接收到数据，超时检测时间10ms
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

				//对接收到的数据进行处理
				//可以定义发送数据帧，并启动发送。如果初始化的时候开启USART_IT_TC中断，则没发送一个字节进入中断一次
				//通过进入中断的计数器，确定该发送发送帧中的哪个字节，直到发完为止
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
