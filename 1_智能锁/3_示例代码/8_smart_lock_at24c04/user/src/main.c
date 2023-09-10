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

int main(void)
{
	uint8_t str_send[] = "今天是2022年6月8日hello";
	uint8_t str_rec[30] = {0};
	
	usart1_init(115200);
	at24c04_init();
	
	at24c04_write_bytes(0,sizeof(str_send),str_send);
	at24c04_read_bytes(0,sizeof(str_rec),str_rec);
	
	
	while(1)
	{	
		printf("rec:%s\r\n",str_rec);
		tim5_delay_ms(500);
	}
	
	
	return 0;
}

















