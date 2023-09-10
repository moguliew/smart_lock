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


#define LCD_SPI_SCL_H 		GPIO_SetBits(GPIOB, GPIO_Pin_13)
#define LCD_SPI_SCL_L 		GPIO_ResetBits(GPIOB, GPIO_Pin_13)
#define LCD_MOSI_H 	    	GPIO_SetBits(GPIOB, GPIO_Pin_15)
#define LCD_MOSI_L 			GPIO_ResetBits(GPIOB, GPIO_Pin_15)
#define LCD_RES_H 			GPIO_SetBits(GPIOB, GPIO_Pin_10)
#define LCD_RES_L 			GPIO_ResetBits(GPIOB, GPIO_Pin_10)
#define LCD_CS_H	  		GPIO_SetBits(GPIOB,GPIO_Pin_12)
#define LCD_CS_L	  		GPIO_ResetBits(GPIOB,GPIO_Pin_12)
#define LCD_DATA	  		GPIO_SetBits(GPIOB,GPIO_Pin_14)
#define LCD_CMD     		GPIO_ResetBits(GPIOB,GPIO_Pin_14)
#define LCD_LED_ON	  		GPIO_SetBits(GPIOB,GPIO_Pin_1)
#define LCD_LED_OFF	  		GPIO_ResetBits(GPIOB,GPIO_Pin_1)

typedef struct _HEADCOLOR
{
   unsigned char scan;
   unsigned char gray;
   unsigned short w;
   unsigned short h;
   unsigned char is565;
   unsigned char rgb;
}HEADCOLOR; 


void lcd_init(void);
void lcd_clear(uint16_t color);
void lcd_xy_clear(uint16_t x,uint16_t y,uint16_t w,uint16_t h,uint16_t color);
void lcd_circle(uint16_t x,uint16_t y,uint16_t r,uint16_t color);
void lcd_solid_circle(uint16_t x,uint16_t y,uint16_t r,uint16_t color);
void lcd_show_16ch(uint16_t x,uint16_t y,uint8_t ch,uint16_t f_color,uint16_t b_color);
void lcd_show_32ch(uint16_t x,uint16_t y,uint8_t ch,uint16_t f_color,uint16_t b_color);
void lcd_show_16hz(uint16_t x,uint16_t y,uint8_t *str,uint16_t f_color,uint16_t b_color);
void lcd_show_32hz(uint16_t x,uint16_t y,uint8_t *str,uint16_t f_color,uint16_t b_color);
void lcd_show_16(uint16_t x,uint16_t y,uint8_t *str,uint16_t f_color,uint16_t b_color);
void lcd_show_32(uint16_t x,uint16_t y,uint8_t *str,uint16_t f_color,uint16_t b_color);
void lcd_show_pic(uint16_t x,uint16_t y,const uint8_t *pic);


#endif




