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
#include "mg200.h"

/**
 * 第一次开机标志：0xff   	存储地址：0x00
 * 开锁密码存储地址：0x01  
 */
int main(void)
{

	u8 bs8116_key;
	
	usart1_init(115200);
	w25q64_init();
	key_init();
	led_init();
	zk_update();        //字库更新  (按着按键2再按复位键)
	voice_init();
	lcd_init();
	bs8116_init();
	at24c04_init();
	door_init();
	mg200_init();
	
	
	while(1)
	{	
		bs8116_key = bs8116_key_scan();
		//按键1注册指纹
		if(bs8116_key == '1')
		{
			Enroll();
		}
		else if(bs8116_key == '2')
		{
			erase_all();
		}

		//指纹识别开锁
		//Match_l_n();
	
	}
	
	
	return 0;
}

















