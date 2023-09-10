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
*�����һ�ο�����־ 100  0xfe
*����洢��ַ       101~111
*ָ�Ƶ�һ�ο�����־ 120  0xfd  
*ָ��ID�洢��ַ     121~132  
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
	//�ֿ����  (���Ű���2�ٰ���λ��)
	zk_updata();
	voice_init();
	LCD_init();
	BS8116_init();
	at24c04_init();
	door_init();
	mg200_init();
	RFID_Init();
	

	u8 picc_id[4] = {0};
	u8 str_w[16] = "QQ�Ŀ�Ƭ";
	u8 str_r[16] = {0};
	u8 picc_mode = 0;
	while(1)
	{	
		bs8116_key = BS8116_Key_scan();
		
		if(bs8116_key == '1')
		{
			picc_mode = 1;
			printf("����뿨Ƭ׼��д��\r\n");
		}
		else if(bs8116_key == '2')
		{
			picc_mode = 2;
			printf("����뿨Ƭ׼������\r\n");
		}
		else if(bs8116_key == '0')
		{
			picc_mode = 0;
			printf("����뿨Ƭ׼��ʶ��Ƭ\r\n");
		}
		
		
		if(picc_mode == 0)
		{
			PCD_distinguish_PICC(picc_id,1);
		}
		if(picc_mode == 1)
		{
			WriteCardData(1,str_w);
		}
		else if(picc_mode == 2)
		{
			ReadCardData(1,str_r);
		}
		
	}
	
	return 0;
}














