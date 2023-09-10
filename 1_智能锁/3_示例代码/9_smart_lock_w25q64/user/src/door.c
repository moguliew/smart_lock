#include "door.h"                  // Device header

static void delay_ms(uint32_t ms)
{
	uint32_t i = 84 / 4 * 1000 * ms;                            
	while(i)                                                      
	{
		i--;
	}
}

/**
 * ������    ��door_init
 * ��������  ������������ùܽų�ʼ������
 * ��������  ����
 * ��������ֵ����
 * ��������  ��PB3��PB4
 */
void door_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct ={0};
	
	//�˿�ʱ��ʹ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;          //ͨ�����
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;         //����
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4; // 3 4
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;       //��������
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;       //�ٶ�
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	//������ݼĴ�������----����ʼ��
	GPIO_SetBits(GPIOB, GPIO_Pin_3);
	GPIO_SetBits(GPIOB, GPIO_Pin_4);
}




/**
 * ������    ��open_door
 * ��������  �����ź���
 * ��������  ����
 * ��������ֵ����
 * ��������  ��
 */
void open_door(void)
{
	GPIO_SetBits(GPIOB, GPIO_Pin_3);   //1
	GPIO_ResetBits(GPIOB, GPIO_Pin_4); //0
	delay_ms(300);
	GPIO_SetBits(GPIOB, GPIO_Pin_3);
	GPIO_SetBits(GPIOB, GPIO_Pin_4);
}



/**
 * ������    ��close_door
 * ��������  �����ź���
 * ��������  ����
 * ��������ֵ����
 * ��������  ��
 */
void close_door(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_3);   //0
	GPIO_SetBits(GPIOB, GPIO_Pin_4);     //1
	delay_ms(300);
	GPIO_SetBits(GPIOB, GPIO_Pin_3);
	GPIO_SetBits(GPIOB, GPIO_Pin_4);
}






