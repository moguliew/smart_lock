#include "stm32f4xx.h"                  // Device header
#include "led.h" 
#include "key.h"
#include "door.h"
#include "usart.h"
#include "timer.h"
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
#include "rfid.h"
#include "esp32.h"

static void delay_ms(u32 ms)
{
	u32 i= 84 / 4 * 1000 * ms;
	while(i)
	{
		i--;
	}
}


/***********************************************
*密码第一次开机标志 100  0xfe
*密码存储地址       101~111
*指纹第一次开机标志 120  0xfd  
*指纹ID存储地址     121~132  
************************************************/


int main(void)
{

	u8 bs8116_key;
	u8 page_flag = 1;
	u8 erase_one_flag = 1;
	
	u8 mg200_buff[9]={0};
	u8 mg200_open_flag = 0;
	u8 mg200_i = 0;
	u8 mg200_ID = 0;
	
	u8 mg200_Enroll_sta = 0;
	u8 mg200_erase_sta = 0;
	
	usart1_init(115200);
	w25q64_init();
	key_init();
	led_init();
	//字库更新  (按着按键2再按复位键)
	zk_updata();
	voice_init();
	LCD_init();
	BS8116_init();
	at24c04_init();
	door_init();
	mg200_init();
	RFID_Init();
	Esp32_Init();
	Esp32_Wificonnect("LL","12345678");
	Esp32_TCPconnect("192.168.137.111",10000);
	

	while(1)
	{	
		usart2_esp32_send_str("1");
		delay_ms(500);
	
	}
	
	return 0;
}














