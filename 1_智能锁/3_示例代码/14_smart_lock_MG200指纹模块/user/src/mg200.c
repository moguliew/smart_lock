#include "stm32f4xx.h"                  // Device header
#include "mg200.h" 
#include "stdio.h" 
#include "tim.h" 
#include "voice.h" 
#include "door.h" 
/**
 * ������    ��usart6_init
 * ��������  ��USART6��ʼ������
 * ��������  ��uint32_t bands
 * ��������ֵ����
 * ��������  ��
 */
void usart6_init(uint32_t bands)
{
	/* IO */
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


/**
 * ������    ��usart6_send_byte
 * ��������  ������6����һ���ֽں���
 * ��������  ��uint8_t data
 * ��������ֵ����
 * ��������  ��
 */
void usart6_send_byte(uint8_t data)
{
	//�ȴ���һλ�������
	while(!(USART_GetFlagStatus(USART6,USART_FLAG_TC)));
	USART_SendData(USART6, data);
}

/**
 * ������    ��usart6_send_str
 * ��������  ������6���Ͷ��ֽں���
 * ��������  ��uint8_t *str
 * ��������ֵ����
 * ��������  ��
 */
void usart6_send_str(uint8_t *str)
{
	while(*str != '\0')
	{
		usart6_send_byte(*str);
		str++;
	}
}


/**
 * ������    ��mg200_init
 * ��������  ��ָ��ģ���ʼ������
 * ��������  ����
 * ��������ֵ����
 * ��������  ��PC0-----Detect
 *             PC1-----PWR
 */
void mg200_init(void)
{
	/* ����6���� */
	usart6_init(115200);
	
	/* MG200_IO */
	//1. �������ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	
	//2. ����PC0��PC1
	GPIO_InitTypeDef  GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;//����Ϊ����
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;//0�Źܽ�
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;//��������
	GPIO_InitStruct.GPIO_Speed = GPIO_Low_Speed;//����
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;//����Ϊ���
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;//1�Źܽ�
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	
	//����Դ
	MG200_PWR_OFF;

}



/**
 * ������    ��mg200_write_cmd
 * ��������  ��MG20����ָ���
 * ��������  ��uint8_t cmd,uint8_t parameter
 * ��������ֵ����
 * ��������  ��
 */
void mg200_write_cmd(uint8_t cmd,uint8_t parameter)
{
	uint8_t check_sum = 0;        //У��ͱ���
	
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


/**
 * ������    ��mg200_read_cmd
 * ��������  �������ж��н��յ������ݰ�
 * ��������  ��cmd--֮ǰ���͵�ָ�Ҫ���յ���Ӧ�����Ӧ
 * 			   *parameter--���յ����������
 *             *result--���յ�����ִ�е�Ч��
 * ��������ֵ���ɹ�����0��ʧ�ܷ��ش����� 
 * ��������  ��        
 */
uint8_t mg200_read_cmd(uint8_t cmd,uint8_t *parameter,uint8_t *result)
{
	
	/* �ȴ�ָ�����ݰ����ж��н������ */
	while(!mg200_rec_flag);
	mg200_rec_flag = 0;               //��־λ����
	
	/* ��֤���յ������ݰ� */
	if(mg200_rec_buf[0] != 0x6c)   return 1;
	if(mg200_rec_buf[1] != 0x62)   return 2;
	if(mg200_rec_buf[2] != 0x63)   return 3;
	if(mg200_rec_buf[3] != cmd)    return 4;
	
	*result = mg200_rec_buf[4];               //��ȡ��ִ��Ч������
	*parameter = mg200_rec_buf[5];            //��ȡ�����صĲ���
	//if(mg200_rec_buf[6] != 0x00)   return 5;//Ԥ��λ��ʱ����õ�����һ����0x00  ��ȡ�汾��
	
	uint8_t check_sum = (0x62+0x63+cmd+*result+*parameter + mg200_rec_buf[6]) & 0xff;
	if(mg200_rec_buf[7] != check_sum) 
		return 6;
		
	return 0;
}



/**
 * ������    ��ReqCaptureAndExtract
 * ��������  ��MG200�ɼ�ָ������
 * ��������  ��ָ��Ĳ���
 * ��������ֵ��uint8_t 
 * ��������  ������0 ��ʾ�ɹ�
 */
uint8_t ReqCaptureAndExtract(uint8_t parameter)
{
	uint8_t param,result;
	uint8_t err_val;

	mg200_write_cmd(0X51,parameter);
	err_val = mg200_read_cmd(0x51,&param,&result);
	if(err_val == 0)
	{
		switch(result)
		{
			case 0: 	printf("ץȡָ��ͼ����������ȡ�ɹ�\r\n");break;
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





/**
 * ������    ��Enroll
 * ��������  ��ע�����û���ָ��
 * ��������  ����
 * ��������ֵ��uint8_t 
 * ��������  ������0 ��ʾע��ɹ�
 */
uint8_t Enroll(void)
{
	uint8_t ret;
	
	/* ��ʾ��ʼע�ᣬ��Ӧ�ϵ� */
	printf("�뽫��ָ�ŵ�ָ��ģ�鿪ʼע��ָ��\r\n");
	    
	/* ���βɼ� */
	//��һ��
	printf("��һ��ָ�Ʋɼ�\r\n");
	while(!MG200_DETECT); 
	MG200_PWR_ON;                  //����Դ
	tim5_delay_ms(30);
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
	
	
	
	/* ע�� */
	mg200_write_cmd(0x7f,0);          //ע��IDΪ�Զ�����
	uint8_t para,resu;
	
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




/**
 * ������    ��Match_l_n
 * ��������  ���ȶ�ָ��
 * ��������  ����
 * ��������ֵ��uint8_t    ID��
 * ��������  ���Աȳɹ������հ��Ĳ�����ID�ţ�ʧ��0x00
 *			   �Աȳɹ������հ���result��0x00��ʧ��0x92
 */
uint8_t Match_l_n(void)
{
	uint8_t ret;
	uint8_t rec_parameter = 0xff;
	static uint8_t show_flag = 1;
	
	if(show_flag)
	{
		printf("�������ָ����ָ�ƶԱȿ���\r\n");
		show_flag = 0;
	}
	
	//����ָ��,��Ӧ�ϵ�
	if(MG200_DETECT)
	{
		MG200_PWR_ON;
		tim5_delay_ms(30);
		
		/* �ɼ�ָ�� */
		do
		{
			ret = ReqCaptureAndExtract(0x00);
		}while(ret);
		
		
		/* �Ա�ָ�� */
		mg200_write_cmd(0x71,0);//���ͱȶԵ���Ϣ
		if(mg200_read_cmd(0x71,&rec_parameter,&ret) != 0)
		{
			printf("ͨ��ʧ��,�������ݰ�����\r\n");
			MG200_PWR_OFF;
			voice(DOOROPEN_FAIL); 
			return 0xff;
		}
		
		/* ���ݶԱȽ��ִ�п��� */
		if(ret == 0x00)
		{
			printf("�ȶԳɹ�\r\n");
			voice(DOOROPEN_SUCCESS);
			open_door();
			while(MG200_DETECT);              //̧��ָ�ƣ�����
			close_door();
		}
		else//0x92
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






/**
 * ������    ��erase_one
 * ��������  ��ɾ��ָ��IDָ��
 * ��������  ��uint8_t ID��
 * ��������ֵ��uint8_t 
 * ��������  ��
 */
uint8_t erase_one(uint8_t id)
{
	uint8_t rec_parameter,ret;
	
	MG200_PWR_ON;
	tim5_delay_ms(30);
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



/**
 * ������    ��erase_all
 * ��������  ��ɾ������ָ��ID
 * ��������  ����
 * ��������ֵ��uint8_t 
 * ��������  ��
 */
uint8_t erase_all(void)
{
	uint8_t rec_parameter,ret;
	
	MG200_PWR_ON;
	tim5_delay_ms(30);
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












