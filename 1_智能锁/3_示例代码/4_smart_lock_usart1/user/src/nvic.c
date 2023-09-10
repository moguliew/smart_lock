#include "string.h" 
#include "door.h" 
#include "stdio.h" 


/**
 * 函数名    ：USART1_IRQHandler
 * 函数功能  ：串口1的中断服务函数
 * 函数参数  ：无
 * 函数返回值：无
 * 函数描述  ：
 */
uint8_t usart1_rec_buf[20] = {0};
void USART1_IRQHandler(void)
{
	static uint8_t i = 0;
	//判断是接收中断
	if(USART_GetITStatus(USART1,USART_IT_RXNE))
	{
		//清中断标志位
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
		//紧急事件
		usart1_rec_buf[i] = USART_ReceiveData(USART1);
		i++;
	}
	
	
	//判断是空闲中断
	if(USART_GetITStatus(USART1,USART_IT_IDLE))
	{
		//清中断标志位
		USART1->SR;
		USART1->DR;
		//紧急事件
		usart1_rec_buf[i] = '\0';
		i = 0;
		printf("接收到的字符串是：%s\r\n",usart1_rec_buf);
		if(strcmp((char *)usart1_rec_buf,"open") == 0)
		{
			open_door();
		}
		else if(strcmp((char *)usart1_rec_buf,"close") == 0)
		{
			close_door();
		}
	}
	
}














