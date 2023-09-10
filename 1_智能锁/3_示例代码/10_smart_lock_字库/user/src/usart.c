#include "stm32f4xx.h"                  // Device header
#include "stdio.h" 
/**
 * ������    ��usart1_init
 * ��������  ����USART1��ʼ������
 * ��������  ��u32 baud
 * ��������ֵ����
 * ��������  ��PA9-----Tx
 *            PA10----Rx
 */
void usart1_init(u32 baud)
{

	/*IO����������*/
	//�˿�ʱ��ʹ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;          	//����ģʽ
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;         //����
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; // 9 10
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;       //��������
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;       //�ٶ�
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	//���ù�������
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9 ,GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10 ,GPIO_AF_USART1);
	
	/*���ڿ���������*/
	//����ʱ��ʹ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	USART_InitTypeDef USART_InitStruct;
	USART_InitStruct.USART_BaudRate =  baud;            //������
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   //��Ӳ������
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                   //ȫ˫��
	USART_InitStruct.USART_Parity = USART_Parity_No;                               //����żУ��
	USART_InitStruct.USART_StopBits = USART_StopBits_1;                            //1��ֹͣλ
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;                       //8���ֳ�
	
	USART_Init(USART1,&USART_InitStruct);
	
	//�ж�ʹ��
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);                                //�����ж�ʹ��
	USART_ITConfig(USART1,USART_IT_IDLE,ENABLE);                                //�����ж�ʹ��
	
	/*NVIC����������*/
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_2);  //���ȼ�����
	
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;    //�ж�Դ����
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;      //��Ӧͨ��ʹ��
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;  //������ռ���ȼ�����ֵ
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;         //������Ӧ���ȼ�����ֵ      
	NVIC_Init(&NVIC_InitStruct);
	
	//����ʹ��
	USART_Cmd(USART1,ENABLE);
}




/**
 * ������    ��usart1_send_byte
 * ��������  ������1����һ���ֽں���
 * ��������  ��uint8_t data
 * ��������ֵ����
 * ��������  ��
 */
void usart1_send_byte(uint8_t data)
{
	//�ȴ���һλ�������
	while(!(USART_GetFlagStatus(USART1,USART_FLAG_TC)));
	USART_SendData(USART1, data);
}



/**
 * ������    ��usart1_send_str
 * ��������  ������1���Ͷ��ֽں���
 * ��������  ��uint8_t *str
 * ��������ֵ����
 * ��������  ��
 */
void usart1_send_str(uint8_t *str)
{
	while(*str != '\0')
	{
		usart1_send_byte(*str);
		str++;
	}
}






///////////////////////////////////////////////////////////
#if 1
#include <stdio.h>

/* ��֪����������C������ʹ�ð������ĺ��� */
#pragma import(__use_no_semihosting)

/* ���� _sys_exit() �Ա���ʹ�ð�����ģʽ */
void _sys_exit(int x)
{
    x = x;
}

/* ��׼����Ҫ��֧������ */
struct __FILE
{
    int handle;
};

FILE __stdout;

/* ��дfputc */
int fputc(int ch, FILE *stream)
{
    /* �����жϴ����Ƿ������ */
    while((USART1->SR & 0X40) == 0);

    /* ���ڷ�����ɣ������ַ����� */
    USART1->DR = (uint8_t) ch;

    return ch;
}

#endif
//////////////////////////////////////////////////////////






