#include "stm32f4xx.h"                  // Device header

/***********************************************
*函数名    ：led_init
*函数功能  ：对LED灯所用管脚初始化配置
*函数参数  ：无
*函数返回值：无
*函数描述  ：PB8-----LED1
             PB9-----LED2
***********************************************/
void led_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	//端口时钟使能
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;          //通用输出
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;         //推挽
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9; // 8 9
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;       //无上下拉
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;       //速度
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	//输出数据寄存器配置----灯起始关
	GPIO_SetBits(GPIOB, GPIO_Pin_8);
	GPIO_SetBits(GPIOB, GPIO_Pin_9);
}























