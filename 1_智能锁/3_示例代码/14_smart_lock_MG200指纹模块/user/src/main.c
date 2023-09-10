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
 * ��һ�ο�����־��0xff   	�洢��ַ��0x00
 * ��������洢��ַ��0x01  
 */
int main(void)
{

	u8 bs8116_key;
	
	usart1_init(115200);
	w25q64_init();
	key_init();
	led_init();
	zk_update();        //�ֿ����  (���Ű���2�ٰ���λ��)
	voice_init();
	lcd_init();
	bs8116_init();
	at24c04_init();
	door_init();
	mg200_init();
	
	
	while(1)
	{	
		bs8116_key = bs8116_key_scan();
		//����1ע��ָ��
		if(bs8116_key == '1')
		{
			Enroll();
		}
		else if(bs8116_key == '2')
		{
			erase_all();
		}

		//ָ��ʶ����
		//Match_l_n();
	
	}
	
	
	return 0;
}

















