#ifndef __IIC_LIB_H
#define __IIC_LIB_H

#include "stm32f10x_conf.h"

void IIC_Configuration(void);

uint8_t I2C_Master_BufferWrite(I2C_TypeDef *I2Cx, uint8_t *pBuffer, uint32_t NumByteToWrite, uint8_t SlaveAddress);

uint8_t I2C_Master_BufferRead(I2C_TypeDef *I2Cx, uint8_t *pBuffer, uint32_t NumByteToRead, uint8_t SlaveAddress, uint8_t wordaddress);

#endif