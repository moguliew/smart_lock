#include "stm32f4xx.h"                  // Device header
#include "key.h"
static void delay_ms(u32 ms)
{
	u32 i= 84 / 4 * 1000 * ms;
	while(i)
	{
		i--;
	}
}


/***********************************************
*函数名    ：key_init
*函数功能  ：对按键所用管脚初始化配置
*函数参数  ：无
*函数返回值：无
*函数描述  ：PC13------KEY
***********************************************/
void key_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	//端口时钟使能
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;          //通用输入
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;              //13
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;       //无上下拉
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	
}



/***********************************************
*函数名    ：key_scan
*函数功能  ：按键扫描函数
*函数参数  ：无
*函数返回值：u8
*函数描述  ：返回1表示按键按下，返回0xff没有按键
***********************************************/
u8 key_scan(void)
{
	static u8 flag = 1;
	u8 key_val = 0xff;
	
	if(!KEY  && flag)
	{
		delay_ms(10);
		if(!KEY)
		{
			flag = 0;
			key_val = 1;
		}
	}
	
	
	if(KEY)
	{
		flag = 1;
	}
	
	return key_val;
}









