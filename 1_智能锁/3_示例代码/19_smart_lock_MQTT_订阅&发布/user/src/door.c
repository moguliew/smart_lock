#include "stm32f4xx.h"                  // Device header

static void delay_ms(u32 ms)
{
	u32 i= 84 / 4 * 1000 * ms;
	while(i)
	{
		i--;
	}
}
/***********************************************
*函数名    ：door_init
*函数功能  ：门锁电机所用管脚初始化配置
*函数参数  ：无
*函数返回值：无
*函数描述  ：PB3
             PB4
***********************************************/
void door_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	//端口时钟使能
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;          //通用输出
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;         //推挽
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4; // 8 9
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;       //无上下拉
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;       //速度
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	//输出数据寄存器配置----灯起始关
	GPIO_SetBits(GPIOB, GPIO_Pin_3);
	GPIO_SetBits(GPIOB, GPIO_Pin_4);
}




/***********************************************
*函数名    ：open_door
*函数功能  ：开门函数
*函数参数  ：无
*函数返回值：无
*函数描述  ：
***********************************************/
void open_door(void)
{
	GPIO_SetBits(GPIOB, GPIO_Pin_3);
	GPIO_ResetBits(GPIOB, GPIO_Pin_4);
	delay_ms(300);
	GPIO_SetBits(GPIOB, GPIO_Pin_3);
	GPIO_SetBits(GPIOB, GPIO_Pin_4);
}





/***********************************************
*函数名    ：close_door
*函数功能  ：开门函数
*函数参数  ：无
*函数返回值：无
*函数描述  ：
***********************************************/
void close_door(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_3);
	GPIO_SetBits(GPIOB, GPIO_Pin_4);
	delay_ms(300);
	GPIO_SetBits(GPIOB, GPIO_Pin_3);
	GPIO_SetBits(GPIOB, GPIO_Pin_4);
}






