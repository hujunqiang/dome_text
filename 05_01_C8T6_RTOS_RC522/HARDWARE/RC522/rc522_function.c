#include "rc522_function.h"

#define macRC522_DELAY() delay_xus(200)

uint8_t UID[5], Temp[4];
uint8_t RF_Buffer[18];
uint8_t Password_Buffer[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // Mifare One 缺省密码

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define KEYA 0x60
#define KEYB 0x61
/*全局变量*/
unsigned char CT[2];    // 卡类型
unsigned char SN[4];    // 卡号
unsigned char DATA[16]; // 存放数据
unsigned char RFID[16]; // 存放RFID
// unsigned char card0_bit = 0;
unsigned char card1_bit = 0;
unsigned char card2_bit = 0;
unsigned char card3_bit = 0;
// unsigned char card4_bit = 0;
unsigned char total = 0;
// 这UID定义在这不知道干啥用的。。。 替换成自己卡的UID
unsigned char card_1[4] = {118, 254, 211, 48};
unsigned char card_2[4] = {198, 183, 205, 48};
unsigned char card_3[4] = {102, 50, 213, 48};

u8 KEY_A[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
u8 KEY_B[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
u8 AUDIO_OPEN[6] = {0xAA, 0x07, 0x02, 0x00, 0x09, 0xBC};
// (测试用 )   3区块数据
unsigned char RFID1[16] = {0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0xff, 0x07, 0x80, 0x29, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
unsigned char RFID2[16] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x07, 0x80, 0x29, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
//(测试用 )    3区块密钥
u8 KEY_A1[6] = {0x10, 0x20, 0x30, 0x40, 0x50, 0x60};
u8 KEY_A2[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

u8 KEY_B1[6] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
u8 KEY_B2[6] = {0x10, 0x20, 0x30, 0x00, 0x00, 0x00};
u8 KEY_B3[6] = {0x01, 0x02, 0x03, 0x00, 0x00, 0x00};
// 置零用
unsigned char DATA0[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
unsigned char DATA1[16] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0x00, 0xff, 0x07, 0x80, 0x29, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
unsigned char DATA_A[16] = {0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
unsigned char DATA_B[1] = {0x42};
unsigned char DATA_C[1] = {0x43};
unsigned char status;
// 0x08   就是2扇区  (0区块)（即第9块）
unsigned char addr = 0x08;
// unsigned char addr=0x08;

#define RC522_DELAY() delay_xus(20)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * 函数名：SPI_RC522_SendByte
 * 描述  ：向RC522发送1 Byte 数据
 * 输入  ：byte，要发送的数据
 * 返回  : RC522返回的数据
 * 调用  ：内部调用
 */
void SPI_RC522_SendByte(u8 byte)
{
    u8 counter;
    for (counter = 0; counter < 8; counter++)
    {
        if (byte & 0x80)
            macRC522_MOSI_1();
        else
            macRC522_MOSI_0();
        // delay_xus(3);
        macRC522_DELAY();
        macRC522_SCK_0();
        // delay_xus(1);
        // delay_xus(3);
        macRC522_DELAY();
        macRC522_SCK_1();
        // delay_xus(3);
        macRC522_DELAY();
        byte <<= 1;
    }
}

/*
 * 函数名：SPI_RC522_ReadByte
 * 描述  ：从RC522发送1 Byte 数据
 * 输入  ：无
 * 返回  : RC522返回的数据
 * 调用  ：内部调用
 */
u8 SPI_RC522_ReadByte(void)
{
    u8 counter;
    u8 SPI_Data;
    for (counter = 0; counter < 8; counter++)
    {
        SPI_Data <<= 1;
        macRC522_SCK_0();
        // delay_xus(3);
        macRC522_DELAY();
        if (macRC522_MISO_GET() == 1)
            SPI_Data |= 0x01;
        // delay_xus(2);
        // delay_xus(3);
        macRC522_DELAY();
        macRC522_SCK_1();
        // delay_xus(3);
        macRC522_DELAY();
    }
    return SPI_Data;
}

/*
 * 函数名：ReadRawRC
 * 描述  ：读RC522寄存器
 * 输入  ：ucAddress，寄存器地址
 * 返回  : 寄存器的当前值
 * 调用  ：内部调用
 */
u8 ReadRawRC(u8 ucAddress)
{
    u8 ucAddr, ucReturn;
    ucAddr = ((ucAddress << 1) & 0x7E) | 0x80;
    macRC522_CS_Enable();
    SPI_RC522_SendByte(ucAddr);
    ucReturn = SPI_RC522_ReadByte();
    macRC522_CS_Disable();
    return ucReturn;
}

/*
 * 函数名：WriteRawRC
 * 描述  ：写RC522寄存器
 * 输入  ：ucAddress，寄存器地址
 *         ucValue，写入寄存器的值
 * 返回  : 无
 * 调用  ：内部调用
 */
void WriteRawRC(u8 ucAddress, u8 ucValue)
{
    u8 ucAddr;
    ucAddr = (ucAddress << 1) & 0x7E;
    macRC522_CS_Enable();
    SPI_RC522_SendByte(ucAddr);
    SPI_RC522_SendByte(ucValue);
    macRC522_CS_Disable();
}

/*
 * 函数名：SetBitMask
 * 描述  ：对RC522寄存器置位
 * 输入  ：ucReg，寄存器地址
 *         ucMask，置位值
 * 返回  : 无
 * 调用  ：内部调用
 */
void SetBitMask(u8 ucReg, u8 ucMask)
{
    u8 ucTemp;
    ucTemp = ReadRawRC(ucReg);
    WriteRawRC(ucReg, ucTemp | ucMask); // set bit mask
}

/*
 * 函数名：ClearBitMask
 * 描述  ：对RC522寄存器清位
 * 输入  ：ucReg，寄存器地址
 *         ucMask，清位值
 * 返回  : 无
 * 调用  ：内部调用
 */
void ClearBitMask(u8 ucReg, u8 ucMask)
{
    u8 ucTemp;
    ucTemp = ReadRawRC(ucReg);
    WriteRawRC(ucReg, ucTemp & (~ucMask)); // clear bit mask
}

/*
 * 函数名：PcdAntennaOn
 * 描述  ：开启天线
 * 输入  ：无
 * 返回  : 无
 * 调用  ：内部调用
 */
void PcdAntennaOn(void)
{
    u8 uc;
    uc = ReadRawRC(TxControlReg);
    if (!(uc & 0x03))
        SetBitMask(TxControlReg, 0x03);
}

/*
 * 函数名：PcdAntennaOff
 * 描述  ：开启天线
 * 输入  ：无
 * 返回  : 无
 * 调用  ：内部调用
 */
void PcdAntennaOff(void)
{
    ClearBitMask(TxControlReg, 0x03);
}

/*
 * 函数名：PcdRese
 * 描述  ：复位RC522
 * 输入  ：无
 * 返回  : 无
 * 调用  ：外部调用
 */
void PcdReset(void)
{
    macRC522_Reset_Disable();
    delay_xus(1);
    macRC522_Reset_Enable();
    delay_xus(1);
    macRC522_Reset_Disable();
    delay_xus(1);
    WriteRawRC(CommandReg, 0x0f);
    while (ReadRawRC(CommandReg) & 0x10)
        ;
    delay_xus(1);
    WriteRawRC(ModeReg, 0x3D);       // 定义发送和接收常用模式 和Mifare卡通讯，CRC初始值0x6363
    WriteRawRC(TReloadRegL, 30);     // 16位定时器低位
    WriteRawRC(TReloadRegH, 0);      // 16位定时器高位
    WriteRawRC(TModeReg, 0x8D);      // 定义内部定时器的设置
    WriteRawRC(TPrescalerReg, 0x3E); // 设置定时器分频系数
    WriteRawRC(TxAutoReg, 0x40);     // 调制发送信号为100%ASK
}

/*
 * 函数名：M500PcdConfigISOType
 * 描述  ：设置RC522的工作方式
 * 输入  ：ucType，工作方式
 * 返回  : 无
 * 调用  ：外部调用
 */
void M500PcdConfigISOType(u8 ucType)
{
    if (ucType == 'A') // ISO14443_A
    {
        ClearBitMask(Status2Reg, 0x08);
        WriteRawRC(ModeReg, 0x3D);   // 3F
        WriteRawRC(RxSelReg, 0x86);  // 84
        WriteRawRC(RFCfgReg, 0x7F);  // 4F
        WriteRawRC(TReloadRegL, 30); // tmoLength);// TReloadVal = 'h6a =tmoLength(dec)
        WriteRawRC(TReloadRegH, 0);
        WriteRawRC(TModeReg, 0x8D);
        WriteRawRC(TPrescalerReg, 0x3E);
        delay_xus(2);
        PcdAntennaOn(); // 开天线
    }
}

/*
 * 函数名：PcdComMF522
 * 描述  ：通过RC522和ISO14443卡通讯
 * 输入  ：ucCommand，RC522命令字
 *         pInData，通过RC522发送到卡片的数据
 *         ucInLenByte，发送数据的字节长度
 *         pOutData，接收到的卡片返回数据
 *         pOutLenBit，返回数据的位长度
 * 返回  : 状态值
 *         = MI_OK，成功
 * 调用  ：内部调用
 */
char PcdComMF522(u8 ucCommand, u8 *pInData, u8 ucInLenByte, u8 *pOutData, u32 *pOutLenBit)
{
    char cStatus = MI_ERR;
    u8 ucIrqEn = 0x00;
    u8 ucWaitFor = 0x00;
    u8 ucLastBits;
    u8 ucN;
    u32 ul;
    switch (ucCommand)
    {
    case PCD_AUTHENT:     // Mifare认证
        ucIrqEn = 0x12;   // 允许错误中断请求ErrIEn  允许空闲中断IdleIEn
        ucWaitFor = 0x10; // 认证寻卡等待时候 查询空闲中断标志位
        break;

    case PCD_TRANSCEIVE:  // 接收发送 发送接收
        ucIrqEn = 0x77;   // 允许TxIEn RxIEn IdleIEn LoAlertIEn ErrIEn TimerIEn
        ucWaitFor = 0x30; // 寻卡等待时候 查询接收中断标志位与 空闲中断标志位
        break;

    default:
        break;
    }
    WriteRawRC(ComIEnReg, ucIrqEn | 0x80); // IRqInv置位管脚IRQ与Status1Reg的IRq位的值相反
    ClearBitMask(ComIrqReg, 0x80);         // Set1该位清零时，CommIRqReg的屏蔽位清零
    WriteRawRC(CommandReg, PCD_IDLE);      // 写空闲命令
    SetBitMask(FIFOLevelReg, 0x80);        // 置位FlushBuffer清除内部FIFO的读和写指针以及ErrReg的BufferOvfl标志位被清除
    for (ul = 0; ul < ucInLenByte; ul++)
        WriteRawRC(FIFODataReg, pInData[ul]); // 写数据进FIFOdata
    WriteRawRC(CommandReg, ucCommand);        // 写命令
    if (ucCommand == PCD_TRANSCEIVE)
        SetBitMask(BitFramingReg, 0x80); // StartSend置位启动数据发送 该位与收发命令使用时才有效
    ul = 1000;                           // 根据时钟频率调整，操作M1卡最大等待时间25ms
    do                                   // 认证 与寻卡等待时间
    {
        ucN = ReadRawRC(ComIrqReg); // 查询事件中断
        ul--;
    } while ((ul != 0) && (!(ucN & 0x01)) && (!(ucN & ucWaitFor))); // 退出条件i=0,定时器中断，与写空闲命令
    ClearBitMask(BitFramingReg, 0x80);                              // 清理允许StartSend位
    if (ul != 0)
    {
        if (!(ReadRawRC(ErrorReg) & 0x1B)) // 读错误标志寄存器BufferOfI CollErr ParityErr ProtocolErr
        {
            cStatus = MI_OK;
            if (ucN & ucIrqEn & 0x01) // 是否发生定时器中断
                cStatus = MI_NOTAGERR;
            if (ucCommand == PCD_TRANSCEIVE)
            {
                ucN = ReadRawRC(FIFOLevelReg);             // 读FIFO中保存的字节数
                ucLastBits = ReadRawRC(ControlReg) & 0x07; // 最后接收到得字节的有效位数
                if (ucLastBits)
                    *pOutLenBit = (ucN - 1) * 8 + ucLastBits; // N个字节数减去1（最后一个字节）+最后一位的位数 读取到的数据总位数
                else
                    *pOutLenBit = ucN * 8; // 最后接收到的字节整个字节有效
                if (ucN == 0)
                    ucN = 1;
                if (ucN > MAXRLEN)
                    ucN = MAXRLEN;

                for (ul = 0; ul < ucN; ul++)
                    pOutData[ul] = ReadRawRC(FIFODataReg);
            }
        }
        else
            cStatus = MI_ERR;
    }
    SetBitMask(ControlReg, 0x80); // stop timer now
    WriteRawRC(CommandReg, PCD_IDLE);
    return cStatus;
}

/*
 * 函数名：PcdRequest
 * 描述  ：寻卡
 * 输入  ：ucReq_code，寻卡方式
 *                     = 0x52，寻感应区内所有符合14443A标准的卡
 *                     = 0x26，寻未进入休眠状态的卡
 *         pTagType，卡片类型代码
 *                   = 0x4400，Mifare_UltraLight
 *                   = 0x0400，Mifare_One(S50)
 *                   = 0x0200，Mifare_One(S70)
 *                   = 0x0800，Mifare_Pro(X))
 *                   = 0x4403，Mifare_DESFire
 * 返回  : 状态值
 *         = MI_OK，成功
 * 调用  ：外部调用
 */
char PcdRequest(u8 ucReq_code, u8 *pTagType)
{
    char cStatus;
    u8 ucComMF522Buf[MAXRLEN];
    u32 ulLen;
    ClearBitMask(Status2Reg, 0x08);                                                 // 清理指示MIFARECyptol单元接通以及所有卡的数据通信被加密的情况
    WriteRawRC(BitFramingReg, 0x07);                                                //	发送的最后一个字节的 七位
    SetBitMask(TxControlReg, 0x03);                                                 // TX1,TX2管脚的输出信号传递经发送调制的13.56的能量载波信号
    ucComMF522Buf[0] = ucReq_code;                                                  // 存入 卡片命令字
    cStatus = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 1, ucComMF522Buf, &ulLen); // 寻卡
    if ((cStatus == MI_OK) && (ulLen == 0x10))                                      // 寻卡成功返回卡类型
    {
        *pTagType = ucComMF522Buf[0];
        *(pTagType + 1) = ucComMF522Buf[1];
    }
    else
        cStatus = MI_ERR;
    return cStatus;
}

/*
 * 函数名：PcdAnticoll
 * 描述  ：防冲撞
 * 输入  ：pSnr，卡片序列号，4字节
 * 返回  : 状态值
 *         = MI_OK，成功
 * 调用  ：外部调用
 */
char PcdAnticoll(u8 *pSnr)
{
    char cStatus;
    u8 uc, ucSnr_check = 0;
    u8 ucComMF522Buf[MAXRLEN];
    u32 ulLen;
    ClearBitMask(Status2Reg, 0x08);  // 清MFCryptol On位 只有成功执行MFAuthent命令后，该位才能置位
    WriteRawRC(BitFramingReg, 0x00); // 清理寄存器 停止收发
    ClearBitMask(CollReg, 0x80);     // 清ValuesAfterColl所有接收的位在冲突后被清除
    ucComMF522Buf[0] = 0x93;         // 卡片防冲突命令
    ucComMF522Buf[1] = 0x20;
    cStatus = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 2, ucComMF522Buf, &ulLen); // 与卡片通信
    if (cStatus == MI_OK)                                                           // 通信成功
    {
        for (uc = 0; uc < 4; uc++)
        {
            *(pSnr + uc) = ucComMF522Buf[uc]; // 读出UID
            ucSnr_check ^= ucComMF522Buf[uc];
        }
        if (ucSnr_check != ucComMF522Buf[uc])
            cStatus = MI_ERR;
    }
    SetBitMask(CollReg, 0x80);
    return cStatus;
}

/*
 * 函数名：CalulateCRC
 * 描述  ：用RC522计算CRC16
 * 输入  ：pIndata，计算CRC16的数组
 *         ucLen，计算CRC16的数组字节长度
 *         pOutData，存放计算结果存放的首地址
 * 返回  : 无
 * 调用  ：内部调用
 */
void CalulateCRC(u8 *pIndata, u8 ucLen, u8 *pOutData)
{
    u8 uc, ucN;
    ClearBitMask(DivIrqReg, 0x04);
    WriteRawRC(CommandReg, PCD_IDLE);
    SetBitMask(FIFOLevelReg, 0x80);
    for (uc = 0; uc < ucLen; uc++)
        WriteRawRC(FIFODataReg, *(pIndata + uc));
    WriteRawRC(CommandReg, PCD_CALCCRC);
    uc = 0xFF;
    do
    {
        ucN = ReadRawRC(DivIrqReg);
        uc--;
    } while ((uc != 0) && !(ucN & 0x04));
    pOutData[0] = ReadRawRC(CRCResultRegL);
    pOutData[1] = ReadRawRC(CRCResultRegM);
}

/*
 * 函数名：PcdSelect
 * 描述  ：选定卡片
 * 输入  ：pSnr，卡片序列号，4字节
 * 返回  : 状态值
 *         = MI_OK，成功
 * 调用  ：外部调用
 */
char PcdSelect(u8 *pSnr)
{
    char ucN;
    u8 uc;
    u8 ucComMF522Buf[MAXRLEN];
    u32 ulLen;
    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x70;
    ucComMF522Buf[6] = 0;
    for (uc = 0; uc < 4; uc++)
    {
        ucComMF522Buf[uc + 2] = *(pSnr + uc);
        ucComMF522Buf[6] ^= *(pSnr + uc);
    }
    CalulateCRC(ucComMF522Buf, 7, &ucComMF522Buf[7]);
    ClearBitMask(Status2Reg, 0x08);
    ucN = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 9, ucComMF522Buf, &ulLen);
    if ((ucN == MI_OK) && (ulLen == 0x18))
        ucN = MI_OK;
    else
        ucN = MI_ERR;
    return ucN;
}

/*
 * 函数名：PcdAuthState
 * 描述  ：验证卡片密码
 * 输入  ：ucAuth_mode，密码验证模式
 *                     = 0x60，验证A密钥
 *                     = 0x61，验证B密钥
 *         u8 ucAddr，块地址
 *         pKey，密码
 *         pSnr，卡片序列号，4字节
 * 返回  : 状态值
 *         = MI_OK，成功
 * 调用  ：外部调用
 */
char PcdAuthState(u8 ucAuth_mode, u8 ucAddr, u8 *pKey, u8 *pSnr)
{
    char cStatus;
    u8 uc, ucComMF522Buf[MAXRLEN];
    u32 ulLen;
    ucComMF522Buf[0] = ucAuth_mode;
    ucComMF522Buf[1] = ucAddr;
    for (uc = 0; uc < 6; uc++)
        ucComMF522Buf[uc + 2] = *(pKey + uc);
    for (uc = 0; uc < 6; uc++)
        ucComMF522Buf[uc + 8] = *(pSnr + uc);
    cStatus = PcdComMF522(PCD_AUTHENT, ucComMF522Buf, 12, ucComMF522Buf, &ulLen);

    if ((cStatus != MI_OK) || (!(ReadRawRC(Status2Reg) & 0x08)))
        cStatus = MI_ERR;
    return cStatus;
}

/*
 * 函数名：PcdWrite
 * 描述  ：写数据到M1卡一块
 * 输入  ：u8 ucAddr，块地址
 *         pData，写入的数据，16字节
 * 返回  : 状态值
 *         = MI_OK，成功
 * 调用  ：外部调用
 */
char PcdWrite(u8 ucAddr, u8 *pData)
{
    char cStatus;
    u8 uc, ucComMF522Buf[MAXRLEN];
    u32 ulLen;

    ucComMF522Buf[0] = PICC_WRITE;
    ucComMF522Buf[1] = ucAddr;

    CalulateCRC(ucComMF522Buf, 2, &ucComMF522Buf[2]);

    cStatus = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, &ulLen);

    if ((cStatus != MI_OK) || (ulLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
        cStatus = MI_ERR;

    if (cStatus == MI_OK)
    {
        // memcpy(ucComMF522Buf, pData, 16);
        for (uc = 0; uc < 16; uc++)
            ucComMF522Buf[uc] = *(pData + uc);

        CalulateCRC(ucComMF522Buf, 16, &ucComMF522Buf[16]);

        cStatus = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 18, ucComMF522Buf, &ulLen);

        if ((cStatus != MI_OK) || (ulLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
            cStatus = MI_ERR;
    }

    return cStatus;
}

/*
 * 函数名：PcdRead
 * 描述  ：读取M1卡一块数据
 * 输入  ：u8 ucAddr，块地址
 *         pData，读出的数据，16字节
 * 返回  : 状态值
 *         = MI_OK，成功
 * 调用  ：外部调用
 */
char PcdRead(u8 ucAddr, u8 *pData)
{
    char cStatus;
    u8 uc, ucComMF522Buf[MAXRLEN];
    u32 ulLen;

    ucComMF522Buf[0] = PICC_READ;
    ucComMF522Buf[1] = ucAddr;

    CalulateCRC(ucComMF522Buf, 2, &ucComMF522Buf[2]);

    cStatus = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, &ulLen);

    if ((cStatus == MI_OK) && (ulLen == 0x90))
    {
        for (uc = 0; uc < 16; uc++)
            *(pData + uc) = ucComMF522Buf[uc];
    }
    else
        cStatus = MI_ERR;
    return cStatus;
}

/*
 * 函数名：PcdHalt
 * 描述  ：命令卡片进入休眠状态
 * 输入  ：无
 * 返回  : 状态值
 *         = MI_OK，成功
 * 调用  ：外部调用
 */
char PcdHalt(void)
{
    u8 ucComMF522Buf[MAXRLEN];
    u32 ulLen;

    ucComMF522Buf[0] = PICC_HALT;
    ucComMF522Buf[1] = 0;

    CalulateCRC(ucComMF522Buf, 2, &ucComMF522Buf[2]);
    PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, &ulLen);

    return MI_OK;
}

void IC_CMT(u8 *UID, u8 *KEY, u8 RW, u8 *Dat)
{
    u8 ucArray_ID[4] = {0};             // 先后存放IC卡的类型和UID(IC卡序列号)
    PcdRequest(0x52, ucArray_ID);       // 寻卡
    PcdAnticoll(ucArray_ID);            // 防冲撞
    PcdSelect(UID);                     // 选定卡
    PcdAuthState(0x60, 0x10, KEY, UID); // 校验

    if (RW) // 读写选择，1是读，0是写
        PcdRead(0x10, Dat);
    else
        PcdWrite(0x10, Dat);

    PcdHalt();
}

//==============================================================================
// 读取卡的ID号
//==============================================================================

#if 0
char * IC_test ( void )
{
    static char cStr [ 30 ];
    u8 ucArray_ID [ 4 ];   //先后存放IC卡的类型和UID(IC卡序列号)
    u8 ucStatusReturn;  //返回状态
    static u8 ucLineCount = 0; 
    while ( 1 )
    { 
        if ( ( ucStatusReturn = PcdRequest ( PICC_REQALL, ucArray_ID ) ) != MI_OK ) //寻卡
        {
             ucStatusReturn = PcdRequest ( PICC_REQALL, ucArray_ID );//若失败再次寻卡         
        }
        if ( ucStatusReturn == MI_OK  )
        {
            if ( PcdAnticoll ( ucArray_ID ) == MI_OK )   //防冲撞（当有多张卡进入读写器操作范围时，防冲突机制会从其中选择一张进行操作）
            {
                sprintf ( cStr, "The Card ID is--> %02X:%02X:%02X:%02X", 
                                                    ucArray_ID [ 0 ], 
                                                    ucArray_ID [ 1 ], 
                                                    ucArray_ID [ 2 ], 
                                                    ucArray_ID [ 3 ] );
                printf ( "%s\r\n",cStr ); 
                if ( ucLineCount == 0 )  ucLineCount ++;
                if ( ucLineCount == 17 ) ucLineCount = 0;
            }
        }
        vTaskDelay(1);
    }
    return cStr;
}
#endif

#if 1
//==============================================================================
// 读取卡的类型
// 读取卡的ID号
//==============================================================================
char *ReaderCard(void)
{
    static char cStr[30];
    if (PcdRequest(PICC_REQALL, Temp) == MI_OK) // 选卡
    {

        if (PcdAnticoll(UID) == MI_OK) // 防冲撞
        {
            sprintf(cStr, "%02X:%02X:%02X:%02X",
                    UID[0],
                    UID[1],
                    UID[2],
                    UID[3]);
            printf("%s\r\n", cStr);
        }
    }
    return cStr;
}
#endif

// 显示卡的卡号，以十六进制显示
void ShowID(u8 *p)
{
    u8 num[9];
    u8 i;
    for (i = 0; i < 4; i++)
    {
        num[i * 2] = p[i] / 16;
        num[i * 2] > 9 ? (num[i * 2] += '7') : (num[i * 2] += '0');
        num[i * 2 + 1] = p[i] % 16;
        num[i * 2 + 1] > 9 ? (num[i * 2 + 1] += '7') : (num[i * 2 + 1] += '0');
    }
    num[8] = 0;
    printf("\r\nID>>>%s\r\n", num);
}

// 测试程序0，完成addr读写读
void RC522_Handle(void)
{
    u8 i = 0;
    status = PcdRequest(PICC_REQALL, CT); // 寻卡
    // printf("\r\nstatus>>>>>>%d\r\n", status);
    if (status == MI_OK) // 寻卡成功
    {
        status = MI_ERR;
        status = PcdAnticoll(SN); // 防冲撞 获得UID 存入SN
    }
    if (status == MI_OK) // 防冲撞成功
    {
        status = MI_ERR;
        // ShowID(SN); // 串口打印卡的ID号 UID
        // 做个判断
        if ((SN[0] == card_1[0]) && (SN[1] == card_1[1]) && (SN[2] == card_1[2]) && (SN[3] == card_1[3]))
        {
            card1_bit = 1;
            printf("\r\nThe User is:card_1\r\n");
        }
        if ((SN[0] == card_2[0]) && (SN[1] == card_2[1]) && (SN[2] == card_2[2]) && (SN[3] == card_2[3]))
        {
            card2_bit = 1;
            printf("\r\nThe User is:card_2\r\n");
        }
        if ((SN[0] == card_3[0]) && (SN[1] == card_3[1]) && (SN[2] == card_3[2]) && (SN[3] == card_3[3]))
        {
            card3_bit = 1;
            printf("\r\nThe User is:card_3\r\n");
        }
        // total = card1_bit+card2_bit+card3_bit+card4_bit+card0_bit;
        status = PcdSelect(SN);
    }
    else
    {
    }
    if (status == MI_OK) // 选卡成功
    {
        status = MI_ERR;
        // 验证A密钥 块地址 密码 SN
        // 注意：此处的块地址0x0B即2扇区3区块，可以替换成变量addr，此块地址只需要指向某一扇区就可以了，即2扇区为0x08-0x0B这个范围都有效，且只能对验证过的扇区进行读写操作
        status = PcdAuthState(KEYA, 0x0B, KEY_A, SN);
        if (status == MI_OK) // 验证成功
        {
            printf("\r\nPcdAuthState(A) success\r\n");
        }
        else
        {
            printf("\r\nPcdAuthState(A) failed\r\n");
        }
        // 验证B密钥 块地址 密码 SN  块地址0x0B即2扇区3区块，可以替换成变量addr
        status = PcdAuthState(KEYB, 0x0B, KEY_B, SN);
        if (status == MI_OK) // 验证成功
        {
            printf("\r\nPcdAuthState(B) success\r\n");
        }
        else
        {
            printf("\r\nPcdAuthState(B) failed\r\n");
        }
    }
    if (status == MI_OK) // 验证成功
    {
        status = MI_ERR;
        // 读取M1卡一块数据 块地址 读取的数据 注意：因为上面验证的扇区是2扇区，所以只能对2扇区的数据进行读写，即0x08-0x0B这个范围，超出范围读取失败。
        status = PcdRead(addr, DATA);
        if (status == MI_OK) // 读卡成功
        {
            // printf("RFID:%s\r\n", RFID);
            printf("\r\nDATA:");
            for (i = 0; i < 16; i++)
            {
                printf("%02x", DATA[i]);
            }
            printf("\r\n");
        }
        else
        {
            printf("\r\nPcdRead() failed\r\n");
        }
    }
    if (status == MI_OK) // 读卡成功
    {
        status = MI_ERR;
        printf("\r\nWrite the card after 1 second. Do not move the card!!!\r\n");

        delay_xms(1000);
        // status = PcdWrite(addr, DATA0);
        // 写数据到M1卡一块
        status = PcdWrite(addr, DATA1);
        if (status == MI_OK) // 写卡成功
        {
            printf("\r\nPcdWrite() success\r\n");
        }
        else
        {
            printf("\r\nPcdWrite() failed\r\n");
            delay_xms(3000);
        }
    }
    if (status == MI_OK) // 写卡成功
    {
        status = MI_ERR;
        // 读取M1卡一块数据 块地址 读取的数据
        status = PcdRead(addr, DATA);
        if (status == MI_OK) // 读卡成功
        {
            // printf("DATA:%s\r\n", DATA);
            printf("\r\nDATA:");
            for (i = 0; i < 16; i++)
            {
                printf("%02x", DATA[i]);
            }
            printf("\r\n");
        }
        else
        {
            printf("\r\nPcdRead() failed\r\n");
        }
    }
    if (status == MI_OK) // 读卡成功
    {
        status = MI_ERR;
        printf("\r\nRC522_Handle() run finished after 1 second!\r\n");
        delay_xms(1000);
    }
}

/*
// // UID为你要修改的卡的UID key_type：0为KEYA，非0为KEYB KEY为密钥 RW:1是读，0是写 data_addr为修改的地址 data为数据内容
// void IC_RW(u8 *UID, u8 key_type, u8 *KEY, u8 RW, u8 data_addr, u8 *data)
// {
//     char status;
//     u8 i = 0;
//     u8 ucArray_ID[4] = {0};                // 先后存放IC卡的类型和UID(IC卡序列号)
//     status = PcdRequest(0x52, ucArray_ID); // 寻卡
//     if (status == MI_OK)
//         ShowID(ucArray_ID);
//     else
//         return;
//     status = PcdAnticoll(ucArray_ID); // 防冲撞
//     if (status != MI_OK)
//         return;
//     status = PcdSelect(UID); // 选定卡
//     if (status != MI_OK)
//     {
//         printf("\r\nUID don't match\r\n");
//         return;
//     }
//     if (0 == key_type)
//         status = PcdAuthState(KEYA, data_addr, KEY, UID); // 校验
//     else
//         status = PcdAuthState(KEYB, data_addr, KEY, UID); // 校验
//     if (status != MI_OK)
//     {
//         printf("\r\nKEY don't match\r\n");
//         return;
//     }
//     if (RW) // 读写选择，1是读，0是写
//     {
//         status = PcdRead(data_addr, data);
//         if (status == MI_OK)
//         {
//             printf("\r\ndata:");
//             for (i = 0; i < 16; i++)
//             {
//                 printf("%02x", data[i]);
//             }
//             printf("\r\n");
//         }
//         else
//         {
//             printf("\r\nPcdRead() failed\r\n");
//             return;
//         }
//     }
//     else
//     {
//         status = PcdWrite(data_addr, data);
//         if (status == MI_OK)
//         {
//             printf("\r\nPcdWrite() finished\r\n");
//         }
//         else
//         {
//             printf("\r\nPcdWrite() failed\r\n");
//             return;
//         }
//     }
//     status = PcdHalt();
//     if (status == MI_OK)
//     {
//         printf("\r\nPcdHalt() finished\r\n");
//     }
//     else
//     {
//         printf("\r\nPcdHalt() failed\r\n");
//         return;
//     }
// }
*/

void write_card_a(void){
    status = PcdRequest(PICC_REQALL, CT); // 寻卡
    if (status == MI_OK){     // 读卡成功
        status = MI_ERR;
        printf("\r\nWrite the card after 1 second. Do not move the card!!!\r\n");
        delay_xms(1000);
        status = PcdWrite(addr, DATA0);  // 写数据到M1卡一块
        if (status == MI_OK){            // 写卡成功
            printf("\r\nPcdWrite() success\r\n");
        }else{
            printf("\r\nPcdWrite() failed\r\n");
            delay_xms(3000);
        }
    }
}
void write_card_b(void){
    status = PcdRequest(PICC_REQALL, CT); // 寻卡
    if (status == MI_OK){     // 读卡成功
        status = MI_ERR;
        printf("\r\nWrite the card after 1 second. Do not move the card!!!\r\n");
        delay_xms(1000);
        status = PcdWrite(addr, DATA_B);  // 写数据到M1卡一块
        if (status == MI_OK){            // 写卡成功
            printf("\r\nPcdWrite() success\r\n");
        }else{
            printf("\r\nPcdWrite() failed\r\n");
            delay_xms(3000);
        }
    }
}
void write_card_c(void){
    status = PcdRequest(PICC_REQALL, CT); // 寻卡
    if (status == MI_OK){     // 读卡成功
        status = MI_ERR;
        printf("\r\nWrite the card after 1 second. Do not move the card!!!\r\n");
        delay_xms(1000);
        status = PcdWrite(addr, DATA_C);  // 写数据到M1卡一块
        if (status == MI_OK){            // 写卡成功
            printf("\r\nPcdWrite() success\r\n");
        }else{
            printf("\r\nPcdWrite() failed\r\n");
            delay_xms(3000);
        }
    }
}

void read_card(void){
    u8 i = NULL;
    status = PcdRequest(PICC_REQALL, CT); // 寻卡
    if (status == MI_OK) {    // 验证成功
        status = MI_ERR;
		status = PcdRead(addr, DATA);
        if (status == MI_OK){  // 读卡成功 
            printf("\r\nDATA:");
            for (i = 0; i < 1; i++){
                printf("%02x", DATA[i]);
            }
            printf("\r\n");
        }
		else{
           printf("\r\nPcdRead() failed\r\n");
       }
    }
	
}


