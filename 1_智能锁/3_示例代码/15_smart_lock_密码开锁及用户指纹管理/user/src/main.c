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
	
	
	//�����Ϊ��һ�ο���
	AT24C04_read_byte(120,&mg200_open_flag);
	if(mg200_open_flag != 0xfd)
	{
		AT24C04_write_bytes(121,sizeof(mg200_buff),mg200_buff);
		AT24C04_write_byte(120,0xfd);
		erase_all();
	}
	
	
	while(1)
	{	
		bs8116_key = BS8116_Key_scan();
		
		//�����������
		if(page_mode == 1)
		{
			//���뿪��
			open_passward(bs8116_key);
			
			//ָ�ƿ���
			Match_l_n();
		}
		
	
		//����Ա���ܽ���
		else if(page_mode == 2)
		{
			if(page_flag == 1)
			{
				printf("[1]ע��ָ��\r\n");
				printf("[2]ɾ��ָ��ָ��\r\n");
				printf("[3]ɾ��ȫ��ָ��\r\n");
				
				printf("[#]�˳��������\r\n");
				
				page_flag = 0;
			}
		
			//ע��ָ��
			if(bs8116_key == '1')
			{
				//����AT24C04��ָ��ID��������
				AT24C04_read_bytes(121,sizeof(mg200_buff),mg200_buff);
				
				while(mg200_buff[mg200_i] != 0)
				{
					mg200_i++;
				}
				//����ѭ���±����Ҫд���λ��,�±�+1����ID��
				mg200_ID = mg200_i+1;
				
				//ע�ᵽָ��ģ��
				mg200_Enroll_sta = Enroll(mg200_ID);
				//ע��ɹ��ͽ�ID�Ŵ��������ں��ڲ�ѯ����
				if(mg200_Enroll_sta == 0)    
				{
					mg200_buff[mg200_i] = mg200_ID;
					AT24C04_write_bytes(121,sizeof(mg200_buff),mg200_buff);
					mg200_i = 0;
				}
			}
			
			/*ɾ��ָ��ָ��*/
			else if(bs8116_key == '2')
			{
				//���뵽ɾ��ָ�ƽ���
				page_mode = 3;
			}
			
			//ɾ������ָ��
			else if(bs8116_key == '3')
			{
				mg200_erase_sta = erase_all();
				if(mg200_erase_sta == 0)
				{
					u8 erase_buff[9] = {0};
					AT24C04_write_bytes(121,sizeof(erase_buff),erase_buff); //����д�뵽AT24��
					
				}
				
			}
			//�˳�����Ա����
			else if(bs8116_key == '#')
			{
				page_mode = 1;
				page_flag = 1;
			}
		}
		
		
		
		//ɾ��ָ��ָ�ƶ�������
		else if(page_mode == 3)
		{
			if(erase_one_flag == 1)
			{
				//����AT24C04��ָ��ID��������
				AT24C04_read_bytes(121,sizeof(mg200_buff),mg200_buff);
					
				//չʾ��Чָ��ID
				for(u8 i=0;i<9;i++)
				{
					if(mg200_buff[i] != 0)
					{
						printf("mg200[%d]�û�ID:%d\r\n",i,mg200_buff[i]);
					}	
				}
				printf("������Ҫɾ����ID��:\r\n");
				erase_one_flag = 0;
			}
			

			if(bs8116_key != 0xff && bs8116_key != '*' && bs8116_key != '#' && bs8116_key != '0')
			{
				mg200_erase_sta = erase_one(bs8116_key-48);
				if(mg200_erase_sta == 0)
				{
					mg200_buff[bs8116_key-48-1] = 0;     //�������ɾ����ID��Ԫ������
					AT24C04_write_bytes(121,sizeof(mg200_buff),mg200_buff); //����д�뵽AT24��
					
				}
				
				//�ص�����Ա����
				page_mode = 2;      //Ϊ�˻ص�2�Ž������
				erase_one_flag = 1; //�´���ʾ��Чָ��
				page_flag = 1;      //�ػ�2�Ž�����ʾ2�Ž��������
			}
		}
		
		
	}
	
	
	return 0;
}

















