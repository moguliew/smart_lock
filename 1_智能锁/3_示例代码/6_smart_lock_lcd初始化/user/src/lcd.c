#include "stm32f4xx.h"                  // Device header
#include "lcd.h"
#include "tim.h"

/**
 * ������    ��lcd_spi_io_init
 * ��������  ��ģ��SPI�Ĺܽų�ʼ����
 * ��������  ����
 * ��������ֵ����
 * ��������  ��LCD_CLK----------------------------PB13     
 *			  LCD_SDA----------------------------PB15        
 */
void lcd_spi_io_init(void)
{
	//��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	//����ģʽ����
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Pin =  GPIO_Pin_13 | GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Fast_Speed;
	GPIO_Init(GPIOB, &GPIO_InitStruct);	
	
}

/**
 * ������    ��lcd_io_init
 * ��������  ��LCD�Ĺܽų�ʼ����
 * ��������  ���� 
 * ��������ֵ����
 * ��������  �� LEDK-----PB1        ���������        //ͨ�����
 *			RESET----PB10        ��λ����          //ͨ�����
 *			LCD_CS---PB12        Ƭѡ����          //ͨ�����	
 *			LCD_D/C--PB14        ��������ѡ����    //ͨ�����		  
 */
void lcd_io_init(void)
{
	//GPIO��ʼ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//1.��ʱ��
	
	//�������
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin =  GPIO_Pin_1 | GPIO_Pin_10 | GPIO_Pin_12 | GPIO_Pin_14;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStruct);	
	
	//��ʼ״̬
	LCD_LED_OFF;               //�����
	LCD_CS_H;                  //Ƭѡ����
}



/**
 * ������    ��lcd_spi_byte
 * ��������  ��SPI����һ���ֽں���
 * ��������  ����
 * ��������ֵ����
 * ��������  ��    
 */
void lcd_spi_byte(uint8_t data)
{
	uint8_t i;
	//����
	LCD_SPI_SCL_L;
	LCD_MOSI_H;
	
	/*��������*/
	for(i=0;i<8;i++)
	{
		LCD_SPI_SCL_L;    					
		if(data & 0x80) 
			LCD_MOSI_H; 
		else 
			LCD_MOSI_L;
		LCD_SPI_SCL_H;   
		data = data << 1;
	}	
}



/*
 * ������    ��st7789vm_write_command
 * ��������  ����LCDд�����
 * ��������  ��uint8_t cmd
 * ��������ֵ����
 * ��������  ��    
 */
void st7789vm_write_command(uint8_t cmd)
{
	LCD_CS_L;			//������Ƭѡ
	LCD_CMD;			//��ʾ���͵�������
	lcd_spi_byte(cmd);	//���巢�͵�8λ ����
	LCD_CS_H;			//����Ƭѡ
}


/*
 * ������    ��st7789vm_write_command_parameter
 * ��������  ����LCDд�����������
 * ��������  ��uint8_t cmd_data
 * ��������ֵ����
 * ��������  ��    
 */
void st7789vm_write_command_parameter(uint8_t cmd_data)
{
	LCD_CS_L;					//������Ƭѡ
	LCD_DATA;					//��ʾ���͵�������
	lcd_spi_byte(cmd_data);		//���巢�͵�8λ �������
	LCD_CS_H;					//����Ƭѡ
}



/**
 * ������    ��st7789vm_write_data
 * ��������  ����LCDд���ݺ���
 * ��������  ��uint16_t data
 * ��������ֵ����
 * ��������  ������ֵ����ɫֵ   
 */
void st7789vm_write_data(uint16_t data)
{
	LCD_CS_L;					//������Ƭѡ
	LCD_DATA;					//��ʾ���͵�������
	lcd_spi_byte(data>>8);		//���巢�͵�8λ �������
	lcd_spi_byte(data & 0x00ff);
	LCD_CS_H;					//����Ƭѡ
}

/**
 * ������    ��st7789vm_rest
 * ��������  ����LCD��λ����
 * ��������  ����
 * ��������ֵ����
 * ��������  ��  
 */
void st7789vm_rest(void)
{
	LCD_RES_L;
	tim5_delay_ms(100);
	LCD_RES_H;
	//Tim5_delay_ms(100);
}









/**
 * ������    ��LCD_init
 * ��������  ����LCD��ʼ����
 * ��������  ����
 * ��������ֵ����
 * ��������  ��  
 */
void lcd_init(void)
{
	//LCD_IO��ʼ��
	lcd_io_init();
	//SPI_IO��ʼ��
	lcd_spi_io_init();
	//LCD��λ
	st7789vm_rest();
	//IC����������ֲ
	/* start initial sequence */ 
	st7789vm_write_command(0x36);
	st7789vm_write_command_parameter(0x00);

	st7789vm_write_command(0x3A); 
	st7789vm_write_command_parameter(0x05);

	st7789vm_write_command(0xB2);
	st7789vm_write_command_parameter(0x0C);
	st7789vm_write_command_parameter(0x0C);
	st7789vm_write_command_parameter(0x00);
	st7789vm_write_command_parameter(0x33);
	st7789vm_write_command_parameter(0x33); 

	st7789vm_write_command(0xB7); 
	st7789vm_write_command_parameter(0x35);  

	st7789vm_write_command(0xBB);
	st7789vm_write_command_parameter(0x19);

	st7789vm_write_command(0xC0);
	st7789vm_write_command_parameter(0x2C);

	st7789vm_write_command(0xC2);
	st7789vm_write_command_parameter(0x01);

	st7789vm_write_command(0xC3);
	st7789vm_write_command_parameter(0x12);   

	st7789vm_write_command(0xC4);
	st7789vm_write_command_parameter(0x20);  

	st7789vm_write_command(0xC6); 
	st7789vm_write_command_parameter(0x0F);    

	st7789vm_write_command(0xD0); 
	st7789vm_write_command_parameter(0xA4);
	st7789vm_write_command_parameter(0xA1);

	st7789vm_write_command(0xE0);
	st7789vm_write_command_parameter(0xD0);
	st7789vm_write_command_parameter(0x04);
	st7789vm_write_command_parameter(0x0D);
	st7789vm_write_command_parameter(0x11);
	st7789vm_write_command_parameter(0x13);
	st7789vm_write_command_parameter(0x2B);
	st7789vm_write_command_parameter(0x3F);
	st7789vm_write_command_parameter(0x54);
	st7789vm_write_command_parameter(0x4C);
	st7789vm_write_command_parameter(0x18);
	st7789vm_write_command_parameter(0x0D);
	st7789vm_write_command_parameter(0x0B);
	st7789vm_write_command_parameter(0x1F);
	st7789vm_write_command_parameter(0x23);

	st7789vm_write_command(0xE1);
	st7789vm_write_command_parameter(0xD0);
	st7789vm_write_command_parameter(0x04);
	st7789vm_write_command_parameter(0x0C);
	st7789vm_write_command_parameter(0x11);
	st7789vm_write_command_parameter(0x13);
	st7789vm_write_command_parameter(0x2C);
	st7789vm_write_command_parameter(0x3F);
	st7789vm_write_command_parameter(0x44);
	st7789vm_write_command_parameter(0x51);
	st7789vm_write_command_parameter(0x2F);
	st7789vm_write_command_parameter(0x1F);
	st7789vm_write_command_parameter(0x1F);
	st7789vm_write_command_parameter(0x20);
	st7789vm_write_command_parameter(0x23);

	st7789vm_write_command(0x21); 
	st7789vm_write_command(0x11); 
	st7789vm_write_command(0x29); 
	
	//������
	LCD_LED_ON;
	//����
	lcd_clear(0x0000);
}




/**
 * ������    ��lcd_clear
 * ��������  ������
 * ��������  ��uint16_t color
 * ��������ֵ����
 * ��������  ��  
 */
void lcd_clear(uint16_t color)
{
	/*ȷ������*/
	//ȷ��x����
	st7789vm_write_command(0x2A);     //����������
	//��ʼ������
	st7789vm_write_data(0);     
	//����������
	st7789vm_write_data(LCD_W-1);
	
	//ȷ��Y����
	st7789vm_write_command(0x2B);     //����������
	//��ʼ������
	st7789vm_write_data(0);
	//����������
	st7789vm_write_data(LCD_H-1);
	
	/*ȷ����ɫ*/
	st7789vm_write_command(0x2C);     //��ɫ����
	for(uint32_t i=0;i<LCD_W*LCD_H;i++)
	{
		st7789vm_write_data(color);
	}
}




/**
 * ������    ��LCD_xy_clear
 * ��������  ����ĳ������
 * ��������  ��uint16_t x,uint16_t y,uint16_t w,uint16_t h,uint16_t color
 * ��������ֵ����
 * ��������  ��  
 */
void lcd_xy_clear(uint16_t x,uint16_t y,uint16_t w,uint16_t h,uint16_t color)
{
	/*ȷ������*/
	//ȷ��x����
	st7789vm_write_command(0x2A);     //����������
	//��ʼ������
	st7789vm_write_data(x);     
	//����������
	st7789vm_write_data(x+w-1);
	
	//ȷ��Y����
	st7789vm_write_command(0x2B);     //����������
	//��ʼ������
	st7789vm_write_data(y);
	//����������
	st7789vm_write_data(y+h-1);
	
	
	/*ȷ����ɫ*/
	st7789vm_write_command(0x2C);     //��ɫ����
	for(uint32_t i=0;i<w*h;i++)
	{
		st7789vm_write_data(color);
	}
}








/**
 * ������    ��lcd_draw_point
 * ��������  ����㺯��
 * ��������  ��uint16_t x,uint16_t y,uint16_t color
 * ��������ֵ����
 * ��������  ��  
 */
void lcd_draw_point(uint16_t x,uint16_t y,uint16_t color)
{
	/*ȷ������*/
	//ȷ��x����
	st7789vm_write_command(0x2A);     //����������
	//��ʼ������
	st7789vm_write_data(x);     
	//����������
	st7789vm_write_data(x);
	
	//ȷ��Y����
	st7789vm_write_command(0x2B);     //����������
	//��ʼ������
	st7789vm_write_data(y);
	//����������
	st7789vm_write_data(y);
	
	
	/*ȷ����ɫ*/
	st7789vm_write_command(0x2C);     //��ɫ����
	st7789vm_write_data(color);
	
}


/**
 * ������    ��LCD_Ring
 * ��������  ��LCD��Ļ��Բ����
 * ��������  ��uint16_t x,uint16_t y,uint16_t r,uint16_t color 
 * ��������ֵ����
 * ��������  ��
 *		     ��x - x0��^2 + (y - y0)^2  == r*r 
 */
void lcd_circle(uint16_t x,uint16_t y,uint16_t r,uint16_t color)
{
	uint16_t i,j;
	
	for(i=x-r;i<=x+r;i++)         //����������
	{
		for(j=y-r;j<=y+r;j++)       //����������
		{
			if((i-x)*(i-x) + (j-y)*(j-y) >= (r-3)*(r-3) && (i-x)*(i-x) + (j-y)*(j-y) <= r*r)   //�жϱ����ĵ��Ƿ��������
			{
				//��㺯��
				lcd_draw_point(i,j,color); //���������ĵ�����
			}
		}
	}
}


/**
 * ������    ��LCD_Ring_shi
 * ��������  ��LCD��Ļ��Բ����ʵ��Բ
 * ��������  ��uint16_t x,uint16_t y,uint16_t r,uint16_t color 
 * ��������ֵ����
 * ��������  ��
 *			��x - x0��^2 + (y - y0)^2  <= r*r 
 */
void lcd_solid_circle(uint16_t x,uint16_t y,uint16_t r,uint16_t color)
{
	uint16_t i,j;
	
	for(i=x-r;i<=x+r;i++)         //����������
	{
		for(j=y-r;j<=y+r;j++)       //����������
		{
			if((i-x)*(i-x) + (j-y)*(j-y) <= r*r)   //�жϱ����ĵ��Ƿ��������
			{
				//��㺯��
				lcd_draw_point(i,j,color); //���������ĵ�����
			}
		}
	}
}














