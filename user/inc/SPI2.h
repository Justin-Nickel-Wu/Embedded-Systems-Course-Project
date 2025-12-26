#ifndef __SPI2_H
#define __SPI2_H

#include "stm32f10x_conf.h"

#define W25X_WriteEnable 0x06
#define W25X_WriteDisable 0x04
#define W25X_ReadStatusReg1 0x05
#define W25X_PageProgram 0x02
#define W25X_ReadData 0x03
#define W25X_SectorErase 0x20

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
