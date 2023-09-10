#include "stm32f4xx.h"                  // Device header


/***********************************************
*������    ��Tim5_delay_ms
*��������  ����ʱ��5���뼶��ʱ����
*��������  ��u32 ms
*��������ֵ����
*��������  ��APB1  84M    84��Ƶ  1��/us
***********************************************/
void Tim5_delay_ms(u32 ms)
{
	//��ʱ��ʱ��ʹ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);
	
	
	//��ʱ������ʱ����Ԫ����
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct = {0};
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;        //��������
	TIM_TimeBaseInitStruct.TIM_Period = 1 * 1000 * ms - 1;              //����ֵ
	TIM_TimeBaseInitStruct.TIM_Prescaler = 84 - 1;                      //84��Ƶ
	
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseInitStruct);
	//����������/���±�־Wie
	TIM_ClearFlag(TIM5,TIM_FLAG_Update);
	//���õ��λ�������
	TIM_SelectOnePulseMode(TIM5,TIM_OPMode_Single);         //����
	
	//ʹ�ܼ�����
	TIM_Cmd(TIM5, ENABLE);
	//�ȴ��������
	while(!(TIM_GetFlagStatus(TIM5,TIM_FLAG_Update)));
	
}




/***********************************************
*������    ��Tim5_delay_us
*��������  ����ʱ��5΢�뼶��ʱ����
*��������  ��u32 us
*��������ֵ����
*��������  ��APB1  84M    84��Ƶ  1��/us
***********************************************/
void Tim5_delay_us(u32 us)
{
	//��ʱ��ʱ��ʹ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);
	//��ʱ������ʱ����Ԫ����
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct = {0};
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;        //��������
	TIM_TimeBaseInitStruct.TIM_Period = 1 * us - 1;                    //����ֵ
	TIM_TimeBaseInitStruct.TIM_Prescaler = 84 - 1;                      //84��Ƶ
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseInitStruct);
	//����������/���±�־Wie
	TIM_ClearFlag(TIM5,TIM_FLAG_Update);
	//ʹ�ܼ�����
	TIM_Cmd(TIM5, ENABLE);
	//�ȴ��������
	while(!(TIM_GetFlagStatus(TIM5,TIM_FLAG_Update)));
	
	//�رն�ʱ��
	TIM_Cmd(TIM5, DISABLE);
}




/***********************************************
*������    ��Tim9_IT_ms
*��������  ����ʱ��9���뼶�ж�
*��������  ��u32 ms
*��������ֵ����
*��������  ��APB2  84M    8400��Ƶ  10��/ms  
***********************************************/
void Tim9_IT_ms(u32 ms)
{
	/*��ʱ������*/
	//ʱ��ʹ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9,ENABLE);
	//����ʱ����Ԫ����
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct = {0};
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;        //��������
	TIM_TimeBaseInitStruct.TIM_Period = 10 * ms - 1;                    //����ֵ
	TIM_TimeBaseInitStruct.TIM_Prescaler = 8400 - 1;                      //84��Ƶ
	TIM_TimeBaseInit(TIM9, &TIM_TimeBaseInitStruct);
	//����������/���� �жϱ�־Wie
	TIM_ClearITPendingBit(TIM9,TIM_IT_Update);
	//�����ж�ʹ��
	TIM_ITConfig(TIM9,TIM_IT_Update,ENABLE);
	
	/*NVIC����*/
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = TIM1_BRK_TIM9_IRQn ;    //�ж�Դ����
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;      //��Ӧͨ��ʹ��
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;  //������ռ���ȼ�����ֵ
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;         //������Ӧ���ȼ�����ֵ      
	NVIC_Init(&NVIC_InitStruct);
	
	//ʹ�ܼ�����
	TIM_Cmd(TIM9, ENABLE);
}




/********************************************************************
*������    ��TIM4_ch4_Led2_Pwm
*��������  ����ʱ��4ͨ��4���PWM
*��������  ����
*��������ֵ����
*��������  ��APB1  84M   84��Ƶ       1��/us     ����1000������1ms ��
             PB9
*****************************************************************/
void TIM4_ch4_Led2_Pwm(void)
{
	/*IO����������*/
	//ʱ��ʹ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;          //ͨ�����
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;         //����
	GPIO_InitStruct.GPIO_Pin =  GPIO_Pin_9;             //  9
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;       //��������
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;       //�ٶ�
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	//��������
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource9 ,GPIO_AF_TIM4);
	
	/*��ʱ������������*/
	//ʱ��ʹ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	
	//����ʱ����Ԫ����
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct = {0};
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;        //��������
	TIM_TimeBaseInitStruct.TIM_Period =1000 - 1;                    //����ֵ
	TIM_TimeBaseInitStruct.TIM_Prescaler = 84 - 1;                      //84��Ƶ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStruct);
	
	//ͨ������
	TIM_OCInitTypeDef TIM_OCInitStruct  = {0};
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;                //pwm1
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_Low;         //�͵�ƽ��Ч
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;    //ͨ��ʹ��
	TIM_OCInitStruct.TIM_Pulse   = 500;
	TIM_OC4Init(TIM4, &TIM_OCInitStruct);
	
	//ʹ�ܼ�����
	TIM_Cmd(TIM4, ENABLE);
}













