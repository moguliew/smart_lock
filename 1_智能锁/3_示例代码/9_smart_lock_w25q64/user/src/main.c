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


int main(void)
{
	uint8_t str_send[] = "今天是2022年6月8日hello";
	uint8_t str_rec[30] = {0};
	
	usart1_init(115200);
	w25q64_init();
	
	printf("开始擦除\r\n");
	w25q64_chip_erase();
	printf("擦除完成\r\n");
	
	w25q64_page_write(0x011101,sizeof(str_send),str_send);
	
	w25q64_read(0x011101,sizeof(str_send),str_rec);
	
	
	while(1)
	{	
		printf("rec:%s\r\n",str_rec);	
		tim5_delay_ms(500);
	}
	
	
	return 0;
}

















