#include "stm32f4xx.h"                  // Device header
#include "string.h" 
#include "door.h" 
#include "stdio.h" 
#include "w25q64.h"
#include "led.h"
#include "nvic.h"
/***********************************************
*函数名    ：USART1_IRQHandler
*函数功能  ：串口1的中断服务函数
*函数参数  ：无
*函数返回值：无
*函数描述  ：烧录地址是从0x020000
***********************************************/
u8 usart1_rec_buff[20] = {0};
u8 zk_flag = 0;


void USART1_IRQHandler(void)
{
	u8 zk_data;
	
	static u32 zk_addr = 0x020000;
	static u32 zk_cont = 0x00141F58;
	
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



/***********************************************
*函数名    ：USART6_IRQHandler
*函数功能  ：串口6的中断服务函数
*函数参数  ：无
*函数返回值：无
*函数描述  ：
***********************************************/
u8 mg200_rec_buff[10];
u8 mg200_rec_flag = 0;
void USART6_IRQHandler(void)
{
	static u8 i=0;
	//判断是接收中断
	if(USART_GetITStatus(USART6,USART_IT_RXNE))
	{
		//请中断标志位
		USART_ClearITPendingBit(USART6,USART_IT_RXNE);
		//紧急事件
		mg200_rec_buff[i++] = USART_ReceiveData(USART6);
		
	}
	
	//判断是空闲中断
	if(USART_GetITStatus(USART6,USART_IT_IDLE))
	{
		//请中断标志位
		USART6->SR;
		USART6->DR;
		//紧急事件
		i = 0;       //下一次接收从0号开始放
		mg200_rec_flag = 1;
		
	}
	
}









USART2_RECSTR esp32rec = {0};
/*****************************************************
函数功能：ESP32接收数据
函数形参：void
函数返回值：void
函数说明：		
*****************************************************/
void USART2_IRQHandler(void)
{
	u8 temp;
	
	if(USART_GetITStatus(USART2,USART_IT_RXNE))
	{
		//清除接收中断标志
		temp = USART_ReceiveData(USART2);
	
		if(temp == 0)
		{
			esp32rec.buff[esp32rec.len++] = ' ';
		}
		else
		{
			esp32rec.buff[esp32rec.len++] = temp;
		}		
	}
	else if(USART_GetITStatus(USART2,USART_IT_IDLE))//空闲中断
	{
		//清除空闲中断标志
		USART2->SR;
		USART2->DR;
		
		
		esp32rec.buff[esp32rec.len] = '\0';
		esp32rec.len = 0;
		esp32rec.flag= 1;
		
		//printf("%s", esp32rec.buff);
	}
}























/***********************************************
*函数名    ：TIM1_BRK_TIM9_IRQHandler
*函数功能  ：TIM9的中断服务函数
*函数参数  ：无
*函数返回值：无
*函数描述  ：
***********************************************/
void TIM1_BRK_TIM9_IRQHandler(void)
{
	static u16 led_pwm = 0;
	static u8 led_pwm_flag = 1;
	
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










