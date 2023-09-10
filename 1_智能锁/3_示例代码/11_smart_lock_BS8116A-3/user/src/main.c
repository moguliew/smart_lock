#include "stm32f4xx.h"                  // Device header
#include "led.h" 
#include "key.h"
#include "door.h"
#include "usart.h"
#include "tim.h"
#include "lcd.h"
#include "font.h"
#include "at24c04.h"
#include "stdio.h"
#include "w25q64.h"
#include "nvic.h"
#include "bs8116.h"

int main(void)
{
	uint8_t key;
	usart1_init(115200);
	w25q64_init();
	key_init();
	led_init();
	lcd_init();
	bs8116_init();
	
	while(1)
	{	
		key = bs8116_key_scan();
		switch(key)
		{
			case '1':printf("1\r\n");break;
			case '2':printf("2\r\n");break;
			case '3':printf("3\r\n");break;     
			case '4':printf("4\r\n");break;
			case '5':printf("5\r\n");break;
			case '6':printf("6\r\n");break;
			case '7':printf("7\r\n");break;
			case '8':printf("8\r\n");break;
			case '9':printf("9\r\n");break;
			case '*':printf("*\r\n");break;
			case '0':printf("0\r\n");break;
			case '#':printf("#\r\n");break;
		}
		
	}
	
	return 0;
}

















