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
	

	u8 picc_mode = 0;
	u8 val_buff[16] = {0};
	while(1)
	{	
		bs8116_key = BS8116_Key_scan();
		
		if(bs8116_key == '0')
		{
			//识别卡片开锁
			printf("请放置卡片,识别开锁\r\n");
			picc_mode = 0;
		}
		if(bs8116_key == '1')
		{
			//注册卡片
			printf("请放置卡片,准备注册...\r\n");
			picc_mode = 1;
		}
		else if(bs8116_key == '2')
		{
			//删除卡片
			printf("请选择要删除的卡片\r\n");
			picc_mode = 2;
		}
		
		if(picc_mode == 0)
		{
			//识别开锁
			PICC_clock();	
		}
		else if(picc_mode == 1)
		{
			//注册
			Enroll_PICC();
		}
		else if(picc_mode == 2)
		{
			//删除
			
		}
		
		
		
		
		
			
	}
	
	return 0;
}














