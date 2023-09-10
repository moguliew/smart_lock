#include "string.h" 
#include "door.h" 
#include "stdio.h" 


/**
 * ������    ��USART1_IRQHandler
 * ��������  ������1���жϷ�����
 * ��������  ����
 * ��������ֵ����
 * ��������  ��
 */
uint8_t usart1_rec_buf[20] = {0};
void USART1_IRQHandler(void)
{
	static uint8_t i = 0;
	//�ж��ǽ����ж�
	if(USART_GetITStatus(USART1,USART_IT_RXNE))
	{
		//���жϱ�־λ
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
		//�����¼�
		usart1_rec_buf[i] = USART_ReceiveData(USART1);
		i++;
	}
	
	
	//�ж��ǿ����ж�
	if(USART_GetITStatus(USART1,USART_IT_IDLE))
	{
		//���жϱ�־λ
		USART1->SR;
		USART1->DR;
		//�����¼�
		usart1_rec_buf[i] = '\0';
		i = 0;
		printf("���յ����ַ����ǣ�%s\r\n",usart1_rec_buf);
		if(strcmp((char *)usart1_rec_buf,"open") == 0)
		{
			open_door();
		}
		else if(strcmp((char *)usart1_rec_buf,"close") == 0)
		{
			close_door();
		}
	}
	
}














