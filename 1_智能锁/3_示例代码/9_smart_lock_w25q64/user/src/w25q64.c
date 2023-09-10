#include "stm32f4xx.h"                  // Device header
#include "w25q64.h"

/****************************************SPI***********************************************/
/**
 * 函数名    ：w25q64_spi_init
 * 函数功能  ：w25q64所用SPI初始化配置
 * 函数参数  ：无
 * 函数返回值：无
 * 函数描述  ：CLK--时钟线--PA5--复用推挽
 *			  DO---数据输出线--PA6--复用推挽
 *			  DI---数据输入线--PA7--复用
 */
void w25q64_spi_init(void)
{
	//时钟使能
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	
	//GPIO配置
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;          //通用输出
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;         //推挽
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7; 
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;       //无上下拉
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;       //速度
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	//复用功能配置
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource5 ,GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource6 ,GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource7 ,GPIO_AF_SPI1);
	
	
	/* SPI控制器配置 */
	//时钟使能
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
	
	SPI_InitTypeDef SPI_InitStruct = {0};
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;  //2分频
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;                        //第一个跳变沿 0
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low ;                         //空闲低电平  0
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;                   //8位数据
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //全双工
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;                  //先传输高位
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;                       //主机模式
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;                           //软件从器件管理
	SPI_Init(SPI1, &SPI_InitStruct);
	
	SPI_NSSInternalSoftwareConfig(SPI1,SPI_NSSInternalSoft_Set);     //内部NSS引脚置1
	
	//SPI使能
	SPI_Cmd(SPI1, ENABLE);
	
}

/**
 * 函数名  : w25q64_spi_byte
 * 函数功能：SPI发送和接收1byte数据
 * 函数形参：uint8_t data
 * 函数返回值：uint8_t 
 * 说明：高位先发
 */
uint8_t w25q64_spi_byte(uint8_t data)
{
	uint8_t buf = 0;
	//等到发送寄存器为空就可以写入数据
	while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE));
	
	SPI_I2S_SendData(SPI1,data);
	//等到接收数据寄存器不为空就可以读取数据
	while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE));
	
	buf = SPI_I2S_ReceiveData(SPI1);
	return buf;
}




/****************************************W25Q64***********************************************/
/**
 * 函数名    ：w25q64_init
 * 函数功能  ：w25q64初始化配置
 * 函数参数  ：无
 * 函数返回值：无
 * 函数描述  ：CS:PA4
 */
void w25q64_init(void)
{
	w25q64_spi_init();
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;          //通用输出
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;         //推挽
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4 ; 
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;       //无上下拉
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;       //速度
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	GPIO_SetBits(GPIOA, GPIO_Pin_4);
	
	//解除块保护
	write_status();
	
}



/**
 * 函数名    ：read_status
 * 函数功能  ：读w25q64状态寄存器的值
 * 函数参数  ：无
 * 函数返回值：状态寄存器的值
 * 函数描述  ：命令0x05
 */
uint8_t read_status(void)
{
	uint8_t sta;
	W25Q64_CS_L;
	w25q64_spi_byte(0x05);
	sta = w25q64_spi_byte(0xff);
	W25Q64_CS_H;
	
	return sta;
}


/**
 * 函数名    ：wirite_enable
 * 函数功能  ：写使能
 * 函数参数  ：无
 * 函数返回值：无
 * 函数描述  ：命令0x06
 */
void wirite_enable(void)
{
	W25Q64_CS_L;
	w25q64_spi_byte(0x06);
	W25Q64_CS_H;
}



/**
 * 函数名    ：write_status
 * 函数功能  ：写w25q64状态寄存器，解除所有保护
 * 函数参数  ：无
 * 函数返回值：uint8_t
 * 函数描述  ：命令0x01
 */
void write_status(void)
{
	wirite_enable();
	W25Q64_CS_L;
	w25q64_spi_byte(0x01);
	w25q64_spi_byte(0x00);      
	W25Q64_CS_H;
	//读状态寄存器的busy位等待写完
	while((read_status()) & 0x01);
}



/**
 * 函数名    ：w25q64_read
 * 函数功能  ：从w25q64中读数据
 * 函数参数  ：uint32_t inner_addr,uint32_t num_byte,uint8_t *str
 * 函数返回值：无
 * 函数描述  ：命令0x03
 */
void w25q64_read(uint32_t inner_addr,uint32_t num_byte,uint8_t *str)
{
	W25Q64_CS_L;
	w25q64_spi_byte(0x03);
	w25q64_spi_byte(inner_addr >> 16); 
	w25q64_spi_byte(inner_addr >> 8);  
	w25q64_spi_byte(inner_addr & 0xff); 
	while(1)
	{
		*str = w25q64_spi_byte(0xff);
		num_byte--;
		if(num_byte == 0)
		{
			break;
		}
		str++;
	}
	W25Q64_CS_H;	
}


/**
 * 函数名    ：w25q64_page_write
 * 函数功能  ：往w25q64中写数据 页写
 * 函数参数  ：uint32_t inner_addr,uint8_t num_byte,uint8_t *str
 * 函数返回值：无
 * 函数描述  ：命令0x02
 */
uint8_t w25q64_page_write(uint32_t inner_addr,uint8_t num_byte,uint8_t *str)
{
	
	/*判断要写的内容是否会跨页*/
	if(inner_addr / 256 != (inner_addr + num_byte - 1) / 256)
	{
		return 0;
	}
	
	wirite_enable();
	W25Q64_CS_L;
	w25q64_spi_byte(0x02); 
	w25q64_spi_byte(inner_addr >> 16); 
	w25q64_spi_byte(inner_addr >> 8);  
	w25q64_spi_byte(inner_addr & 0xff); 
	while(1)
	{
		w25q64_spi_byte(*str); 
		num_byte--;
		if(num_byte == 0) break;
		str++;	
	}
	W25Q64_CS_H;
	//读状态寄存器busy，等待数据写入完成
	while((read_status()) & 0x01);
	
	return 1;
}



/**
 * 函数名    ：w25q64_sector_erase
 * 函数功能  ：擦除传入地址的所在扇区
 * 函数参数  ：uint32_t inner_addr
 * 函数返回值：无
 * 函数描述  ：命令0x20
 */
void w25q64_sector_erase(uint32_t inner_addr)
{
	wirite_enable();
	W25Q64_CS_L;
	w25q64_spi_byte(0x20); 
	w25q64_spi_byte(inner_addr >> 16); 
	w25q64_spi_byte(inner_addr >> 8);  
	w25q64_spi_byte(inner_addr & 0xff); 
	W25Q64_CS_H;
	//读状态寄存器busy，等待数据写入完成
	while((read_status()) & 0x01);
}


/**
 * 函数名    ：w25q64_block_erase
 * 函数功能  ：擦除传入地址的所在块区
 * 函数参数  ：uint32_t inner_addr
 * 函数返回值：无
 * 函数描述  ：命令0xD8
 */
void w25q64_block_erase(uint32_t inner_addr)
{
	wirite_enable();
	W25Q64_CS_L;
	w25q64_spi_byte(0xD8); 
	w25q64_spi_byte(inner_addr >> 16); 
	w25q64_spi_byte(inner_addr >> 8);  
	w25q64_spi_byte(inner_addr & 0xff); 
	W25Q64_CS_H;
	//读状态寄存器busy，等待数据写入完成
	while((read_status()) & 0x01);
}


/**
 * 函数名    ：w25q64_chip_erase
 * 函数功能  ：芯片擦除
 * 函数参数  ：无
 * 函数返回值：无
 * 函数描述  ：命令0xC7
 */
void w25q64_chip_erase(void)
{
	wirite_enable();
	W25Q64_CS_L;
	w25q64_spi_byte(0xC7); 
	W25Q64_CS_H;
	//读状态寄存器busy，等待数据写入完成
	while((read_status()) & 0x01);
}


/**
 * 函数名    ：w25q64_blocks
 * 函数功能  ：多块擦除
 * 函数参数  ：uint32_t inner_addr,uint8_t blocks
 * 函数返回值：无
 * 函数描述  ：
 */
void w25q64_blocks(uint32_t inner_addr,uint8_t blocks)
{
	while(blocks)
	{
		w25q64_block_erase(inner_addr);
		inner_addr += 65536;
		blocks--;
	}
}









































