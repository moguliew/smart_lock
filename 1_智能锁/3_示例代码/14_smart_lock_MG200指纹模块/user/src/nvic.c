#include "stm32f4xx.h"                  // Device header
#include "string.h" 
#include "door.h" 
#include "stdio.h" 
#include "w25q64.h"
#include "led.h"
#include "lcd.h"

/**
 * 函数名    ：USART1_IRQHandler
 * 函数功能  ：串口1的中断服务函数
 * 函数参数  ：无
 * 函数返回值：无
 * 函数描述  ：烧录地址是从0x020000
 */ 
uint8_t usart1_rec_buff[20] = {0};
uint8_t zk_flag = 0;


void USART1_IRQHandler(void)
{
	uint8_t zk_data;
	
	static uint32_t zk_addr = BASE_ADDR;
	static uint32_t zk_cont = ZK_CNT;
	
	//判断是接收中断
	if(USART_GetITStatus(USART1,USART_IT_RXNE))
	{
		//清中断标志位
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
		//紧急事件
		
		//烧录字库
		if(zk_flag)
		{	
			LED1_ON;
			zk_data = USART_ReceiveData(USART1);
			w25q64_page_write(zk_addr,1,&zk_data);
			zk_addr++;
			zk_cont--;
			if(zk_cont == 0)
			{
				//烧录完成
				printf("字库烧录完成!\r\n");
				LED1_OFF;
				zk_flag = 0;
			}	
		}
		

	}
	
	
	//判断是空闲中断
	if(USART_GetITStatus(USART1,USART_IT_IDLE))
	{
		//请中断标志位
		USART1->SR;
		USART1->DR;
		//紧急事件
			
	}
	
}



/**
 * 函数名    ：USART6_IRQHandler
 * 函数功能  ：串口6的中断服务函数
 * 函数参数  ：无
 * 函数返回值：无
 * 函数描述  ：
 */ 
uint8_t mg200_rec_buf[10];
uint8_t mg200_rec_flag = 0;
void USART6_IRQHandler(void)
{
	static uint8_t i = 0;
	if(USART_GetITStatus(USART6,USART_IT_RXNE))
	{
		USART_ClearITPendingBit(USART6,USART_IT_RXNE);
		
		mg200_rec_buf[i++] = USART_ReceiveData(USART6);
		
	}
	
	if(USART_GetITStatus(USART6,USART_IT_IDLE))
	{
		USART6->SR;
		USART6->DR;

		i = 0;       			
		mg200_rec_flag = 1;     //接收完成mg200_rec_flag标志位置1
		
	}
	
}








/**
 * 函数名    ：TIM1_BRK_TIM9_IRQHandler
 * 函数功能  ：TIM9的中断服务函数
 * 函数参数  ：无
 * 函数返回值：无
 * 函数描述  ：
 */ 
void TIM1_BRK_TIM9_IRQHandler(void)
{
	static uint16_t led_pwm = 0;
	static uint8_t led_pwm_flag = 1;
	
	//判断TIM9的更新中断
	if(TIM_GetITStatus(TIM9,TIM_IT_Update))
	{
		//清除中断标志位
		TIM_ClearITPendingBit(TIM9,TIM_IT_Update);
		//紧急事件
		if(led_pwm_flag == 1)
		{
			led_pwm++;
			if(led_pwm >= 1000)
			{
				led_pwm_flag = 0;
			}
		}
		
		else if(led_pwm_flag == 0)
		{
			led_pwm--;
			if(led_pwm <= 0)
			{
				led_pwm_flag = 1;
			}
		}
		TIM_SetCompare4(TIM4,led_pwm);
	}
}










