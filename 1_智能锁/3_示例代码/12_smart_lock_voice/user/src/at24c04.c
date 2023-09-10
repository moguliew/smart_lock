#include "stm32f4xx.h"                  // Device header
#include "tim.h" 
#include "at24c04.h"

/************AT24C04_IIC**********************/
/**
 * ������    :iic_io_init
 * ��������  :IIC����IO�ڳ�ʼ������
 * ��������  :��
 * ��������ֵ:�� 
 * ��������  :PA8----SCL   ͨ���������
 *            PC9----SDA   ͨ�ÿ�©���
 */
void iic_io_init(void)
{
	//��GPIOʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	
	//����ģʽ
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	//����״̬
	IIC_SCL_H;
	IIC_SDA_OUT_H;
}


/**
 * ������    :iic_start
 * ��������  :iic��ʼ�źź���
 * ��������  :��
 * ��������ֵ:��
 * ��������  :ʱ����Ϊ�ߵ�ƽ�������߳����½���
 */

void iic_start(void)
{
	//ʱ�������ͣ��ſ��Զ�������
	IIC_SCL_L;  
	//���������ߣ�Ŀ����Ϊ�����½�����׼��
	IIC_SDA_OUT_H;   
	//ʱ��������  ������ʼ�źŵ�����
	IIC_SCL_H;
	tim5_delay_us(5);
	//����������  ���Բ�����ʼ�ź�
	IIC_SDA_OUT_L;
	tim5_delay_us(5);
	
	//��ȫ
	IIC_SCL_L;
}



/**
 * ������    :iic_stop
 * ��������  :iicֹͣ�źź���
 * ��������  :��
 * ��������ֵ:��
 * ��������  :ʱ����Ϊ�ߵ�ƽ�������߳���������
 */
void iic_stop(void)
{
	//׼������
	IIC_SCL_L;
	IIC_SDA_OUT_L;
	
	//����ֹͣ�ź�
	IIC_SCL_H;
	tim5_delay_us(5);
	IIC_SDA_OUT_H;
	tim5_delay_us(5);
}



/**
 * ������    :iic_send_ack
 * ��������  :����Ӧ��/��Ӧ���ź�
 * ��������  :0--����Ӧ���źš�1--���Ͳ�Ӧ���ź� 
 * ��������ֵ:��
 * ��������  :Ӧ��ʱ����Ϊ�ߵ�ƽ���������Ѿ����ֺõ͵�ƽ��
 *			 ��Ӧ��: ʱ����Ϊ�ߵ�ƽ���������Ѿ����ֺøߵ�ƽ��				
 */
void iic_send_ack(uint8_t  ack)
{
	
	if(ack == 0)       		//����Ӧ��
	{
		IIC_SCL_L;      	//ʱ�������ͣ��ſ��Զ�������
		tim5_delay_us(5); 
		IIC_SDA_OUT_L;  	//���������ȱ��ֵ͵�ƽ
		tim5_delay_us(5); 
		IIC_SCL_H;      	//������Ӧ���ź�	
		tim5_delay_us(5); 
	}
	else if (ack == 1) 		//���Ͳ�Ӧ���� 
	{
		IIC_SCL_L;      	//ʱ�������ͣ��ſ��Զ�������
		tim5_delay_us(5); 
		IIC_SDA_OUT_H;  	//���������ȱ��ָߵ�ƽ
		tim5_delay_us(5); 
		IIC_SCL_H;      	//�����˲�Ӧ���ź�
		tim5_delay_us(5); 
	}

	//��ȫ
	IIC_SCL_L;
}



/**
 * ������    :iic_get_ack
 * ��������  :���Ӧ��/��Ӧ���ź�
 * ��������  :��
 * ��������ֵ:����0 ��ʾӦ���źţ�����1 ��ʾ��Ӧ���ź� 
 * ��������  :		
 */
uint8_t  iic_get_ack(void)
{
	uint8_t  ack;
	
	/* ������ҪΪ���� */
	IIC_SCL_L;
	IIC_SDA_OUT_H;
	
	/* ���Ӧ��/��Ӧ���ź� */
	IIC_SCL_L;   			//���������ӻ�����ʱ���ߣ��ӻ��ſ��Զ������ߣ�������Ӧ��/��Ӧ���ź�
	tim5_delay_us(5);  
	IIC_SCL_H;   			//�����ſ��Զ�������
	tim5_delay_us(5);  
	if(IIC_SDA_INT == 0)
	{
		//�͵�ƽ����Ӧ���ź�
		ack = 0;
	}
	else
	{
		//�ߵ�ƽ----��Ӧ���ź�
		ack = 1;
	}
	
	//��ȫ
	IIC_SCL_L;
	return ack;
}



/*
*������    :iic_send_byte
*��������  :IIC����һ���ֽں���
*��������  :uint8_t  data
*��������ֵ:��
*��������  :
*/
void iic_send_byte(uint8_t  data)
{
	uint8_t  i;
	for(i = 0;i < 8;i++)
	{
		//ʱ�������Ͳſ��Ըı�������
		IIC_SCL_L;
		tim5_delay_us(5);  
		//���ݲ����Ķ�Ӧλ��0����1���������ߵĸߵ�
		if(data & 0x80)
		{
			IIC_SDA_OUT_H;
		}
		else
		{
			IIC_SDA_OUT_L;
		}
		tim5_delay_us(5);  
		//ʱ�������ߣ������ӻ��������ӻ��ſ��Զ���һλ����
		IIC_SCL_H;
		tim5_delay_us(5);  
		//��һλ���ݣ��θ�λ������λ
		data = data << 1;
	}
	
	//��ȫ
	IIC_SCL_L;
}




/**
 * ������    :iic_read_byte
 * ��������  :����ͨ��IIC����һ���ֽں���
 * ��������  :��
 * ��������ֵ:uint8_t 
 * ��������  :
 */
uint8_t  iic_read_byte(void)
{
	uint8_t  data;    
	uint8_t  i;
	
	/* ��IO���л�Ϊ���� */
	IIC_SCL_L;
	IIC_SDA_OUT_H;
	
	/* ������ */
	for(i = 0;i < 8;i++)
	{
		IIC_SCL_L;             //���������ӻ�����ʱ����
		tim5_delay_us(5);   
		IIC_SCL_H;             //�������Զ�����
		tim5_delay_us(5);  
		data = data << 1;
		if(IIC_SDA_INT) 
			data = data | 0x01;		
	}
	
	//��ȫ
	IIC_SCL_L;
	
	return data;
}



/************************AT24C04*************************/
/**
 * ������    :at24c04_init
 * ��������  :��AT24C04�ĳ�ʼ��
 * ��������  :
 * ��������ֵ:
 */
void at24c04_init(void)
{
	iic_io_init();
}


/**
 * ������    :at24c04�ֽ�д
 * ��������  :��AT24C04��ĳ����ַ�ռ�дһ���ֽ�����
 * ��������  :inner_addr--�����ڲ��洢�ռ��ַ��0~511����data--Ҫд�������
 * ��������ֵ:ÿ�η��Ͷ�������Ӧ��ͨ������ֵ�жϾ����Ĵη��ͳ�����			
 * ��������  :�궨������־��
 *				AT24C04_NO_ERR 0;   //Ӧ���޴���
 *				AT24C04_ERR1   1;   //��Ӧ��
 *				AT24C04_ERR2   2;   //��Ӧ��
 *				AT24C04_ERR3   3;   //��Ӧ��
 */
uint8_t  at24c04_write_byte(uint16_t  inner_addr,uint8_t  data)
{
	uint8_t  ack;
	
	iic_start();
	
	if(inner_addr <= 255)//�浽��һ��
	{
		iic_send_byte(AT24C04_ADDR0_W);    //��һ���д������ַ
		
	}
	else//��ڶ���
	{
		iic_send_byte(AT24C04_ADDR1_W);   
		//inner_addr = inner_addr - 256;    //�ڶ����ʵ�ʵ�ַ
		//inner_addr = inner_addr & 0xff;    //�ڶ����ʵ�ʵ�ַ
	}
	ack = iic_get_ack();
	if(ack == 1)
	{
		iic_stop();
		return AT24C04_ERR1; 
	}
	
	/* �������Ե�ַ */
	iic_send_byte(inner_addr & 0xff);     
	ack = iic_get_ack();
	if(ack == 1)
	{
		iic_stop();
		return AT24C04_ERR2; 
	}
	
	/* �������� */
	iic_send_byte(data);
	ack = iic_get_ack();
	if(ack == 1)
	{
		iic_stop();
		return AT24C04_ERR3; 
	}
	iic_stop();
	tim5_delay_ms(5);
	
	return AT24C04_NO_ERR;
}



/**
 * ������    :at24c04_read_byte
 * ��������  :��AT24C04��ĳ����ַ�ռ��һ���ֽ�����
 * ��������  :inner_addr--�����ڲ��洢�ռ��ַ(0~511)��uint8_t *data--���������ݴ�ŵĵ�ַ
 * ��������ֵ:ÿ�η��Ͷ�������Ӧ��ͨ������ֵ�жϾ����Ĵη��ͳ�����			
 * ��������  :�궨������־��
 *				AT24C04_NO_ERR 0;   //Ӧ���޴���
 *				AT24C04_ERR1   1;   //��Ӧ��
 *				AT24C04_ERR2   2;   //��Ӧ��
 *				AT24C04_ERR3   3;   //��Ӧ��
 */
uint8_t  at24c04_read_byte(uint16_t  inner_addr,uint8_t  *data)
{
	uint8_t  ack;
	
	iic_start();
	if(inner_addr <= 255)//�ӵ�һ���
	{
		iic_send_byte(AT24C04_ADDR0_W);    //��һ���д������ַ
	}
	else//�ӵڶ����
	{
		iic_send_byte(AT24C04_ADDR1_W);    //�ڶ����д������ַ
	} 
	ack = iic_get_ack();
	if(ack == 1)
	{
		iic_stop();
		return AT24C04_ERR1; 
	}
	
	/* ���;��Ե�ַ */
	iic_send_byte(inner_addr & 0xff);
	ack = iic_get_ack();
	if(ack == 1)
	{
		iic_stop();
		return AT24C04_ERR2; 
	}
	
	
	iic_start();
	if(inner_addr <= 255)//����һ��
	{
		iic_send_byte(AT24C04_ADDR0_R);    //��һ��Ķ�������ַ
	}
	else//���ڶ���
	{
		iic_send_byte(AT24C04_ADDR1_R);    //�ڶ���Ķ�������ַ   
	} 
	ack = iic_get_ack();
	if(ack == 1)
	{
		iic_stop();
		return AT24C04_ERR1; 
	}	
	
	*data = iic_read_byte();
	iic_send_ack(1);
	iic_stop();
	
	return AT24C04_NO_ERR;

}



/**
 * ������    :at24c04_read_bytes
 * ��������  :��AT24C04��ĳ����ַ�ռ俪ʼ����������ֽ�����
 * ��������  :inner_addr-----�����ڲ��洢�ռ��ַ��0~511����
 *			  num------------Ҫ�����ٸ��ֽڣ�
 *            uint8_t  *str--���������ݴ�ŵĵ�ַ
 * ��������ֵ:ÿ�η��Ͷ�������Ӧ��ͨ������ֵ�жϾ����Ĵη��ͳ�����			
 * ��������  :�궨������־��
 *				AT24C04_NO_ERR 0;   //Ӧ���޴���
 *				AT24C04_ERR1   1;   //��Ӧ��
 *				AT24C04_ERR2   2;   //��Ӧ��
 *				AT24C04_ERR3   3;   //��Ӧ��
 */
uint8_t  at24c04_read_bytes(uint16_t  inner_addr,uint8_t  num,uint8_t  *str)
{
	uint8_t  ack;
	
	iic_start();
	if(inner_addr <= 255)//�浽��һ��
	{
		iic_send_byte(AT24C04_ADDR0_W);    //��һ���д������ַ
	}
	else//��ڶ���
	{
		iic_send_byte(AT24C04_ADDR1_W);    //�ڶ����д������ַ
	} 
	ack = iic_get_ack();
	if(ack == 1)
	{
		iic_stop();
		return AT24C04_ERR1; 
	}	
	
	/* ���;��Ե�ַ */
	iic_send_byte(inner_addr & 0xff);
	ack = iic_get_ack();
	if(ack == 1)
	{
		iic_stop();
		return AT24C04_ERR2; 
	}
	
	iic_start();
	if(inner_addr <= 255)//����һ��
	{
		iic_send_byte(AT24C04_ADDR0_R);    //��һ��Ķ�������ַ
	}
	else//���ڶ���
	{
		iic_send_byte(AT24C04_ADDR1_R);    //�ڶ���Ķ�������ַ   
	} 
	ack = iic_get_ack();
	if(ack == 1)
	{
		iic_stop();
		return AT24C04_ERR1; 
	}	
	
	//ѭ��������
	while(1)
	{
		*str = iic_read_byte();
		num--;
		if(num == 0)
		{
			iic_send_ack(1);
			break;
		}
		iic_send_ack(0);
		str++;
	}
	
	iic_stop();
	
	return AT24C04_NO_ERR;
}



/**
 * ������    :at24c04_write_page
 * ��������  :��AT24C04��ĳ����ַ�ռ俪ʼҳд
 * ��������  :uint16_t  inner_addr--�����ڲ��洢�ռ��ַ��0~511��  
 *            uint8_t  num----------Ҫд���ٸ��ֽ�
 *			  uint8_t  *str---------Ҫд���ַ������׵�ַ
 * ��������ֵ:ÿ�η��Ͷ�������Ӧ��ͨ������ֵ�жϾ����Ĵη��ͳ�����			
 * ��������  :�궨������־��
 *				AT24C04_NO_ERR 0;   //Ӧ���޴���
 *				AT24C04_ERR1   1;   //��Ӧ��
 *				AT24C04_ERR2   2;   //��Ӧ��
 *				AT24C04_ERR3   3;   //��Ӧ��
 *				AT24C04_ERR4   4;   //��ҳ����
 */
uint8_t  at24c04_write_page(uint16_t  inner_addr,uint8_t  num,uint8_t  *str)
{
	uint8_t  ack;
	
	/* �ж�Ҫд�������Ƿ���ҳ */
	if(inner_addr / 16 != (inner_addr + num - 1) / 16)
	{
		return AT24C04_ERR4;
	}
	
	/* ҳд */
	iic_start();
	if(inner_addr <= 255)//�浽��һ��
	{
		iic_send_byte(AT24C04_ADDR0_W);    //��һ���д������ַ
	}
	else//��ڶ���
	{
		iic_send_byte(AT24C04_ADDR1_W);    //�ڶ����д������ַ
	} 
	ack = iic_get_ack();
	if(ack == 1)
	{
		iic_stop();
		return AT24C04_ERR1; 
	}	
	
	/* ���;��Ե�ַ */
	iic_send_byte(inner_addr & 0xff);
	ack = iic_get_ack();
	if(ack == 1)
	{
		iic_stop();
		return AT24C04_ERR2; 
	}
	
	while(1)
	{
		iic_send_byte(*str);
		ack = iic_get_ack();
		if(ack == 1)
		{
			iic_stop();
			return AT24C04_ERR3; 
		}	
		num--;
		if(num == 0)
		{
			break;
		}
		str++;
	}
	iic_stop();
	
	//д����
	tim5_delay_ms(5);
	
	return AT24C04_NO_ERR;
}




/**
 * ������    :at24c04_write_bytes
 * ��������  :��AT24C04��ĳ����ַ�ռ俪ʼ����д���Կ�ҳ
 * ��������  :uint16_t  inner_addr--�����ڲ��洢�ռ��ַ  0~511  
 *            uint8_t  num----------Ҫ�����ٸ��ֽ�
 *			  uint8_t  *str---------Ҫд���ַ������׵�ַ
 * ��������ֵ:ÿ�η��Ͷ�������Ӧ��ͨ������ֵ�жϾ����Ĵη��ͳ�����			
 * ��������  :�궨������־��
 *				AT24C04_NO_ERR 0;   //Ӧ���޴���
 *				AT24C04_ERR1   1;   //��Ӧ��
 *				AT24C04_ERR2   2;   //��Ӧ��
 *				AT24C04_ERR3   3;   //��Ӧ��
 */
uint8_t  at24c04_write_bytes(uint16_t  inner_addr,uint8_t  num,uint8_t *str)
{
	uint8_t  less_num;
	
	while(1)
	{	
		less_num = 16 - (inner_addr % 16);//���㱾ҳ��ʣ���ٸ��ռ����д     
		/* �ж��Ƿ���ҳ */
		if(less_num >= num)//����ҳ
		{
			at24c04_write_page(inner_addr,num,str);//����ҳд������д��  
			break;//�˳�ѭ���ṹ	
		}		
		else//��ҳ
		{
			//�ȵ���ҳд�����ѱ�ҳд��
			at24c04_write_page(inner_addr,less_num,str);
			//�洢оƬ�л�����ҳ����ʼλ��λ��
			inner_addr = inner_addr + less_num;
			
			//�����ʣ��Ҫд�ĸ���
			num = num - less_num;                 //7
			//�����ʣ��Ҫд�����ݵĵ�ַ
			str = str + less_num;	
		}
	}		
	return AT24C04_NO_ERR;
}



















