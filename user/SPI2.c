#include "stm32f10x_conf.h"
#include "SPI2.h"

void SPI2_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef SPI_InitStructure;

    /* 1. 使能 GPIOB、GPIOC、SPI2 时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

    /* 2. 配置 PB13 SCK / PB15 MOSI 为复用推挽输出 50MHz */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP; // 复用推挽
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* 3. 配置 PB14 MISO 为浮空输入 */
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 输入浮空
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* 4. 配置 PC3 作为手动片选（CS） */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP; // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_SetBits(GPIOC, GPIO_Pin_3); // CS 拉高（闲时不选中 Flash）

    /* 5. SPI2 配置 */
    SPI_InitStructure.SPI_Direction         = SPI_Direction_2Lines_FullDuplex; // 全双工
    SPI_InitStructure.SPI_Mode              = SPI_Mode_Master;                 // 主机模式
    SPI_InitStructure.SPI_DataSize          = SPI_DataSize_8b;                 // 8位数据
    SPI_InitStructure.SPI_CPOL              = SPI_CPOL_High;                   // 空闲时SCK=1
    SPI_InitStructure.SPI_CPHA              = SPI_CPHA_2Edge;                  // 第二个边沿采样
    SPI_InitStructure.SPI_NSS               = SPI_NSS_Soft;                    // 软件管理 NSS
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;       // 分频 256
    SPI_InitStructure.SPI_FirstBit          = SPI_FirstBit_MSB;                // MSB first
    SPI_InitStructure.SPI_CRCPolynomial     = 7;                               // 默认

    SPI_Init(SPI2, &SPI_InitStructure);

    SPI_Cmd(SPI2, ENABLE); // 使能 SPI2
}

u8 SPI2_ReadWriteByte(u8 TxData) {
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI2, TxData);

    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
    return SPI_I2S_ReceiveData(SPI2);
}

void W25Q32_ReadID(u8 *buf) {
    W25Q32_CS_LOW(); // 拉低 CS，开始通信

    SPI2_ReadWriteByte(0x9F); // JEDEC ID 指令

    buf[0] = SPI2_ReadWriteByte(0xFF); // Manufacturer ID
    buf[1] = SPI2_ReadWriteByte(0xFF); // Memory Type
    buf[2] = SPI2_ReadWriteByte(0xFF); // Capacity

    W25Q32_CS_HIGH(); // 拉高 CS，结束通信
}

u8 W25Q32_ReadSR(void) {
    u8 sr;

    FLASH_CS_LOW();
    SPI2_ReadWriteByte(W25X_ReadStatusReg1);
    sr = SPI2_ReadWriteByte(0xFF);
    FLASH_CS_HIGH();

    return sr;
}

void W25Q32_WaitBusy(void) {
    while (W25Q32_ReadSR() & 0x01); // WIP 位为1表示忙
}

void W25Q32_WriteEnable(void) {
    FLASH_CS_LOW();
    SPI2_ReadWriteByte(W25X_WriteEnable);
    FLASH_CS_HIGH();
}