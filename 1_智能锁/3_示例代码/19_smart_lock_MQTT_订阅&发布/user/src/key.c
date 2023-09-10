#include "stm32f4xx.h"                  // Device header
#include "key.h"
static void delay_ms(u32 ms)
{
	u32 i= 84 / 4 * 1000 * ms;
	while(i)
	{
		i--;
	}
}


/***********************************************
*������    ��key_init
*��������  ���԰������ùܽų�ʼ������
*��������  ����
*��������ֵ����
*��������  ��PC13------KEY
***********************************************/
void key_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	//�˿�ʱ��ʹ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;          //ͨ������
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;              //13
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;       //��������
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	
}



/***********************************************
*������    ��key_scan
*��������  ������ɨ�躯��
*��������  ����
*��������ֵ��u8
*��������  ������1��ʾ�������£�����0xffû�а���
***********************************************/
u8 key_scan(void)
{
	static u8 flag = 1;
	u8 key_val = 0xff;
	
	if(!KEY  && flag)
	{
		delay_ms(10);
		if(!KEY)
		{
			flag = 0;
			key_val = 1;
		}
	}
	
	
	if(KEY)
	{
		flag = 1;
	}
	
	return key_val;
}









