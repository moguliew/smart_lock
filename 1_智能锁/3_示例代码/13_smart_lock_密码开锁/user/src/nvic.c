#include "stm32f4xx.h"                  // Device header
#include "string.h" 
#include "door.h" 
#include "stdio.h" 
#include "w25q64.h"
#include "lcd.h"
#include "led.h"

/**
 * 函数名    ：USART1_IRQHandler
 * 函数功能  ：串口1的中断服务函数
 * 函数参数  ：无
 * 函数返回值：无
 * 函数描述  ：烧录地址是从0x000000
 */
uint8_t usart1_rec_buf[20] = {0};
uint8_t zk_flag = 0;


void USART1_IRQHandler(void)
{
	uint8_t zk_data;
	
	static uint32_t zk_addr = BASE_ADDR; 
	static uint32_t zk_cnt = ZK_CNT;
	
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
			zk_cnt--;
			if(zk_cnt == 0)
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
 * 函数名    ：TIM1_BRK_TIM9_IRQHandler
 * 函数功能  ：TIM9的中断服务函数
 * 函数参数  ：无
 * 函数返回值：无
 * 函数描述  ：
 */
void TIM1_BRK_TIM9_IRQHandler(void)
{
	static u16 led_pwm = 0;
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










