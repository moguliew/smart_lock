#include "stm32f4xx.h"                  // Device header
#include "string.h" 
#include "door.h" 
#include "stdio.h" 
#include "w25q64.h"
#include "led.h"
#include "nvic.h"
/***********************************************
*������    ��USART1_IRQHandler
*��������  ������1���жϷ�����
*��������  ����
*��������ֵ����
*��������  ����¼��ַ�Ǵ�0x020000
***********************************************/
u8 usart1_rec_buff[20] = {0};
u8 zk_flag = 0;


void USART1_IRQHandler(void)
{
	u8 zk_data;
	
	static u32 zk_addr = 0x020000;
	static u32 zk_cont = 0x00141F58;
	
	//�ж��ǽ����ж�
	if(USART_GetITStatus(USART1,USART_IT_RXNE))
	{
		//���жϱ�־λ
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
		//�����¼�
		
		//��¼�ֿ�
		if(zk_flag)
		{	
			LED1_ON;
			zk_data = USART_ReceiveData(USART1);
			w25q64_page_write(zk_addr,1,&zk_data);
			zk_addr++;
			zk_cont--;
			if(zk_cont == 0)
			{
				//��¼���
				printf("�ֿ���¼���!\r\n");
				LED1_OFF;
				zk_flag = 0;
			}	
		}
		

	}
	
	
	//�ж��ǿ����ж�
	if(USART_GetITStatus(USART1,USART_IT_IDLE))
	{
		//���жϱ�־λ
		USART1->SR;
		USART1->DR;
		//�����¼�
		
		
	}
	
}



/***********************************************
*������    ��USART6_IRQHandler
*��������  ������6���жϷ�����
*��������  ����
*��������ֵ����
*��������  ��
***********************************************/
u8 mg200_rec_buff[10];
u8 mg200_rec_flag = 0;
void USART6_IRQHandler(void)
{
	static u8 i=0;
	//�ж��ǽ����ж�
	if(USART_GetITStatus(USART6,USART_IT_RXNE))
	{
		//���жϱ�־λ
		USART_ClearITPendingBit(USART6,USART_IT_RXNE);
		//�����¼�
		mg200_rec_buff[i++] = USART_ReceiveData(USART6);
		
	}
	
	//�ж��ǿ����ж�
	if(USART_GetITStatus(USART6,USART_IT_IDLE))
	{
		//���жϱ�־λ
		USART6->SR;
		USART6->DR;
		//�����¼�
		i = 0;       //��һ�ν��մ�0�ſ�ʼ��
		mg200_rec_flag = 1;
		
	}
	
}









USART2_RECSTR esp32rec = {0};
/*****************************************************
�������ܣ�ESP32��������
�����βΣ�void
��������ֵ��void
����˵����		
*****************************************************/
void USART2_IRQHandler(void)
{
	u8 temp;
	
	if(USART_GetITStatus(USART2,USART_IT_RXNE))
	{
		//��������жϱ�־
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
	else if(USART_GetITStatus(USART2,USART_IT_IDLE))//�����ж�
	{
		//��������жϱ�־
		USART2->SR;
		USART2->DR;
		
		
		esp32rec.buff[esp32rec.len] = '\0';
		esp32rec.len = 0;
		esp32rec.flag= 1;
		
		//printf("%s", esp32rec.buff);
	}
}























/***********************************************
*������    ��TIM1_BRK_TIM9_IRQHandler
*��������  ��TIM9���жϷ�����
*��������  ����
*��������ֵ����
*��������  ��
***********************************************/
void TIM1_BRK_TIM9_IRQHandler(void)
{
	static u16 led_pwm = 0;
	static u8 led_pwm_flag = 1;
	
	//�ж�TIM9�ĸ����ж�
	if(TIM_GetITStatus(TIM9,TIM_IT_Update))
	{
		//����жϱ�־λ
		TIM_ClearITPendingBit(TIM9,TIM_IT_Update);
		//�����¼�
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










