#include "stm32f4xx.h"                  // Device header


/***********************************************
*函数名    ：Tim5_delay_ms
*函数功能  ：定时器5毫秒级延时函数
*函数参数  ：u32 ms
*函数返回值：无
*函数描述  ：APB1  84M    84分频  1次/us
***********************************************/
void Tim5_delay_ms(u32 ms)
{
	//定时器时钟使能
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);
	
	
	//定时器基本时基单元配置
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct = {0};
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;        //递增计数
	TIM_TimeBaseInitStruct.TIM_Period = 1 * 1000 * ms - 1;              //重载值
	TIM_TimeBaseInitStruct.TIM_Prescaler = 84 - 1;                      //84分频
	
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseInitStruct);
	//清除计数完成/更新标志Wie
	TIM_ClearFlag(TIM5,TIM_FLAG_Update);
	//设置单次还是连续
	TIM_SelectOnePulseMode(TIM5,TIM_OPMode_Single);         //单次
	
	//使能计数器
	TIM_Cmd(TIM5, ENABLE);
	//等待计数完成
	while(!(TIM_GetFlagStatus(TIM5,TIM_FLAG_Update)));
	
}




/***********************************************
*函数名    ：Tim5_delay_us
*函数功能  ：定时器5微秒级延时函数
*函数参数  ：u32 us
*函数返回值：无
*函数描述  ：APB1  84M    84分频  1次/us
***********************************************/
void Tim5_delay_us(u32 us)
{
	//定时器时钟使能
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);
	//定时器基本时基单元配置
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct = {0};
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;        //递增计数
	TIM_TimeBaseInitStruct.TIM_Period = 1 * us - 1;                    //重载值
	TIM_TimeBaseInitStruct.TIM_Prescaler = 84 - 1;                      //84分频
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseInitStruct);
	//清除计数完成/更新标志Wie
	TIM_ClearFlag(TIM5,TIM_FLAG_Update);
	//使能计数器
	TIM_Cmd(TIM5, ENABLE);
	//等待计数完成
	while(!(TIM_GetFlagStatus(TIM5,TIM_FLAG_Update)));
	
	//关闭定时器
	TIM_Cmd(TIM5, DISABLE);
}




/***********************************************
*函数名    ：Tim9_IT_ms
*函数功能  ：定时器9毫秒级中断
*函数参数  ：u32 ms
*函数返回值：无
*函数描述  ：APB2  84M    8400分频  10次/ms  
***********************************************/
void Tim9_IT_ms(u32 ms)
{
	/*定时器配置*/
	//时钟使能
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9,ENABLE);
	//基本时基单元配置
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct = {0};
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;        //递增计数
	TIM_TimeBaseInitStruct.TIM_Period = 10 * ms - 1;                    //重载值
	TIM_TimeBaseInitStruct.TIM_Prescaler = 8400 - 1;                      //84分频
	TIM_TimeBaseInit(TIM9, &TIM_TimeBaseInitStruct);
	//清除计数完成/更新 中断标志Wie
	TIM_ClearITPendingBit(TIM9,TIM_IT_Update);
	//更新中断使能
	TIM_ITConfig(TIM9,TIM_IT_Update,ENABLE);
	
	/*NVIC配置*/
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = TIM1_BRK_TIM9_IRQn ;    //中断源设置
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;      //响应通道使能
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;  //设置抢占优先级级别值
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;         //设置响应优先级级别值      
	NVIC_Init(&NVIC_InitStruct);
	
	//使能计数器
	TIM_Cmd(TIM9, ENABLE);
}




/********************************************************************
*函数名    ：TIM4_ch4_Led2_Pwm
*函数功能  ：定时器4通道4输出PWM
*函数参数  ：无
*函数返回值：无
*函数描述  ：APB1  84M   84分频       1次/us     周期1000个数（1ms ）
             PB9
*****************************************************************/
void TIM4_ch4_Led2_Pwm(void)
{
	/*IO控制器配置*/
	//时钟使能
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	//GPIO端口配置
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;          //通用输出
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;         //推挽
	GPIO_InitStruct.GPIO_Pin =  GPIO_Pin_9;             //  9
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;       //无上下拉
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;       //速度
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	//复用配置
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource9 ,GPIO_AF_TIM4);
	
	/*定时器控制器配置*/
	//时钟使能
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	
	//基本时基单元配置
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct = {0};
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;        //递增计数
	TIM_TimeBaseInitStruct.TIM_Period =1000 - 1;                    //重载值
	TIM_TimeBaseInitStruct.TIM_Prescaler = 84 - 1;                      //84分频
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStruct);
	
	//通道配置
	TIM_OCInitTypeDef TIM_OCInitStruct  = {0};
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;                //pwm1
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_Low;         //低电平有效
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;    //通道使能
	TIM_OCInitStruct.TIM_Pulse   = 500;
	TIM_OC4Init(TIM4, &TIM_OCInitStruct);
	
	//使能计数器
	TIM_Cmd(TIM4, ENABLE);
}













