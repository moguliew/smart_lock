#include "stm32f4xx.h"                  // Device header
#include "w25q64.h"

/****************************************SPI***********************************************/
/**
 * ������    ��w25q64_spi_init
 * ��������  ��w25q64����SPI��ʼ������
 * ��������  ����
 * ��������ֵ����
 * ��������  ��CLK--ʱ����--PA5--��������
 *			  DO---���������--PA6--��������
 *			  DI---����������--PA7--����
 */
void w25q64_spi_init(void)
{
	//ʱ��ʹ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	
	//GPIO����
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;          //ͨ�����
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;         //����
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7; 
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;       //��������
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;       //�ٶ�
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	//���ù�������
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource5 ,GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource6 ,GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource7 ,GPIO_AF_SPI1);
	
	
	/* SPI���������� */
	//ʱ��ʹ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
	
	SPI_InitTypeDef SPI_InitStruct = {0};
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;  //2��Ƶ
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;                        //��һ�������� 0
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low ;                         //���е͵�ƽ  0
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;                   //8λ����
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //ȫ˫��
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;                  //�ȴ����λ
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;                       //����ģʽ
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;                           //�������������
	SPI_Init(SPI1, &SPI_InitStruct);
	
	SPI_NSSInternalSoftwareConfig(SPI1,SPI_NSSInternalSoft_Set);     //�ڲ�NSS������1
	
	//SPIʹ��
	SPI_Cmd(SPI1, ENABLE);
	
}

/**
 * ������  : w25q64_spi_byte
 * �������ܣ�SPI���ͺͽ���1byte����
 * �����βΣ�uint8_t data
 * ��������ֵ��uint8_t 
 * ˵������λ�ȷ�
 */
uint8_t w25q64_spi_byte(uint8_t data)
{
	uint8_t buf = 0;
	//�ȵ����ͼĴ���Ϊ�վͿ���д������
	while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE));
	
	SPI_I2S_SendData(SPI1,data);
	//�ȵ��������ݼĴ�����Ϊ�վͿ��Զ�ȡ����
	while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE));
	
	buf = SPI_I2S_ReceiveData(SPI1);
	return buf;
}




/****************************************W25Q64***********************************************/
/**
 * ������    ��w25q64_init
 * ��������  ��w25q64��ʼ������
 * ��������  ����
 * ��������ֵ����
 * ��������  ��CS:PA4
 */
void w25q64_init(void)
{
	w25q64_spi_init();
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;          //ͨ�����
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;         //����
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4 ; 
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;       //��������
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;       //�ٶ�
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	GPIO_SetBits(GPIOA, GPIO_Pin_4);
	
	//����鱣��
	write_status();
	
}



/**
 * ������    ��read_status
 * ��������  ����w25q64״̬�Ĵ�����ֵ
 * ��������  ����
 * ��������ֵ��״̬�Ĵ�����ֵ
 * ��������  ������0x05
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
 * ������    ��wirite_enable
 * ��������  ��дʹ��
 * ��������  ����
 * ��������ֵ����
 * ��������  ������0x06
 */
void wirite_enable(void)
{
	W25Q64_CS_L;
	w25q64_spi_byte(0x06);
	W25Q64_CS_H;
}



/**
 * ������    ��write_status
 * ��������  ��дw25q64״̬�Ĵ�����������б���
 * ��������  ����
 * ��������ֵ��uint8_t
 * ��������  ������0x01
 */
void write_status(void)
{
	wirite_enable();
	W25Q64_CS_L;
	w25q64_spi_byte(0x01);
	w25q64_spi_byte(0x00);      
	W25Q64_CS_H;
	//��״̬�Ĵ�����busyλ�ȴ�д��
	while((read_status()) & 0x01);
}



/**
 * ������    ��w25q64_read
 * ��������  ����w25q64�ж�����
 * ��������  ��uint32_t inner_addr,uint32_t num_byte,uint8_t *str
 * ��������ֵ����
 * ��������  ������0x03
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
 * ������    ��w25q64_page_write
 * ��������  ����w25q64��д���� ҳд
 * ��������  ��uint32_t inner_addr,uint8_t num_byte,uint8_t *str
 * ��������ֵ����
 * ��������  ������0x02
 */
uint8_t w25q64_page_write(uint32_t inner_addr,uint8_t num_byte,uint8_t *str)
{
	
	/*�ж�Ҫд�������Ƿ���ҳ*/
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
	//��״̬�Ĵ���busy���ȴ�����д�����
	while((read_status()) & 0x01);
	
	return 1;
}



/**
 * ������    ��w25q64_sector_erase
 * ��������  �����������ַ����������
 * ��������  ��uint32_t inner_addr
 * ��������ֵ����
 * ��������  ������0x20
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
	//��״̬�Ĵ���busy���ȴ�����д�����
	while((read_status()) & 0x01);
}


/**
 * ������    ��w25q64_block_erase
 * ��������  �����������ַ�����ڿ���
 * ��������  ��uint32_t inner_addr
 * ��������ֵ����
 * ��������  ������0xD8
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
	//��״̬�Ĵ���busy���ȴ�����д�����
	while((read_status()) & 0x01);
}


/**
 * ������    ��w25q64_chip_erase
 * ��������  ��оƬ����
 * ��������  ����
 * ��������ֵ����
 * ��������  ������0xC7
 */
void w25q64_chip_erase(void)
{
	wirite_enable();
	W25Q64_CS_L;
	w25q64_spi_byte(0xC7); 
	W25Q64_CS_H;
	//��״̬�Ĵ���busy���ȴ�����д�����
	while((read_status()) & 0x01);
}


/**
 * ������    ��w25q64_blocks
 * ��������  ��������
 * ��������  ��uint32_t inner_addr,uint8_t blocks
 * ��������ֵ����
 * ��������  ��
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









































