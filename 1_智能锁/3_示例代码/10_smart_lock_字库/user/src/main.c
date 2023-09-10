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


int main(void)
{
	usart1_init(115200);
	w25q64_init();
	key_init();
	led_init();
	lcd_init();
	
	//字库更新 (按着按键2再按复位键)
	zk_update();
	
	lcd_flash_show_32(50,50,(uint8_t *)"hello 世界",RED,WHITE);
	
	while(1)
	{	
		printf("进入while\r\n");
		tim5_delay_ms(1000);
	}
	
	return 0;
}

















