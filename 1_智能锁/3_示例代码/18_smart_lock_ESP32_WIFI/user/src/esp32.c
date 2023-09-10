#include "stm32f4xx.h"                  // Device header
#include "nvic.h"
#include "string.h"
#include "timer.h"
#include "stdio.h"
/*****************************************************
�������ܣ�usart2��ʼ��
�����βΣ�void
��������ֵ��void
����˵����
		PA2
		PA3
*****************************************************/
void usart2_esp32_Init(void)
{
	//1. �������ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	
	//2. ����PB8��PB9
	GPIO_InitTypeDef  GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;//����Ϊ����
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;//����
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;//6��7�Źܽ�
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;//��������
	GPIO_InitStruct.GPIO_Speed = GPIO_Low_Speed;//����
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	//3.��PC6��PC7���õ�USART6��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2);
	
	//4. ����USART1
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
	USART_InitTypeDef  USART_InitStruct = {0};
	USART_InitStruct.USART_BaudRate = 115200;//������
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ��������
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//ʹ�ܷ��ͽ���
	USART_InitStruct.USART_Parity = USART_Parity_No;//����żУ��
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b; 
	USART_Init(USART2,&USART_InitStruct);
	
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);//ʹ�ܽ����ж�
	USART_ITConfig(USART2,USART_IT_IDLE,ENABLE);//ʹ�ܿ����ж�
	
	NVIC_SetPriority(USART2_IRQn,NVIC_EncodePriority(7-2,1,1));
	NVIC_EnableIRQ(USART2_IRQn);
	
	//5.ʹ��USART1
	USART_Cmd(USART2,ENABLE);
	
}


/*****************************************************
������    : usart2_esp32_send_str
��������  ������2�����ַ�����ESP32
�����β�  ��u8 *data
��������ֵ��void
����˵��  ��		
*****************************************************/
void usart2_esp32_send_str(u8 *data)
{
	while(*data != '\0')
	{
		while((USART2->SR & (0X1 << 6)) == 0);
		USART2->DR = *data++;
	}
}






/*****************************************************
��������  : Esp32_SendandReceive
��������  ��ESP32�ж��Ƿ���յ���Ҫ�Ļ�Ӧ�ź�
�����β�  ��
			      u8 *cmd  -- ���͵�ָ��
			      u8 *respond -- ��Ӧ����Ϣ  "OK"
            u32 wait_ms  -- �趨��ʱʱ��
��������ֵ��u8
����˵��  �������ͷ���0�����򷵻�1		
*****************************************************/
u8 Esp32_SendandReceive(u8 *cmd,u8 *respond,u32 wait_ms)  
{
	
	u32 esp32_cont = 0; 
	
	//����ָ��
	usart2_esp32_send_str(cmd);
	//��ʱ100ms  ָ����ʱ��
	Tim5_delay_ms(100);
	
	//�ж�ָ��ִ���Ƿ�ɹ�,���ɹ���ʱ�˳�
	while(1)
	{
		//�ȴ��ж�����Ľ�����ɱ�־λ��1
		while(!esp32rec.flag)      
		{
			//��ʱ���
			esp32_cont++;
			Tim5_delay_ms(1);
			if(esp32_cont == wait_ms)
			{
				//��ʱ,����ʧ��
				return 1;     
			}
		}
		esp32rec.flag = 0;     //��һ�ν��ջ�������1
		//�ж�ͨ���жϷ����������ݰ�,��ȷ��ָ���Ƿ�ִ�гɹ�
		if(strstr((const char *)esp32rec.buff, (const char *)respond) != NULL)
		{
			//�ҵ���   ��Ӧ�ɹ�
			return 0;
		} 	
	}
}








/***********************************************
*������    ��Exit_Cipmode
*��������  ���˳�͸��ģʽ,��������ģʽ
*��������  ����
*��������ֵ����
*��������  ���ӷ�������ģʽ���뵽����WIFI��ʱ��
             ��Ҫ�ô˺���
***********************************************/
void Exit_Cipmode(void)
{
	//�����˳�͸������
	usart2_esp32_send_str("+++");
	Tim5_delay_ms(2000); 
	printf("͸��ģʽ���˳�,��������ģʽ\r\n");
}




/***********************************************
*������    ��ESP32_Restore
*��������  ���ָ���������
*��������  ����
*��������ֵ��u8   ����0:�ɹ�      ����1:ʧ��
*��������  ��
***********************************************/
u8 ESP32_Restore(void)
{
	u8 sta;
	
	sta = Esp32_SendandReceive("AT+RESTORE\r\n","ready",3000);
	if(sta==0)
	{
		printf("�ָ��������óɹ�\r\n");
	}
	else
	{
		printf("�ָ���������ʧ��\r\n");
	}
	
	return sta;
}




/***********************************************
*������    ��ESP32_init
*��������  ��ESP32��ʼ�����ú���
*��������  ����
*��������ֵ��
*��������  ��
***********************************************/
void Esp32_Init(void)
{
	/*����2��ʼ��*/
	usart2_esp32_Init();
	
	/*WIFI��س�ʼ����*/
	//��ʼ����,��Ч
	Esp32_SendandReceive((u8 *)"AT\r\n",(u8 *)"OK",10); 
	
	if(Esp32_SendandReceive((u8 *)"AT\r\n",(u8 *)"OK",10) != 0 )
	{
		//�˳�͸��
		Exit_Cipmode();  //+++
	}
	
	//�ָ���������ģ��
	ESP32_Restore();
	//����λ�ͻ���ģʽ
	if(Esp32_SendandReceive((u8 *)"AT+CWMODE=1\r\n",(u8 *)"OK",10) == 0)
	{
		printf("�ͻ���ģʽָ��ִ�гɹ�\r\n");
	}
	else  printf("�ͻ���ģָ��ִ��ʧ��\r\n");
	
	/*TCP��س�ʼ����*/
	//͸��ģʽ�趨
	if(Esp32_SendandReceive((u8 *)"AT+CIPMODE=1\r\n",(u8 *)"OK",10) == 0)
	{
		printf("͸��ָ��ִ�гɹ�\r\n");
		
	}
	else  printf("͸��ָ��ִ��ʧ��\r\n");	
}



/***********************************************
*������    ��Esp32_Wificonnect
*��������  ��WIFI���Ӻ���
*��������  ��u8 *user,u8*password
*��������ֵ��u8
*��������  ������0���ӳɹ�,����1����ʧ��
***********************************************/
u8 Esp32_Wificonnect(u8 *user,u8*password)
{											
	u8 wifi_buff[50] = "AT+CWJAP=\"";
	/*ƴ���û���������,��Ϊ�涨�ĸ�ʽ*/
	strcat((char*)wifi_buff,(char*)user);
	// AT+CWJAP=\"LL
	
	strcat((char*)wifi_buff,"\",\"");		
	//AT+CWJAP=\"LL\",\"
	
	strcat((char*)wifi_buff,(char *)password);
	//AT+CWJAP=\"LL\",\"12345678
	
	strcat((char*)wifi_buff,"\"\r\n");
	//AT+CWJAP=\"LL\",\"12345678\"\r\n
	 
	/*����WIFI*/
	if(Esp32_SendandReceive(wifi_buff ,(u8 *)"OK",5000) == 0)
	{
		printf("WIFI����ָ��ִ�гɹ�\r\n");
		return 0;
	}
	else  printf("WIFI����ָ��ִ��ʧ��\r\n");
	return 1;
}



/***********************************************
*������    ��Esp32_Wificlose
*��������  ��WIFI�رպ���
*��������  ��void
*��������ֵ��u8
*��������  ������0���ӳɹ�,����1����ʧ��
***********************************************/
u8 Esp32_Wificlose(void)
{
	
	//���WIFI�Ƿ��ڴ���ģʽ
	if(Esp32_SendandReceive((u8 *)"AT\r\n",(u8 *)"OK",10) != 0 )
	{
		//�˳�͸��
		Exit_Cipmode();  //+++
	}
	
	
	//�ر�WIFI
	if(Esp32_SendandReceive((u8 *)"AT+CWQAP\r\n",(u8 *)"OK",10) == 0)
	{
		printf("�Ͽ�WIFIָ��ִ�гɹ�\r\n");
		return 0;
	}
	else  printf("�Ͽ�WIFIָ��ִ��ʧ��\r\n");
	return 1;	
	
}






/***********************************************
*������    ��Esp32_TCPconnect
*��������  ��TCP���Ӻ���
*��������  ��u8 *remote_ip,u32 com
*��������ֵ��u8
*��������  ������0���ӳɹ�,����1����ʧ��
***********************************************/
u8 Esp32_TCPconnect(u8 *remote_ip,u32 com)
{
	
	u8 tcp_buff[80] = {0};
	
	
	//��IP�Ͷ˿ں�ƴ�ӳ�һ���ַ���
	sprintf((char *)tcp_buff,"AT+CIPSTART=\"TCP\",\"%s\",%d\r\n",remote_ip,com);
	
	
	
	
	//����TCP
	if(Esp32_SendandReceive((u8 *)tcp_buff,(u8 *)"OK",5000) == 0)
	{
		printf("TCP����ָ��ִ�гɹ�\r\n");
		if(Esp32_SendandReceive((u8 *)"AT+CIPSEND\r\n",(u8 *)"OK",10) == 0)
		{
			printf("��������ָ��ִ�гɹ�\r\n");
			printf("���������ӳɹ�,���봫������ģʽ\r\n");
			return 0;
		}
		else
		{
			printf("��������ָ��ִ��ʧ��\r\n");
			return 2;
		}
	}
	
	
	
	else  
	{
		printf("TCP����ָ��ִ��ʧ��\r\n");
		return 1;
		
	}
	
	 
	
	
	
}













