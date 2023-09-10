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
#include "voice.h"
#include "string.h"

/**
 * 第一次开机标志：0xff	    存储地址：0x00    
 * 开锁密码存储地址：0x01  
 */
int main(void)
{

	uint8_t bs8116_key;
	
	usart1_init(115200);
	w25q64_init();
	key_init();
	led_init();
	voice_init();
	lcd_init();
	bs8116_init();
	at24c04_init();
	door_init();
	
	//at24c04_write_bytes(0,255,0);
	
	while(1)
	{	
		bs8116_key = bs8116_key_scan();
		
		//密码开锁
		open_passwd(bs8116_key);
		
		
		//指纹
		
	
		//刷卡
		
	}
	
	
	return 0;
}

















