#include "stm32f10x_conf.h"
#include "sys.h"
#include "IIC-lib.h"

// Initialize I2C peripheral and GPIO pins
void IIC_Configuration(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    I2C_InitTypeDef I2C_InitStructure;

    // 使能 GPIOB 和 I2C1 时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

    // PB6 = SCL，PB7 = SDA，必须为开漏输出（AF_OD）
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    I2C_DeInit(I2C1);

    // I2C 工作模式，标准 I2C，100kHz
    I2C_InitStructure.I2C_Mode      = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;

    // 本机地址（仅在作为从机时使用，这里主机模式其实不影响）
    I2C_InitStructure.I2C_OwnAddress1 = 0XA0;

    I2C_InitStructure.I2C_Ack                 = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;

    // 通信速率 10kHz（非常慢，一般用 100k）
    I2C_InitStructure.I2C_ClockSpeed = 10000;

    I2C_Init(I2C1, &I2C_InitStructure);
    I2C_Cmd(I2C1, ENABLE);
}

// write data to I2C slave device
uint8_t I2C_Master_BufferWrite(I2C_TypeDef *I2Cx, uint8_t *pBuffer, uint32_t NumByteToWrite, uint8_t SlaveAddress) {
    if (NumByteToWrite == 0)
        return 1;

    // 1. 发送 START
    I2C_GenerateSTART(I2Cx, ENABLE);
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));

    // 2. 发送从机地址 + 写方向
    I2C_Send7bitAddress(I2Cx, SlaveAddress, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    // 3. 逐字节写入数据，等待每个字节发送完成
    while (NumByteToWrite--) {
        I2C_SendData(I2Cx, *pBuffer);
        while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
        pBuffer++;
    }

    // 4. STOP
    I2C_GenerateSTOP(I2Cx, ENABLE);
    while ((I2Cx->CR1 & 0x200) == 0x200); // 等待 STOP 完成

    return 0;
}

// read data from I2C slave device
uint8_t I2C_Master_BufferRead(I2C_TypeDef *I2Cx, uint8_t *pBuffer, uint32_t NumByteToRead, uint8_t SlaveAddress, uint8_t wordaddress) {
    u8 times = 0;

    if (NumByteToRead == 0)
        return 1;

    // 等待总线空闲
    while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY));

    I2C_AcknowledgeConfig(I2Cx, ENABLE);

    // 1. START
    I2C_GenerateSTART(I2Cx, ENABLE);
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));

    // 2. 写模式发送从机地址
    I2C_Send7bitAddress(I2Cx, SlaveAddress, I2C_Direction_Transmitter);

    // 若从机忙（如 EEPROM 写周期），会 NACK，需要重试
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
        times++;
        if (times > 200) {
            I2C_Send7bitAddress(I2Cx, SlaveAddress, I2C_Direction_Transmitter);
            times = 0;
        }
    }

    // 发送 EEPROM word address（内部地址）
    I2C_SendData(I2Cx, wordaddress);
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    // 3. 发送第二次 START（Repeated START），进入读模式
    I2C_GenerateSTART(I2Cx, ENABLE);
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));

    // 4. 改为读方向
    I2C_Send7bitAddress(I2Cx, SlaveAddress, I2C_Direction_Receiver);
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

    // 5. 逐字节接收
    while (NumByteToRead) {
        if (NumByteToRead == 1) {
            // 最后一个字节前关闭 ACK 并发送 STOP
            I2C_AcknowledgeConfig(I2Cx, DISABLE);
            I2C_GenerateSTOP(I2Cx, ENABLE);
        }

        // 等待接收到一个字节数据
        while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED));

        *pBuffer++ = I2C_ReceiveData(I2Cx);
        NumByteToRead--;
    }

    // 恢复 ACK
    I2C_AcknowledgeConfig(I2Cx, ENABLE);
    return 0;
}

unsigned char IICInBuf[20]    = {0};
unsigned char IICWriteBuf[20] = {0, 66, 77, 3, 4, 5, 6, 7, 8, 9, 10};
// IICWriteBuf[0] = word address，从第 1 字节开始才是写入数据

u8 writeflag = 0;

void IIC_test() {
    if (writeflag == 1) {
        // 写 10 字节（包含 word address + 9 字节数据）
        I2C_Master_BufferWrite(I2C1, IICWriteBuf, 10, 0xA0);

        delay_ms(10); // EEPROM 写周期（必须等待，否则会读失败）

        writeflag = 0;
    }

    // 读取 20 字节，从 word address = 0 开始读
    I2C_Master_BufferRead(I2C1, IICInBuf, 20, 0xA0, 0);
}
