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
	

	u8 picc_mode = 0;
	u8 val_buff[16] = {0};
	while(1)
	{	
		bs8116_key = BS8116_Key_scan();
		
		if(bs8116_key == '0')
		{
			//ʶ��Ƭ����
			printf("����ÿ�Ƭ,ʶ����\r\n");
			picc_mode = 0;
		}
		if(bs8116_key == '1')
		{
			//ע�ῨƬ
			printf("����ÿ�Ƭ,׼��ע��...\r\n");
			picc_mode = 1;
		}
		else if(bs8116_key == '2')
		{
			//ɾ����Ƭ
			printf("��ѡ��Ҫɾ���Ŀ�Ƭ\r\n");
			picc_mode = 2;
		}
		
		if(picc_mode == 0)
		{
			//ʶ����
			PICC_clock();	
		}
		else if(picc_mode == 1)
		{
			//ע��
			Enroll_PICC();
		}
		else if(picc_mode == 2)
		{
			//ɾ��
			
		}
		
		
		
		
		
			
	}
	
	return 0;
}














