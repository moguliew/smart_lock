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
	
	
	//检测是为第一次开机
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
		
		//如果是主界面
		if(page_mode == 1)
		{
			//密码开锁
			open_passward(bs8116_key);
			
			//指纹开锁
			Match_l_n();
		}
		
	
		//管理员功能界面
		else if(page_mode == 2)
		{
			if(page_flag == 1)
			{
				printf("[1]注册指纹\r\n");
				printf("[2]删除指定指纹\r\n");
				printf("[3]删除全部指纹\r\n");
				
				printf("[#]退出管理界面\r\n");
				
				page_flag = 0;
			}
		
			//注册指纹
			if(bs8116_key == '1')
			{
				//读出AT24C04的指纹ID到数组中
				AT24C04_read_bytes(121,sizeof(mg200_buff),mg200_buff);
				
				while(mg200_buff[mg200_i] != 0)
				{
					mg200_i++;
				}
				//出来循环下标就是要写入的位置,下标+1就是ID号
				mg200_ID = mg200_i+1;
				
				//注册到指纹模块
				mg200_Enroll_sta = Enroll(mg200_ID);
				//注册成功就将ID号存起来用于后期查询管理
				if(mg200_Enroll_sta == 0)    
				{
					mg200_buff[mg200_i] = mg200_ID;
					AT24C04_write_bytes(121,sizeof(mg200_buff),mg200_buff);
					mg200_i = 0;
				}
			}
			
			/*删除指定指纹*/
			else if(bs8116_key == '2')
			{
				//进入到删除指纹界面
				page_mode = 3;
			}
			
			//删除所有指纹
			else if(bs8116_key == '3')
			{
				mg200_erase_sta = erase_all();
				if(mg200_erase_sta == 0)
				{
					u8 erase_buff[9] = {0};
					AT24C04_write_bytes(121,sizeof(erase_buff),erase_buff); //重新写入到AT24中
					
				}
				
			}
			//退出管理员界面
			else if(bs8116_key == '#')
			{
				page_mode = 1;
				page_flag = 1;
			}
		}
		
		
		
		//删除指定指纹独立界面
		else if(page_mode == 3)
		{
			if(erase_one_flag == 1)
			{
				//读出AT24C04的指纹ID到数组中
				AT24C04_read_bytes(121,sizeof(mg200_buff),mg200_buff);
					
				//展示有效指纹ID
				for(u8 i=0;i<9;i++)
				{
					if(mg200_buff[i] != 0)
					{
						printf("mg200[%d]用户ID:%d\r\n",i,mg200_buff[i]);
					}	
				}
				printf("请输入要删除的ID号:\r\n");
				erase_one_flag = 0;
			}
			

			if(bs8116_key != 0xff && bs8116_key != '*' && bs8116_key != '#' && bs8116_key != '0')
			{
				mg200_erase_sta = erase_one(bs8116_key-48);
				if(mg200_erase_sta == 0)
				{
					mg200_buff[bs8116_key-48-1] = 0;     //清空数组删除的ID的元素内容
					AT24C04_write_bytes(121,sizeof(mg200_buff),mg200_buff); //重新写入到AT24中
					
				}
				
				//回到管理员界面
				page_mode = 2;      //为了回到2号界面程序
				erase_one_flag = 1; //下次显示有效指纹
				page_flag = 1;      //重回2号界面显示2号界面的内容
			}
		}
		
		
	}
	
	
	return 0;
}

















