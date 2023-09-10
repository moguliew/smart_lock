#include "stm32f4xx.h"                  // Device header
#include "string.h" 
#include "door.h" 
#include "stdio.h" 
#include "w25q64.h"
#include "lcd.h"
#include "led.h"

/**
 * ������    ��USART1_IRQHandler
 * ��������  ������1���жϷ�����
 * ��������  ����
 * ��������ֵ����
 * ��������  ����¼��ַ�Ǵ�0x000000
 */
uint8_t usart1_rec_buf[20] = {0};
uint8_t zk_flag = 0;


void USART1_IRQHandler(void)
{
	uint8_t zk_data;
	
	static uint32_t zk_addr = BASE_ADDR; 
	static uint32_t zk_cnt = ZK_CNT;
	
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
			zk_cnt--;
			if(zk_cnt == 0)
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

















/**
 * ������    ��TIM1_BRK_TIM9_IRQHandler
 * ��������  ��TIM9���жϷ�����
 * ��������  ����
 * ��������ֵ����
 * ��������  ��
 */
void TIM1_BRK_TIM9_IRQHandler(void)
{
	static u16 led_pwm = 0;
	static uint8_t led_pwm_flag = 1;
	
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










