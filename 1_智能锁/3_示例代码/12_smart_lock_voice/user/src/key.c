#include "key.h"

static void delay_ms(uint32_t ms)
{
	uint32_t i = 84 / 4 * 1000 * ms;
	while(i--);
}

/**
 * @brief:������ʼ��
 * @note:KEY---PC13  �͵�ƽ��ʾ��������
 */
void key_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	//��GPIOBʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	
	//����ģʽ����	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	
}

/**
 * @brief������ɨ�躯��
 * @return�����µİ���
 * @note:�а������·��ذ��µİ�����û�з���0       
 *       ����,��ֹ����һ�μ�⵽�ܶ�Σ��ӱ�־λ��     
 */
uint8_t key_scan(void)
{
	static uint8_t key_flag = 0;
	if((!KEY) || key_flag == 0)
	{
		delay_ms(10);
		if(!KEY)
		{
			key_flag = 1;
			return 1;
		}
	}
	else
	{
		key_flag = 0;
	}
	return 0;
}

