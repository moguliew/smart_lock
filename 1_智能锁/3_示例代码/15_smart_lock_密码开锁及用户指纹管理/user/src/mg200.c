#include "stm32f4xx.h"                  // Device header
#include "mg200.h" 
#include "stdio.h" 
#include "timer.h" 
#include "voice.h" 
#include "door.h" 



u8 page_mode = 1;      //�� �ӽ����ʶ��־λ����





/***********************************************
*������    ��usart6_init
*��������  ��USART6��ʼ������
*��������  ��u32 bands
*��������ֵ����
*��������  ��
***********************************************/
void usart6_init(u32 bands)
{
	
	/*IO*/
	//1. �������ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	
	//2. ����PC6��PC7
	GPIO_InitTypeDef  GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;//����Ϊ����
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;//����
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;//6��7�Źܽ�
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;//��������
	GPIO_InitStruct.GPIO_Speed = GPIO_Low_Speed;//����
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	
	//3.��PC6��PC7���õ�USART6��
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_USART6);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_USART6);
	
	
	/*����USART6*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);
	
	USART_InitTypeDef  USART_InitStruct = {0};
	USART_InitStruct.USART_BaudRate = bands;//������
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ��������
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//ʹ�ܷ��ͽ���
	USART_InitStruct.USART_Parity = USART_Parity_No;//����żУ��
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b; 
	USART_Init(USART6,&USART_InitStruct);
	//ʹ���ж�
	USART_ITConfig(USART6,USART_IT_RXNE,ENABLE);//ʹ�ܽ����ж�
	USART_ITConfig(USART6,USART_IT_IDLE,ENABLE);//ʹ�ܿ����ж�
	
	
	/*NVIC����������*/
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = USART6_IRQn;    //�ж�Դ����
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;      //��Ӧͨ��ʹ��
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;  //������ռ���ȼ�����ֵ
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;         //������Ӧ���ȼ�����ֵ      
	NVIC_Init(&NVIC_InitStruct);
	
	
	//5.ʹ��USART6
	USART_Cmd(USART6,ENABLE);
}


/***********************************************
*������    ��usart6_send_byte
*��������  ������6����һ���ֽں���
*��������  ��u8 data
*��������ֵ����
*��������  ��
***********************************************/
void usart6_send_byte(u8 data)
{
	//�ȴ���һλ�������
	while(!(USART_GetFlagStatus(USART6,USART_FLAG_TC)));
	USART_SendData(USART6, data);
}

/***********************************************
*������    ��usart6_send_str
*��������  ������6���Ͷ��ֽں���
*��������  ��u8 *str
*��������ֵ����
*��������  ��
***********************************************/
void usart6_send_str(u8 *str)
{
	while(*str != '\0')
	{
		usart6_send_byte(*str);
		str++;
	}
}





/***********************************************
*������    ��mg200_init
*��������  ��ָ��ģ���ʼ������
*��������  ����
*��������ֵ����
*��������  ��PC0-----Detect
             PC1-----PWR
***********************************************/
void mg200_init(void)
{
	/*����6����*/
	usart6_init(115200);
	
	/*MG200_IO*/
	//1. �������ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	
	//2. ����PC0
	GPIO_InitTypeDef  GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;//����Ϊ����
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;//0�Źܽ�
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;//��������
	GPIO_InitStruct.GPIO_Speed = GPIO_Low_Speed;//����
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	//����PC1
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;//����Ϊ���
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;//1�Źܽ�
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	
	//����Դ
	MG200_PWR_OFF;

}



/***********************************************
*������    ��mg200_write_cmd
*��������  ��MG20����ָ���
*��������  ��u8 cmd,u8 parameter
*��������ֵ����
*��������  ��
***********************************************/
void mg200_write_cmd(u8 cmd,u8 parameter)
{
	u8 check_sum = 0;        //У��ͱ���
	usart6_send_byte(0x6c);
	usart6_send_byte(0x63);
	usart6_send_byte(0x62);
	usart6_send_byte(cmd);
	usart6_send_byte(parameter);
	usart6_send_byte(0);
	check_sum = (0x63 +0x62 + cmd +parameter) & 0xff;           
	usart6_send_byte(check_sum);
	//��ֹ�����ϵ�  �ᷢ�����ݸ�MCU  ���⵼�±�־λ��ǰ��1
	mg200_rec_flag = 0;
}


/***********************************************
*������    ��mg200_read_cmd
*��������  �������ж��н��յ������ݰ�
*��������  ��u8 cmd,u8 *parameter,u8 *result
*��������ֵ��u8 
*��������  ��cmd ֮ǰ���͵�ָ��  Ҫ���յ���Ӧ�����Ӧ
             *parameter    ���յ����������
             *result       ���յ�����ִ�е�Ч��
***********************************************/
u8 mg200_read_cmd(u8 cmd,u8 *parameter,u8 *result)
{
	
	/*�ȴ�ָ�����ݰ����ж��н������*/
	while(!mg200_rec_flag);
	mg200_rec_flag = 0;               //��־λ����
	/*��֤���յ������ݰ�*/
	if(mg200_rec_buff[0] != 0x6c)   return 1;
	if(mg200_rec_buff[1] != 0x62)   return 2;
	if(mg200_rec_buff[2] != 0x63)   return 3;
	if(mg200_rec_buff[3] != cmd)    return 4;
	
	*result = mg200_rec_buff[4];               //��ȡ��ִ��Ч������
	*parameter = mg200_rec_buff[5];            //��ȡ�����صĲ���
	//if(mg200_rec_buff[6] != 0x00)   return 5;//Ԥ��λ��ʱ����õ�����һ����0x00  ��ȡ�汾��
	
	u8 check_sum = (0x62+0x63+cmd+*result+*parameter + mg200_rec_buff[6]) & 0xff;
	if(mg200_rec_buff[7] != check_sum) return 6;
		
	return 0;

}



/***********************************************
*������    ��ReqCaptureAndExtract
*��������  ��MG200�ɼ�ָ������
*��������  ��u8 parameter
*��������ֵ��u8 
*��������  �������ǵڼ��βɼ�
            ����0 ��ʾ�ɹ�
***********************************************/
u8 ReqCaptureAndExtract(u8 parameter)
{
	u8 parame,result;
	u8 err_val;

	mg200_write_cmd(0X51,parameter);
	err_val = mg200_read_cmd(0x51,&parame,&result);
	if(err_val == 0)
	{
		switch(result)
		{
			case 0: 		printf("ץȡָ��ͼ����������ȡ�ɹ�\r\n");break;
			case 0xb1:  printf("ָ�ƹ�С(small finger)\r\n");break;
			case 0xb2:  printf("��ָ�� (no finger)\r\n");break;
			case 0xb3: 	printf("ָ��λ�ù��ڿ�����Ҫ����ָ���ұ������� left finger��\r\n");break;
			case 0xb4:  printf("ָ��λ�ù��ڿ��ң���Ҫ����ָ����������� right finger��\r\n");break;
			case 0xb5:  printf("ָ��λ�ù��ڿ��ϣ���Ҫ����ָ���±������� up finger��\r\n");break;
			case 0xb6: 	printf("ָ��λ�ù��ڿ��£���Ҫ����ָ���ϱ����� (down finger��\r\n");break;
			case 0xb7:  printf("ʪ��ָ ��ָ�Ʊ����������ˮ�չ��ࣩ (wet finger)\r\n");break;
			case 0xb8:  printf("������ָ(dry finger)\r\n");break;
			case 0xc0: 	printf("������ȡ����\r\n");break;
			case 0xc2: 	printf("������ȡ���� (ͼ��������)\r\n");break;
			default :   printf("ץȡָ��ͼ����������ȡ����\r\n");break;
		}

	}
	else
	{
		printf("ͨ��ʧ�ܣ�\r\n");
		
		return 0xff;
	}
		
	return result;
	
}





/***********************************************
*������    ��Enroll
*��������  ��ע�����û���ָ��
*��������  ��u8 ID
*��������ֵ��u8 
*��������  ��
            ����0 ��ʾע��ɹ�
***********************************************/
u8 Enroll(u8 ID)
{
	u8 ret;
	
	/*��ʾ��ʼע�ᣬ��Ӧ�ϵ�*/
	printf("�뽫��ָ�ŵ�ָ��ģ�鿪ʼע��ָ��\r\n");
	    
	/*���βɼ�*/
	//��һ��
	printf("��һ��ָ�Ʋɼ�\r\n");
	while(!MG200_DETECT); 
	MG200_PWR_ON;                  //����Դ
	Tim5_delay_ms(30);
	do
	{
		ret = ReqCaptureAndExtract(0x00);
	}while(ret);
	printf("��̧����ָ���ٴβɼ���һ��ָ��\r\n");
	while(MG200_DETECT);
	
	//�ڶ���
	printf("�ڶ���ָ�Ʋɼ�\r\n");
	
	while(!MG200_DETECT);
	do
	{
		ret = ReqCaptureAndExtract(0x01);
	}while(ret);
	printf("��̧����ָ���ٴβɼ���һ��ָ��\r\n");
	while(MG200_DETECT);
	
	//������
	printf("������ָ�Ʋɼ�\r\n");
	while(!MG200_DETECT);
	do
	{
		ret = ReqCaptureAndExtract(0x02);
	}while(ret);
	
	
	
	/*ע��*/
	mg200_write_cmd(0x7f,ID);          //ע��IDΪ�Զ�����
	u8 para,resu;
	
	if(mg200_read_cmd(0x7f,&para,&resu) != 0)
	{
		printf("ͨ��ʧ�ܵ���ע��ʧ�ܣ���������ע�ᣬ���ٴν���ע�Ṧ�ܣ�\r\n");
		MG200_PWR_OFF;
		return 0xff;
	}
	else
	{
		switch(resu)
		{
			case 0: printf("ע��ɹ�\r\n");break;
			case 0x83: printf("ID ����(ID < 0 ���� ID > ����û���)����ͨ�Ŵ���\r\n");break;
			case 0x91: printf("ע��ʧ��(�û���������)\r\n");break;
			case 0x93: printf("�Ѿ�ע��� ID\r\n");break;
			case 0x94: printf("ָ����ȡ���� < 3\r\n");break;
		}
	}
	//�رյ�Դ
	MG200_PWR_OFF;
	return 0;
}




/***********************************************
*������    ��Match_l_n
*��������  ���ȶ�ָ��
*��������  ����
*��������ֵ��u8    ID��
*��������  ���Աȳɹ������հ��Ĳ���   ��ID��   ʧ��0x00
						 �Աȳɹ������հ���result ��0x00   ʧ��0x92
***********************************************/
u8 Match_l_n(void)
{
	u8 ret;
	u8 rec_parameter = 0xff;
	static u8 show_flag = 1;
	
	if(show_flag)
	{
		printf("�������ָ����ָ�ƶԱȿ���\r\n");
		show_flag = 0;
	}
	
	
	//����ָ��,��Ӧ�ϵ�
	if(MG200_DETECT)
	{
		MG200_PWR_ON;
		Tim5_delay_ms(30);
		
		/*�ɼ�ָ��*/
		do
		{
			ret = ReqCaptureAndExtract(0x00);
		}while(ret);
		
		
		/*�Ա�ָ��*/
		mg200_write_cmd(0x71,0);//���ͱȶԵ���Ϣ
		if(mg200_read_cmd(0x71,&rec_parameter,&ret) != 0)
		{
			printf("ͨ��ʧ��,�������ݰ�����\r\n");
			MG200_PWR_OFF;
			voice(DOOROPEN_FAIL); 
			return 0xff;
		}
		
		/*���ݶԱȽ��ִ�п���*/
		if(ret == 0x00)
		{
			//�����ӽ����־
			page_mode = 2;
			
			printf("�ȶԳɹ�\r\n");
			voice(DOOROPEN_SUCCESS);
			open_door();
			while(MG200_DETECT);                 //̧��ָ�ƣ�����
			close_door();
		}
		else   //0x92
		{
			printf("�ȶ�ʧ��\r\n");
			voice(DOOROPEN_FAIL);             //����
		}
		
		printf("ID��Ϊ��%d\r\n",rec_parameter);

		//�ϵ�
		MG200_PWR_OFF;	
	}
	
	
	
	return rec_parameter;	

}






/****************************************************
*������    ��erase_one
*��������  ��ɾ��ָ��IDָ��
*��������  ��u8 ID��
*��������ֵ��u8 
*��������  ��
*****************************************************/
u8 erase_one(u8 id)
{
	u8 rec_parameter,ret;
	
	MG200_PWR_ON;
	Tim5_delay_ms(30);
	mg200_write_cmd(0x73,id);
	if(mg200_read_cmd(0x73,&rec_parameter,&ret))
	{
		printf("ͨ��ʧ��,�������ݰ�����\r\n");
		MG200_PWR_OFF;
		return 0xff;
	}
	
	else
	{
		switch(ret)
		{
			case 0:printf("ɾ���ɹ�\r\n");break;
			case 0x83:printf("��������\r\n");break;
			case 0x90:printf("δע����û�\r\n");break;
			case 0xff:printf("д��ROM����\r\n");break;
		}
	}
	
	
	MG200_PWR_OFF;
	
	return 0;
}



/****************************************************
*������    ��erase_all
*��������  ��ɾ������ָ��ID
*��������  ����
*��������ֵ��u8 
*��������  ��
*****************************************************/
u8 erase_all(void)
{
	u8 rec_parameter,ret;
	
	MG200_PWR_ON;
	Tim5_delay_ms(30);
	mg200_write_cmd(0x54,0);
	if(mg200_read_cmd(0x54,&rec_parameter,&ret))
	{
		printf("ͨ��ʧ��,�������ݰ�����\r\n");
		MG200_PWR_OFF;
		return 0xff;
	}
	
	else
	{
		switch(ret)
		{
			case 0:printf("ɾ���ɹ�\r\n");break;
			case 0x90:printf("δע����û�\r\n");break;
			default:printf("ɾ��ʧ��\r\n");break;
		}
		
	}
	
	
	MG200_PWR_OFF;
	
	
	return 0;
}












