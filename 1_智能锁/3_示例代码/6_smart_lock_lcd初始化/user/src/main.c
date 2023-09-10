#include "stm32f4xx.h"                  // Device header
#include "led.h" 
#include "key.h"
#include "door.h"
#include "usart.h"
#include "tim.h"
#include "lcd.h"

int main(void)
{
	lcd_init();
	
	//lcd_xy_clear(50,50,100,100,YELLOW);
	//lcd_circle(120,120,50,RED);
	lcd_solid_circle(120,120,50,RED);
	
	while(1)
	{
		;
	}
	
	return 0;
}

















