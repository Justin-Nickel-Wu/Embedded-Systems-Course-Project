#ifndef __SPI2_H
#define __SPI2_H

#include "stm32f10x_conf.h"

// 拉低 W25Q80 片选引脚
#define W25Q32_CS_LOW() GPIO_ResetBits(GPIOC, GPIO_Pin_3)

// 拉高 W25Q80 片选引脚
#define W25Q32_CS_HIGH() GPIO_SetBits(GPIOC, GPIO_Pin_3)

// 初始化 SPI2
void SPI2_Init(void);

// SPI2 读写一个字节
u8 SPI2_ReadWriteByte(u8 TxData);

// 读取 W25Q80 的 JEDEC ID
void W25Q32_ReadID(u8 *buf);

#endif
