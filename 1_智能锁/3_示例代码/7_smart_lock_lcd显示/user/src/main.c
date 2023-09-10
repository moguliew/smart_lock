#include "stm32f4xx.h"                  // Device header
#include "led.h" 
#include "key.h"
#include "door.h"
#include "usart.h"
#include "tim.h"
#include "lcd.h"

extern const unsigned char gImage_pic[115208];
extern const unsigned char gImage_pic1[115208];
extern const unsigned char gImage_pic4[115208];

int main(void)
{
	lcd_init();
	
//	lcd_show_16ch(10,10,'A',BLACK,WHITE);
//	lcd_show_32ch(10,30,'A',BLACK,WHITE);
//	lcd_show_16hz(10,70,(uint8_t *)"信",BLACK,WHITE);
//	lcd_show_32hz(10,90,(uint8_t *)"信",BLACK,WHITE);
//	lcd_show_16(10,130,(uint8_t *)"hello信盈达",BLACK,WHITE);
//	lcd_show_32(10,150,(uint8_t *)"hello信盈达",BLACK,WHITE);

//	lcd_show_pic(0,0,gImage_pic4);
	
	
	while(1)
	{
		;
	}
	
	return 0;
}

















