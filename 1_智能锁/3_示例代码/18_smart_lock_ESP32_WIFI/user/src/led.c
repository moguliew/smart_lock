#include "stm32f4xx.h"                  // Device header

/***********************************************
*������    ��led_init
*��������  ����LED�����ùܽų�ʼ������
*��������  ����
*��������ֵ����
*��������  ��PB8-----LED1
             PB9-----LED2
***********************************************/
void led_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	//�˿�ʱ��ʹ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;          //ͨ�����
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;         //����
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9; // 8 9
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;       //��������
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;       //�ٶ�
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	//������ݼĴ�������----����ʼ��
	GPIO_SetBits(GPIOB, GPIO_Pin_8);
	GPIO_SetBits(GPIOB, GPIO_Pin_9);
}























