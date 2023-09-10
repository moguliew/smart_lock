#include "stm32f4xx.h"                  // Device header
#include "lcd.h"
#include "timer.h"
#include "font.h"
/**************************************************
*������    ��LCD_spi_IO_init
*��������  ��ģ��SPI�Ĺܽų�ʼ����
*��������  ����
*��������ֵ����
*��������  ��LCD_CLK----------------------------PB13     
						 LCD_SDA----------------------------PB15        
****************************************************/
void LCD_spi_IO_init(void)
{
	//GPIO��ʼ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//1.��ʱ��
	
	//�������
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin =  GPIO_Pin_13 | GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Fast_Speed;
	GPIO_Init(GPIOB, &GPIO_InitStruct);	
	
}




/**************************************************
*������    ��LCD_IO_init
*��������  ��LCD�Ĺܽų�ʼ����
*��������  ����
*��������ֵ����
*��������  �� LEDK-----PB1        ���������        //ͨ�����
							RESET----PB10        ��λ����          //ͨ�����
							LCD_CS---PB12        Ƭѡ����          //ͨ�����	
							LCD_D/C--PB14        ��������ѡ����    //ͨ�����		  
****************************************************/
void LCD_IO_init(void)
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



/**************************************************
*������    ��LCD_spi_byte
*��������  ��SPI����һ���ֽں���
*��������  ����
*��������ֵ����
*��������  ��    
****************************************************/
void LCD_spi_byte(u8 data)
{
	u8 val ;
	u8 i;
	//����
	LCD_SPI_SCL_L;
	LCD_MOSI_H;
	/*��������*/
	for(i=0;i<8;i++)
	{
		LCD_SPI_SCL_L;    					
		if(data & 0x80) LCD_MOSI_H; else LCD_MOSI_L;
		LCD_SPI_SCL_H;   
		data = data << 1;
	}	
}



/**************************************************
*������    ��st7789vm_write_command
*��������  ����LCDд�����
*��������  ��u8 cmd
*��������ֵ����
*��������  ��    
****************************************************/
void st7789vm_write_command(u8 cmd)
{
	LCD_CS_L;//������Ƭѡ
	LCD_CMD;//��ʾ���͵�������
	LCD_spi_byte(cmd);//���巢�͵�8λ ����
	LCD_CS_H;//����Ƭѡ
}


/**************************************************
*������    ��st7789vm_write_command_parameter
*��������  ����LCDд�����������
*��������  ��u8 cmd
*��������ֵ����
*��������  ��    
****************************************************/
void st7789vm_write_command_parameter(u8 cmd_data)
{
	LCD_CS_L;//������Ƭѡ
	LCD_DATA;//��ʾ���͵�������
	LCD_spi_byte(cmd_data);//���巢�͵�8λ �������
	LCD_CS_H;//����Ƭѡ
}



/**************************************************
*������    ��st7789vm_write_data
*��������  ����LCDд���ݺ���
*��������  ��u16 data
*��������ֵ����
*��������  ������ֵ����ɫֵ   
****************************************************/
void st7789vm_write_data(u16 data)
{
	LCD_CS_L;//������Ƭѡ
	LCD_DATA;//��ʾ���͵�������
	LCD_spi_byte(data>>8);//���巢�͵�8λ �������
	LCD_spi_byte(data & 0x00ff);
	LCD_CS_H;//����Ƭѡ
}


/**************************************************
*������    ��st7789vm_rest
*��������  ����LCD��λ����
*��������  ����
*��������ֵ����
*��������  ��  
****************************************************/
void st7789vm_rest(void)
{
	LCD_RES_L;
	Tim5_delay_ms(100);
	LCD_RES_H;
	Tim5_delay_ms(100);
}





/**************************************************
*������    ��LCD_clear
*��������  ������
*��������  ��u16 color
*��������ֵ����
*��������  ��  
****************************************************/
void LCD_clear(u16 color)
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
	for(u32 i=0;i<LCD_W*LCD_H;i++)
	{
		st7789vm_write_data(color);
	}
}








/**************************************************
*������    ��LCD_init
*��������  ����LCD��ʼ����
*��������  ����
*��������ֵ����
*��������  ��  
****************************************************/
void LCD_init(void)
{
	//LCD_IO��ʼ��
	LCD_IO_init();
	//SPI_IO��ʼ��
	LCD_spi_IO_init();
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
	LCD_clear(0x0000);
}





/**************************************************
*������    ��LCD_xy_clear
*��������  ����ĳ������
*��������  ��u16 x,u16 y,u16 w,u16 h,u16 color
*��������ֵ����
*��������  ��  
****************************************************/
void LCD_xy_clear(u16 x,u16 y,u16 w,u16 h,u16 color)
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
	for(u32 i=0;i<w*h;i++)
	{
		st7789vm_write_data(color);
	}
}






/**************************************************
*������    ��LCD_Point
*��������  ����㺯��
*��������  ��u16 x,u16 y,u16 color
*��������ֵ����
*��������  ��  
****************************************************/
void LCD_Point(u16 x,u16 y,u16 color)
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


/******************************************************************************
*������    ��LCD_Ring
*��������  ��LCD��Ļ��Բ����
*��������  ��u16 x,u16 y,u16 r,u16 colour 
*��������ֵ����
*��������  ��
						��x - x0��^2 + (y - y0)^2  == r*r 
*****************************************************************************************/
void LCD_Ring(u16 x,u16 y,u16 r,u16 colour)
{
	u16 i,j;
	
	for(i=x-r;i<=x+r;i++)         //����������
	{
		for(j=y-r;j<=y+r;j++)       //����������
		{
			if((i-x)*(i-x) + (j-y)*(j-y) >=(r-3)*(r-3) && (i-x)*(i-x) + (j-y)*(j-y) <=r*r)   //�жϱ����ĵ��Ƿ��������
			{
				//��㺯��
				LCD_Point(i,j,colour); //���������ĵ�����
			}
		}
	}
}


/******************************************************************************
*������    ��LCD_Ring_shi
*��������  ��LCD��Ļ��Բ����ʵ��Բ
*��������  ��u16 x,u16 y,u16 r,u16 colour 
*��������ֵ����
*��������  ��
						��x - x0��^2 + (y - y0)^2  <= r*r 
*****************************************************************************************/
void LCD_Ring_shi(u16 x,u16 y,u16 r,u16 colour)
{
	u16 i,j;
	
	for(i=x-r;i<=x+r;i++)         //����������
	{
		for(j=y-r;j<=y+r;j++)       //����������
		{
			if((i-x)*(i-x) + (j-y)*(j-y) <=r*r)   //�жϱ����ĵ��Ƿ��������
			{
				//��㺯��
				LCD_Point(i,j,colour); //���������ĵ�����
			}
		}
	}
}




/******************************************************************************
*������    ��LCD_dis_16ch
*��������  ��LCD��Ļ��ʾ16*16���ַ�
*��������  ��u16 x,u16 y,u16 color��u8 ch��u8 mode,u16 b_color 
*��������ֵ����
*��������  ��
						mode ����1  �ʹ�������ɫ
						mode ����0  �Ͳ���������ɫ
********************************************************************************/
void LCD_dis_16ch(u16 x,u16 y,u16 color,u8 ch,u8 mode,u16 b_color)
{
	u8 n,i,j;
	u8 temp;
	
	/*����Ҫ��ʾ���ַ���ո��ƫ��ֵ n*/
	n = ch - ' ';
	
	/*��ʾ*/
	for(i=0;i<16;i++)     //��һ��ռ������
	{
		//��Ҫ��ʾ���ַ���ģ����ȡ�����ŵ�һ��������
		temp = ASC16[n*16+i];
		for(j=0;j<8;j++)   //ÿ�б������ص����
		{
			if(temp & (0x80 >> j))
			{
				LCD_Point(x+j,y+i,color);
			}
			else   //����ɫ
			{
				if(mode == 1)
				{
					LCD_Point(x+j,y+i,b_color);
				}
			}
			
		}
	}
}



/******************************************************************************
*������    ��LCD_dis_32ch
*��������  ��LCD��Ļ��ʾ32*32���ַ�
*��������  ��u16 x,u16 y,u16 color��u8 ch ,u8 mode,u16 b_color
*��������ֵ����
*��������  ��16 * 32			
********************************************************************************/
void LCD_dis_32ch(u16 x,u16 y,u16 color,u8 ch,u8 mode,u16 b_color)
{
	u8 n,i,j;
	u16 temp;
	
	/*����Ҫ��ʾ���ַ���ո��ƫ��ֵ n*/
	n = ch - ' ';
	
	/*��ʾ*/
	for(i=0;i<32;i++)
	{
		//��Ҫ��ʾ���ַ���ģ����ȡ�����ŵ�һ��������
		temp = (ASC32[n*64+2*i] << 8)  |  ASC32[n*64+2*i+1];
		      
		for(j=0;j<16;j++)
		{
			if(temp & (0x8000 >> j))
			{
				LCD_Point(x+j,y+i,color);
			}
			else    //����ɫ
			{
				if(mode  == 1)
				{
					LCD_Point(x+j,y+i,b_color);
				}
				
			}
			
		}
	}
}



/******************************************************************************
*������    ��LCD_dis_16hz
*��������  ��LCD��Ļ��ʾ16*16�ĺ���
*��������  ��u16 x,u16 y,u16 color��u8 *str ,u8 mode ,u16 b_color
*��������ֵ����
*��������  ��
						
********************************************************************************/
void LCD_dis_16hz(u16 x,u16 y,u16 color,u8 *str,u8 mode,u16 b_color)
{
	u8 n = 0;
	u8 i,j;
	u16 temp;
	
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
				LCD_Point(x+j,y+i,color);
			}
			else  //����ɫ
			{
				if(mode == 1)
				{
					LCD_Point(x+j,y+i,b_color);
				}
				
			}
			
		}
	}
}


/******************************************************************************
*������    ��LCD_dis_32hz
*��������  ��LCD��Ļ��ʾ32*32�ĺ���
*��������  ��u16 x,u16 y,u16 color��u8 *str,u8 mode,u16 b_color 
*��������ֵ����
*��������  ��
						
********************************************************************************/
void LCD_dis_32hz(u16 x,u16 y,u16 color,u8 *str,u8 mode,u16 b_color)
{
	u8 n = 0;
	u8 i,j;
	u32 temp;
	
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
				LCD_Point(x+j,y+i,color);
			}
			else  //����ɫ
			{
				if(mode == 1)
				{
					LCD_Point(x+j,y+i,b_color);
				}
			}
			
		}
	}
}



/******************************************************************************
*������    ��LCD_dis_16
*��������  ��LCD��Ļ��ʾ16*16�ĺ��ֺ��ַ����
*��������  ��u16 x,u16 y,u16 color,u8 *str,u8 mode,u16 b_color  
*��������ֵ����
*��������  ��mode 1 �б���ɫ
             mode 0 �ޱ���ɫ
********************************************************************************/
void LCD_dis_16(u16 x,u16 y,u16 color,u8 *str,u8 mode,u16 b_color)
{
	while(*str != '\0')
	{
		//�ж����ַ�
		if(*str>=0 && *str<=127)
		{
			LCD_dis_16ch(x,y,color,*str,mode,b_color);
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
			LCD_dis_16hz(x,y,color,str,mode,b_color);
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




/******************************************************************************
*������    ��LCD_dis_32
*��������  ��LCD��Ļ��ʾ32*32�ĺ��ֺ��ַ����
*��������  ��u16 x,u16 y,u16 color,u8 *str ,u8 mode,u16 b_color
*��������ֵ����
*��������  ��mode 1 �б���ɫ
             mode 0 �ޱ���ɫ				
********************************************************************************/
void LCD_dis_32(u16 x,u16 y,u16 color,u8 *str,u8 mode,u16 b_color )
{
	while(*str != '\0')
	{
		//�ж����ַ�
		if(*str>=0 && *str<=127)
		{
			LCD_dis_32ch(x,y,color,*str,mode,b_color);
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
			LCD_dis_32hz(x,y,color,str,mode,b_color);
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






/******************************************************************************
*������    ��LCD_dis_pic
*��������  ��LCD��Ļ��ʾͼƬ
*��������  ��u16 x,u16 y,u8 *pic 
*��������ֵ����
*��������  ��				
********************************************************************************/
void LCD_dis_pic(u16 x,u16 y,const u8 *pic)
{
	
	u32 i;
	HEADCOLOR *pic_head;     //�ṹ��ָ��
	u16 *p;
	
	//��ͼƬ��ͷ�����ó���
	pic_head = (HEADCOLOR *)pic;
	//����ͼƬһ���ж��ٸ����ص�
	i = (pic_head->w )  * (pic_head->h);
	
	//��ָ��pȥ�������е���ɫ����
	p = (u16 *)(pic + sizeof(HEADCOLOR));  //��ͷ����ƫ��
	
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













/******************************************************************************
*������    ��LCD_dis_flash_16ch
*��������  ��LCD��Ļ��ʾ16*16���ַ�
*��������  ��u16 x,u16 y,u16 color,u16 b_color,u8 ch,u8 mode
*��������ֵ����
*��������  ����¼��ַ��0x020000	
						 ASC16.bin             0x00000000             
						 ASC32.bin             0x00000806             
						 HZK16.bin             0x0000280C             
						 HZK32.bin             0x00042652 

						mode:1 �򱳾���ɫ
						mode:0 ���򱳾���ɫ��b_color���
********************************************************************************/
void LCD_dis_flash_16ch(u16 x,u16 y,u16 color,u16 b_color,u8 ch,u8 mode)
{
	u8 n,i,j;
	u8 temp;
	u8 buff[16] = {0};
	
	/*����Ҫ��ʾ���ַ���0��ƫ��ֵ n*/
	n = ch - 0;
	/*��w25q64�н�Ҫ��ʾ���ַ�����ģ����ȫ�������� 16��*/
	w25q64_read(0x020000+0+n*16,16,buff);
	
	
	/*��ʾ*/
	for(i=0;i<16;i++)
	{
		//��Ҫ��ʾ���ַ���ģ����ȡ�����ŵ�һ��������
		temp = buff[i];
		for(j=0;j<8;j++)
		{
			if(temp & (0x80 >> j))
			{
				LCD_Point(x+j,y+i,color);
			}
			else  //������ɫ
			{
				if(mode==1) LCD_Point(x+j,y+i,b_color);
			}
			
		}
	}
}





/******************************************************************************
*������    ��LCD_dis_flash_32ch
*��������  ��LCD��Ļ��ʾ32*32���ַ�
*��������  ��u16 x,u16 y,u16 color,u16 b_color,u8 ch 
*��������ֵ����
*��������  ����¼��ַ��0x020000	
						 ASC16.bin             0x00000000             
						 ASC32.bin             0x00000806             
						 HZK16.bin             0x0000280C             
						 HZK32.bin             0x00042652
						mode:1 �򱳾���ɫ
						mode:0 ���򱳾���ɫ��b_color���
********************************************************************************/
void LCD_dis_flash_32ch(u16 x,u16 y,u16 color,u16 b_color,u8 ch,u8 mode)
{
	u8 n,i,j;
	u16 temp;
	u8 buff[64] = {0};
	
	/*����Ҫ��ʾ���ַ���ո��ƫ��ֵ n*/
	n = ch - 0;
	/*��w25q64�н�Ҫ��ʾ���ַ�����ģ����ȫ�������� 16��*/
	w25q64_read(0x020000+0x00000806+n*64,64,buff);
	
	/*��ʾ*/
	for(i=0;i<32;i++)
	{
		//��Ҫ��ʾ���ַ���ģ����ȡ�����ŵ�һ��������
		temp = buff[2*i] << 8	 |  buff[2*i+1];
		for(j=0;j<16;j++)
		{
			if(temp & (0x8000 >> j))
			{
				LCD_Point(x+j,y+i,color);
			}
			else  //������ɫ
			{
				if(mode==1) LCD_Point(x+j,y+i,b_color);
			}
			
		}
	}
}




/******************************************************************************
*������    ��LCD_dis_flash_16hz
*��������  ��LCD��Ļ��ʾ16*16�ĺ���
*��������  ��u16 x,u16 y,u16 color,u16 b_color,u8 *str
*��������ֵ����
*��������  ����¼��ַ��0x020000	
						 ASC16.bin             0x00000000             
						 ASC32.bin             0x00000806             
						 HZK16.bin             0x0000280C             
						 HZK32.bin             0x00042652  
						mode:1 �򱳾���ɫ
						mode:0 ���򱳾���ɫ��b_color���
********************************************************************************/
void LCD_dis_flash_16hz(u16 x,u16 y,u16 color,u16 b_color,u8 *str,u8 mode)
{
	u32 n;
	u8 buff[32] = {0};
	u16 temp;
	u8 i,j;
	
	/*����Ҫ��ʾ�ĺ������һ�����ֵ�ƫ����*/
	n = (*str-0xA1) * 94 +  (*(str+1) - 0xA1);
	
	/*��w25q64�ж���Ҫ��ʾ�ĺ��ֵ���ģ���ݷ���һ��������*/
	w25q64_read(0x020000+0x0000280C+n*32,32,buff);

	/*�����ʾ*/
	for(i=0;i<16;i++)
	{
		//��ÿһ�е�����ȡ���� 2���ֽ�
		temp = buff[2*i]<<8 | buff[2*i+1];
		for(j=0;j<16;j++)
		{
			if(temp & (0x8000 >> j))
			{
				LCD_Point(x+j,y+i,color);
			}
			else
			{
				if(mode==1) LCD_Point(x+j,y+i,b_color);
			}
		}
		
	}
	
}




/******************************************************************************
*������    ��LCD_dis_flash_32hz
*��������  ��LCD��Ļ��ʾ32*32�ĺ���
*��������  ��u16 x,u16 y,u16 color,u16 b_color,u8 *str,u8 mode
*��������ֵ����
*��������  ����¼��ַ��0x020000	
						 ASC16.bin             0x00000000             
						 ASC32.bin             0x00000806             
						 HZK16.bin             0x0000280C             
						 HZK32.bin             0x00042652    
						mode:1 �򱳾���ɫ
						mode:0 ���򱳾���ɫ��b_color���
********************************************************************************/
void LCD_dis_flash_32hz(u16 x,u16 y,u16 color,u16 b_color,u8 *str,u8 mode)
{
	u32 n;
	u8 buff[128] = {0};
	u32 temp;
	u8 i,j;
	
	/*����Ҫ��ʾ�ĺ������һ�����ֵ�ƫ����*/
	n = (*str-0xA1) * 94 +  (*(str+1) - 0xA1);
	
	/*��w25q64�ж���Ҫ��ʾ�ĺ��ֵ���ģ���ݷ���һ��������*/
	w25q64_read(0x020000+0x00042652+n*128,128,buff);

	/*�����ʾ*/
	for(i=0;i<32;i++)
	{
		//��ÿһ�е�����ȡ���� 2���ֽ�
		temp = buff[4*i]<<24 | buff[4*i+1]<<16 | buff[4*i+2]<<8 | buff[4*i+3];
		for(j=0;j<32;j++)
		{
			if(temp & (0x80000000 >> j))
			{
				LCD_Point(x+j,y+i,color);
			}
			else
			{
				if(mode==1) LCD_Point(x+j,y+i,b_color);
			}
		}
		
	}
	
}




/******************************************************************************
*������    ��LCD_flash_dis_16
*��������  ��LCD��Ļ��ʾ16*16�ĺ��ֺ��ַ����
*��������  ��u16 x,u16 y,u16 color,u8 *str,u8 mode 
*��������ֵ����
*��������  ��				
********************************************************************************/
void LCD_flash_dis_16(u16 x,u16 y,u16 color,u16 b_color,u8 *str,u8 mode)
{
	while(*str != '\0')
	{
		//�ж����ַ�
		if(*str>=0 && *str<=127)
		{
			LCD_dis_flash_16ch(x,y,color,b_color,*str,mode);
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
			LCD_dis_flash_16hz(x,y,color,b_color,str,mode);
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



/******************************************************************************
*������    ��LCD_flash_dis_32
*��������  ��LCD��Ļ��ʾ32*32�ĺ��ֺ��ַ����
*��������  ��u16 x,u16 y,u16 color,u8 *str,u8 mode 
*��������ֵ����
*��������  ��				
********************************************************************************/
void LCD_flash_dis_32(u16 x,u16 y,u16 color,u16 b_color,u8 *str,u8 mode)
{
	while(*str != '\0')
	{
		//�ж����ַ�
		if(*str>=0 && *str<=127)
		{
			LCD_dis_flash_32ch(x,y,color,b_color,*str,mode);
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
			LCD_dis_flash_32hz(x,y,color,b_color,str,mode);
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





































