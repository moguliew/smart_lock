#include "led.h"

/** 
 * @brief:LED��ʼ��
 * @note:
 * 		 LED1---PB8---�͵�ƽ��
 *		 LED2---PB9---�͵�ƽ��
 */
void led_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	//��GPIOBʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	//����ģʽ����	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP; 
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Fast_Speed;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	//��ʼ������ģʽ
	LED1_OFF;
	LED2_OFF;
	
}






