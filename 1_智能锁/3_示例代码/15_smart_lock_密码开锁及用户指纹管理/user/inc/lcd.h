#ifndef _LCD_H
#define _LCD_H
#include "stm32f4xx.h"

#define LCD_W  240
#define LCD_H  240


/* ��ɫ���忪ʼ */
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE           	 0x001F  
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40 //��ɫ
#define BRRED 			 0XFC07 //�غ�ɫ
#define GRAY  			 0X8430 //��ɫ
#define DARKBLUE      	 0X01CF	//����ɫ
#define LIGHTBLUE      	 0X7D7C	//ǳ��ɫ  
#define GRAYBLUE       	 0X5458 //����ɫ
#define LIGHTGREEN     	 0X841F //ǳ��ɫ
#define LGRAY 			 0XC618 //ǳ��ɫ(PANNEL),���屳��ɫ
#define LGRAYBLUE        0XA651 //ǳ����ɫ(�м����ɫ)
#define LBBLUE           0X2B12 //ǳ����ɫ(ѡ����Ŀ�ķ�ɫ)
/* ��ɫ������� */





//ͼƬͷ��Ϣ�Ľṹ������
typedef struct _HEADCOLOR
{
	 unsigned char scan;   //ɨ�跽��
	 unsigned char gray;   //�Ҷ�ֵ
	 unsigned short w;     //��
	 unsigned short h;     //��
	 unsigned char is565;  // 5 6 5 
	 unsigned char rgb;    //RGB
}HEADCOLOR;   









#define LCD_SPI_SCL_H 	GPIO_SetBits(GPIOB, GPIO_Pin_13)
#define LCD_SPI_SCL_L 	GPIO_ResetBits(GPIOB, GPIO_Pin_13)
#define LCD_MOSI_H 	GPIO_SetBits(GPIOB, GPIO_Pin_15)
#define LCD_MOSI_L 	GPIO_ResetBits(GPIOB, GPIO_Pin_15)
#define LCD_RES_H 	GPIO_SetBits(GPIOB, GPIO_Pin_10)
#define LCD_RES_L 	GPIO_ResetBits(GPIOB, GPIO_Pin_10)
#define LCD_CS_H	  GPIO_SetBits(GPIOB,GPIO_Pin_12)
#define LCD_CS_L	  GPIO_ResetBits(GPIOB,GPIO_Pin_12)
#define LCD_DATA	  GPIO_SetBits(GPIOB,GPIO_Pin_14)
#define LCD_CMD     GPIO_ResetBits(GPIOB,GPIO_Pin_14)
#define LCD_LED_ON	  GPIO_SetBits(GPIOB,GPIO_Pin_1)
#define LCD_LED_OFF	  GPIO_ResetBits(GPIOB,GPIO_Pin_1)


void LCD_init(void);
void LCD_clear(u16 color);
void LCD_xy_clear(u16 x,u16 y,u16 w,u16 h,u16 color);
void LCD_Ring(u16 x,u16 y,u16 r,u16 colour);
void LCD_Ring_shi(u16 x,u16 y,u16 r,u16 colour);
void LCD_Point(u16 x,u16 y,u16 color);
void LCD_dis_16ch(u16 x,u16 y,u16 color,u8 ch,u8 mode,u16 b_color);
void LCD_dis_32ch(u16 x,u16 y,u16 color,u8 ch,u8 mode,u16 b_color);
void LCD_dis_16hz(u16 x,u16 y,u16 color,u8 *str,u8 mode,u16 b_color);
void LCD_dis_32hz(u16 x,u16 y,u16 color,u8 *str,u8 mode,u16 b_color);
void LCD_dis_16(u16 x,u16 y,u16 color,u8 *str,u8 mode,u16 b_color );
void LCD_dis_32(u16 x,u16 y,u16 color,u8 *str,u8 mode,u16 b_color );
void LCD_dis_pic(u16 x,u16 y,const u8 *pic);


void LCD_flash_dis_16(u16 x,u16 y,u16 color,u16 b_color,u8 *str,u8 mode);
void LCD_flash_dis_32(u16 x,u16 y,u16 color,u16 b_color,u8 *str,u8 mode);

#endif




