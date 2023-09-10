#include "stm32f4xx.h"                  // Device header
#include "lcd.h"
#include "tim.h"
#include "font.h"
#include "w25q64.h"

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
 * ������    ��lcd_circle
 * ��������  ��LCD��Ļ��Բ����
 * ��������  ��r--�뾶
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
 * ������    ��lcd_solid_circle
 * ��������  ��LCD��Ļ��Բ����ʵ��Բ
 * ��������  ��r--�뾶 
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








/**
 * ������    ��lcd_show_16ch
 * ��������  ��LCD��Ļ��ʾ16*16���ַ�
 * ��������  ��
 * ��������ֵ����
 * ��������  ��8*16		����ʽ		��ģһ��1byte��һ��16byte
 */
void lcd_show_16ch(uint16_t x,uint16_t y,uint8_t ch,uint16_t f_color,uint16_t b_color)
{
	uint8_t n,i,j;
	uint8_t temp;
	
	/*����Ҫ��ʾ���ַ���ո��ƫ��ֵ n*/
	n = ch - ' ';
	
	/*��ʾ*/
	for(i=0;i<16;i++)     //��һ��ռ������
	{
		//��Ҫ��ʾ���ַ���ģ����ȡ�����ŵ�һ��������
		temp = asc16[n*16+i];
		for(j=0;j<8;j++)   //ÿ�б������ص����
		{
			if(temp & (0x80 >> j))
			{
				lcd_draw_point(x+j,y+i,f_color);
			}
			else   //����ɫ
			{
				lcd_draw_point(x+j,y+i,b_color);
			}
			
		}
	}
}



/*
 * ������    ��lcd_show_32ch
 * ��������  ��LCD��Ļ��ʾ32*32���ַ�
 * ��������  ��
 * ��������ֵ����
 * ��������  ��16 * 32		����ʽ		��ģһ��2byte��һ��64byte	
*/
void lcd_show_32ch(uint16_t x,uint16_t y,uint8_t ch,uint16_t f_color,uint16_t b_color)
{
	uint8_t n,i,j;
	uint16_t temp;
	
	/*����Ҫ��ʾ���ַ���ո��ƫ��ֵ n*/
	n = ch - ' ';
	
	/*��ʾ*/
	for(i=0;i<32;i++)
	{
		//��Ҫ��ʾ���ַ���ģ����ȡ�����ŵ�һ��������
		temp = (asc32[n*64+2*i] << 8)  |  asc32[n*64+2*i+1];
		      
		for(j=0;j<16;j++)
		{
			if(temp & (0x8000 >> j))
			{
				lcd_draw_point(x+j,y+i,f_color);
			}
			else    //����ɫ
			{
				lcd_draw_point(x+j,y+i,b_color);
			}
			
		}
	}
}



/**
 * ������    ��lcd_show_16hz
 * ��������  ��LCD��Ļ��ʾ16*16�ĺ���
 * ��������  ��
 * ��������ֵ����
 * ��������  ��16 * 16		����ʽ		��ģһ��2byte��һ��32byte
 *						
 */
void lcd_show_16hz(uint16_t x,uint16_t y,uint8_t *str,uint16_t f_color,uint16_t b_color)
{
	uint8_t n = 0;
	uint8_t i,j;
	uint16_t temp;
	
	/*����Ҫ��ʾ���ַ���ո��ƫ��ֵ n*/
	while(1)
	{
		if(*str==table[2*n] && *(str+1)==table[2*n+1])
		{
			break;
		}
		n++;
	}
	//n����Ҫ��ʾ�ĺ������һ����ֽ��ƫ��ֵ
	
	
	/*��ʾ*/
	for(i=0;i<16;i++)
	{
		//��Ҫ��ʾ���ַ���ģ����ȡ�����ŵ�һ��������
		temp = hz16[n*32+2*i]<<8 | hz16[n*32+2*i+1];
		for(j=0;j<16;j++)
		{
			if(temp & (0x8000 >> j))
			{
				lcd_draw_point(x+j,y+i,f_color);
			}
			else  //����ɫ
			{
				lcd_draw_point(x+j,y+i,b_color);
			}
			
		}
	}
}


/**
 * ������    ��lcd_show_32hz
 * ��������  ��LCD��Ļ��ʾ32*32�ĺ���
 * ��������  ��
 * ��������ֵ����
 * ��������  ��32 * 32		����ʽ		��ģһ��4byte��һ��128byte 						
 */
void lcd_show_32hz(uint16_t x,uint16_t y,uint8_t *str,uint16_t f_color,uint16_t b_color)
{
	uint8_t n = 0;
	uint8_t i,j;
	uint32_t temp;
	
	/*����Ҫ��ʾ���ַ���ո��ƫ��ֵ n*/
	while(1)
	{
		if(*str==table[2*n] && *(str+1)==table[2*n+1])
		{
			break;
		}
		n++;
	}
	//n����ƫ��ֵ
	
	
	/*��ʾ*/
	for(i=0;i<32;i++)
	{
		//��Ҫ��ʾ���ַ���ģ����ȡ�����ŵ�һ��������
		temp = hz32[n*128+4*i] << 24  | 
		       hz32[n*128+4*i+1] <<16 |
		       hz32[n*128+4*i+2] << 8 |
					 hz32[n*128+4*i+3];
		for(j=0;j<32;j++)
		{
			if(temp & (0x80000000 >> j))
			{
				lcd_draw_point(x+j,y+i,f_color);
			}
			else  //����ɫ
			{
				lcd_draw_point(x+j,y+i,b_color);
			}
			
		}
	}
}



/**
 * ������    ��lcd_show_16
 * ��������  ��LCD��Ļ��ʾ16*16�ĺ��ֺ��ַ����
 * ��������  ��uint16_t x,uint16_t y,uint16_t color,uint8_t *str,uint8_t mode,uint16_t b_color  
 * ��������ֵ����
 * ��������  ��
 */
void lcd_show_16(uint16_t x,uint16_t y,uint8_t *str,uint16_t f_color,uint16_t b_color)
{
	while(*str != '\0')
	{
		//�ж����ַ�
		if(*str <= 127)
		{
			lcd_show_16ch(x,y,*str,f_color,b_color);
			str++;
			x+=8;
			if(x>=LCD_W-8)    //����
			{
				x=0;
				y+=16;
			}
		}
		//�ж��Ǻ���
		else
		{
			lcd_show_16hz(x,y,str,f_color,b_color);
			str+=2;
			x+=16;
			if(x>=LCD_W-16)
			{
				x=0;
				y+=16;
			}
		}
		
	}
	
}




/**
 * ������    ��lcd_show_32
 * ��������  ��LCD��Ļ��ʾ32*32�ĺ��ֺ��ַ����
 * ��������  ��
 * ��������ֵ����
 * ��������  ��			
 */
void lcd_show_32(uint16_t x,uint16_t y,uint8_t *str,uint16_t f_color,uint16_t b_color)
{
	while(*str != '\0')
	{
		//�ж����ַ�
		if(*str <= 127)
		{
			lcd_show_32ch(x,y,*str,f_color,b_color);
			str++;
			x+=16;
			if(x>=LCD_W-16)    //����
			{
				x=0;
				y+=32;
			}
		}
		//�ж��Ǻ���
		else
		{
			lcd_show_32hz(x,y,str,f_color,b_color);
			str+=2;
			x+=32;
			if(x>=LCD_W-32)
			{
				x=0;
				y+=32;
			}
		}
		
	}
	
}






/**
 * ������    ��lcd_show_pic
 * ��������  ��LCD��Ļ��ʾͼƬ
 * ��������  ��
 * ��������ֵ����
 * ��������  ��				
 */
void lcd_show_pic(uint16_t x,uint16_t y,const uint8_t *pic)
{
	
	uint32_t i;
	HEADCOLOR *pic_head;     				//�ṹ��ָ��
	uint16_t *p;
	
	//��ͼƬ��ͷ�����ó���
	pic_head = (HEADCOLOR *)pic;
	//����ͼƬһ���ж��ٸ����ص�
	i = (pic_head->w )  * (pic_head->h);
	
	//��ָ��pȥ�������е���ɫ����
	p = (uint16_t *)(pic + sizeof(HEADCOLOR));  //��ͷ����ƫ��
	
	/*��ʾ*/
	/*�趨������Χ*/
	//���ͺ��������� ��0x2a
	st7789vm_write_command(0x2a);
	//�����������   ��     ��ʼ����     �յ�����
	st7789vm_write_data(x);
	st7789vm_write_data(x+(pic_head->w)-1);
	//�������������� ��0x2b
	st7789vm_write_command(0x2b);
	//�����������   ��     ��ʼ����     �յ�����
	st7789vm_write_data(y);
	st7789vm_write_data(y+(pic_head->h)-1);
	
	/*�趨������ɫ*/
	//������ɫ����   ��0x2c
	st7789vm_write_command(0x2c);
	while(i)
	{
		st7789vm_write_data(*p);
		i--;
		p++;
	}
	
}








/*******************************************************************�ֿ�******************************************************************/
/**
 * ������    ��lcd_flash_show_16ch
 * ��������  ��LCD��Ļ��ʾ16*16���ַ�
 * ��������  ��uint16_t x,uint16_t y,uint16_t color,uint16_t b_color,uint8_t ch,uint8_t mode
 * ��������ֵ����
 * ��������  ����¼��ַ��0x000000	
 *			 ASC16.bin             0x00000000             
 *			 ASC32.bin             0x00000806             
 *			 HZK16.bin             0x0000280C             
 *			 HZK32.bin             0x00042652 
 */
void lcd_flash_show_16ch(uint16_t x,uint16_t y,uint8_t ch,uint16_t f_color,uint16_t b_color)
{
	uint8_t n,i,j;
	uint8_t temp;
	uint8_t buf[16] = {0};
	
	/*����Ҫ��ʾ���ַ���0��ƫ��ֵ n*/
	n = ch - 0;
	/*��w25q64�н�Ҫ��ʾ���ַ�����ģ����ȫ�������� 16��*/
	w25q64_read(BASE_ADDR + ASC16_ADDR + n*16,16,buf);
	
	
	/*��ʾ*/
	for(i=0;i<16;i++)
	{
		//��Ҫ��ʾ���ַ���ģ����ȡ�����ŵ�һ��������
		temp = buf[i];
		for(j=0;j<8;j++)
		{
			if(temp & (0x80 >> j))
			{
				lcd_draw_point(x+j,y+i,f_color);
			}
			else//������ɫ
			{
				lcd_draw_point(x+j,y+i,b_color);
			}
			
		}
	}
}




/**
 * ������    ��lcd_flash_show_32ch
 * ��������  ��LCD��Ļ��ʾ32*32���ַ�
 * ��������  ��uint16_t x,uint16_t y,uint16_t color,uint16_t b_color,uint8_t ch 
 * ��������ֵ����
 * ��������  ����¼��ַ��0x000000	
				 ASC16.bin             0x00000000             
				 ASC32.bin             0x00000806             
				 HZK16.bin             0x0000280C             
				 HZK32.bin             0x00042652                         
 */
void lcd_flash_show_32ch(uint16_t x,uint16_t y,uint8_t ch,uint16_t f_color,uint16_t b_color)
{
	uint8_t n,i,j;
	uint16_t temp;
	uint8_t buf[64] = {0};
	
	/*����Ҫ��ʾ���ַ���ո��ƫ��ֵ n*/
	n = ch - 0;
	/*��w25q64�н�Ҫ��ʾ���ַ�����ģ����ȫ�������� 16��*/
	w25q64_read(BASE_ADDR + ASC32_ADDR + n*64,64,buf);
	
	/*��ʾ*/
	for(i = 0;i < 32;i++)
	{
		//��Ҫ��ʾ���ַ���ģ����ȡ�����ŵ�һ��������
		temp = buf[2*i] << 8 | buf[2*i+1];
		for(j=0;j<16;j++)
		{
			if(temp & (0x8000 >> j))
			{
				lcd_draw_point(x+j,y+i,f_color);
			}
			else//������ɫ
			{
				lcd_draw_point(x+j,y+i,b_color);
			}
			
		}
	}
}




/**
 * ������    lcd_flash_show_16hz
 * ��������  ��LCD��Ļ��ʾ16*16�ĺ���
 * ��������  ��uint16_t x,uint16_t y,uint16_t color,uint16_t b_color,uint8_t *str
 * ��������ֵ����
 * ��������  ����¼��ַ��0x000000	
			 ASC16.bin             0x00000000             
			 ASC32.bin             0x00000806             
			 HZK16.bin             0x0000280C             
			 HZK32.bin             0x00042652  
 */
void lcd_flash_show_16hz(uint16_t x,uint16_t y,uint8_t *str,uint16_t f_color,uint16_t b_color)
{
	uint32_t n;
	uint8_t buf[32] = {0};
	uint16_t temp;
	uint8_t i,j;
	
	/*����Ҫ��ʾ�ĺ������һ�����ֵ�ƫ����*/
	n = (*str - 0xA1) * 94 +  (*(str + 1) - 0xA1);
	
	/*��w25q64�ж���Ҫ��ʾ�ĺ��ֵ���ģ���ݷ���һ��������*/
	w25q64_read(BASE_ADDR + HZK16_ADDR + n*32,32,buf);

	/*�����ʾ*/
	for(i = 0;i < 16;i++)
	{
		//��ÿһ�е�����ȡ���� 2���ֽ�
		temp = buf[2*i] << 8 | buf[2*i+1];
		for(j = 0;j < 16;j++)
		{
			if(temp & (0x8000 >> j))
			{
				lcd_draw_point(x+j,y+i,f_color);
			}
			else
			{
				lcd_draw_point(x+j,y+i,b_color);
			}
		}
		
	}
	
}




/**
 * ������    ��lcd_flash_show_32hz
 * ��������  ��LCD��Ļ��ʾ32*32�ĺ���
 * ��������  ��uint16_t x,uint16_t y,uint16_t color,uint16_t b_color,uint8_t *str,uint8_t mode
 * ��������ֵ����
 * ��������  ����¼��ַ��0x000000	
				 ASC16.bin             0x00000000             
				 ASC32.bin             0x00000806             
				 HZK16.bin             0x0000280C             
				 HZK32.bin             0x00042652    
 */
void lcd_flash_show_32hz(uint16_t x,uint16_t y,uint8_t *str,uint16_t f_color,uint16_t b_color)
{
	uint32_t n;
	uint8_t buf[128] = {0};
	uint32_t temp;
	uint8_t i,j;
	
	/* ����Ҫ��ʾ�ĺ��ֵ�ƫ���� */
	n = (*str - 0xA1) * 94 +  (*(str + 1) - 0xA1);
	
	/*��w25q64�ж���Ҫ��ʾ�ĺ��ֵ���ģ���ݷ���һ��������*/
	w25q64_read(BASE_ADDR + HZK32_ADDR + n*128,128,buf);

	/*�����ʾ*/
	for(i = 0;i < 32;i++)
	{
		//��ÿһ�е�����ȡ���� 2���ֽ�
		temp = buf[4*i]<<24 | buf[4*i+1]<<16 | buf[4*i+2]<<8 | buf[4*i+3];
		for(j = 0;j < 32;j++)
		{
			if(temp & (0x80000000 >> j))
			{
				lcd_draw_point(x+j,y+i,f_color);
			}
			else
			{
				lcd_draw_point(x+j,y+i,b_color);
			}
		}
		
	}
}



/**
 * ������    ��lcd_flash_show_16
 * ��������  ��LCD��Ļ��ʾ16*16�ĺ��ֺ��ַ����
 * ��������  ��uint16_t x,uint16_t y,uint16_t color,uint8_t *str,uint8_t mode 
 * ��������ֵ����
 * ��������  ��				
 */
void lcd_flash_show_16(uint16_t x,uint16_t y,uint8_t *str,uint16_t f_color,uint16_t b_color)
{
	while(*str != '\0')
	{
		//�ж����ַ�
		if(*str <= 127)
		{
			lcd_flash_show_16ch(x,y,*str,f_color,b_color);
			str++;
			x += 8;
			if(x >= LCD_W-8)//����
			{
				x = 0;
				y += 16;
			}
		}
		//�ж��Ǻ���
		else
		{
			lcd_flash_show_16hz(x,y,str,f_color,b_color);
			str += 2;
			x += 16;
			if(x >= LCD_W-16)
			{
				x = 0;
				y += 16;
			}
		}
		
	}
	
}



/**
 * ������    ��lcd_flash_show_32
 * ��������  ��LCD��Ļ��ʾ32*32�ĺ��ֺ��ַ����
 * ��������  ��uint16_t x,uint16_t y,uint16_t color,uint8_t *str,uint8_t mode 
 * ��������ֵ����
 * ��������  ��				
 */
void lcd_flash_show_32(uint16_t x,uint16_t y,uint8_t *str,uint16_t f_color,uint16_t b_color)
{
	while(*str != '\0')
	{
		//�ж����ַ�
		if(*str <= 127)
		{
			lcd_flash_show_32ch(x,y,*str,f_color,b_color);
			str++;
			x += 16;
			if(x >= LCD_W-16)//����
			{
				x = 0;
				y += 32;
			}
		}
		//�ж��Ǻ���
		else
		{
			lcd_flash_show_32hz(x,y,str,f_color,b_color);
			str += 2;
			x += 32;
			if(x >= LCD_W-32)
			{
				x = 0;
				y += 32;
			}
		}
		
	}
	
}











