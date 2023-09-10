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
#include "mqtt.h"
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
	u8 errorCode;
	
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
	
	LCD_flash_dis_16(10,10,RED,RED,"3443D43FD简单快捷入口发",0);
	
	Esp32_Wificonnect("LL","12345678");
	
	
	 errorCode = Esp32_TCPconnect("hepb7KMhWuB.iot-as-mqtt.cn-shanghai.aliyuncs.com",1883);
	
	
	 
	if(!errorCode)
	{
		_mqtt.Init(rxbuf,0,txbuf,0);
		
		//MQTT连接
		errorCode=_mqtt.Connect("esp32|securemode=3,signmethod=hmacsha1|",
		"smart_lock_XYD&hepb7KMhWuB",
		"06422408CFE4AD22C3D8C4CC47FDE5DE12D29D73");		
		if(errorCode==1)
		{
			printf("MQTT接入成功\r\n");
			errorCode=_mqtt.SubscribeTopic("/sys/hepb7KMhWuB/smart_lock_XYD/thing/service/property/set",0,1);//订阅
			if(errorCode==1)
			{
				printf("MQTT订阅成功\r\n");
			}
		}
	}
	
	
	//发布
	char string[520] = {0};
	
	u8 str[10] = "123456";
	u8 status = 1;
	u8 test = 88;
	
	
	sprintf(string, "{\"method\":\"thing.service.property.set\",\"id\":\"303695681\",\"params\":{\"lock_password\":\"%s\",\"lock_status\":%d,\"lock_test\":%d},\"version\":\"1.0.0\"}",str,status,test);

	_mqtt.PublishData("/sys/hepb7KMhWuB/smart_lock_XYD/thing/event/property/post",string,0);
	
	
	

	while(1)
	{	
		if(esp32rec.flag == 1)
		{
			esp32rec.flag = 0;
			printf("接收:%s\r\n",esp32rec.buff);
			
		}
	
	}
	
	return 0;
}














