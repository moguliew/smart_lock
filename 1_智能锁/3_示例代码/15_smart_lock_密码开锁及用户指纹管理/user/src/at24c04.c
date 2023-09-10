#include "stm32f4xx.h"                  // Device header
#include "timer.h" 
#include "at24c04.h"
/*********AT24C04 _ IIC*************************/




/***********************************************
*������    :iic_IO_init
*��������  :IIC����IO�ڳ�ʼ������
*��������  :��
*��������ֵ:��
*��������  :PA8----SCL   ͨ���������
            PC9----SDA   ͨ�ÿ�©���
************************************************/
void iic_IO_init(void)
{
	RCC->AHB1ENR |= (1<<0);
	RCC->AHB1ENR |= (1<<2);
	GPIOA->MODER &= ~(3<<16) ;
	GPIOA->MODER |= (1<<16) ;
	GPIOC->MODER &= ~(3<<18) ;
	GPIOC->MODER |= (1<<18) ;
	//PA8����
	GPIOA->OTYPER &= ~(1<<8);
	//PC9��©
	GPIOC->OTYPER |= (1<<9);
	
	//����״̬
	IIC_SCL_H;
	IIC_SDA_OUT_H;
}




/***********************************************
*������    :iic_star
*��������  :IIC��ʼ�źź���
*��������  :��
*��������ֵ:��
*��������  :ʱ����Ϊ�ߵ�ƽ�������߳����½���
************************************************/

void iic_star(void)
{
	//ʱ�������ͣ��ſ��Զ�������
	IIC_SCL_L;  
	//���������ߣ�Ŀ����Ϊ�����½�����׼��
	IIC_SDA_OUT_H;   
	//ʱ��������  ������ʼ�źŵ�����
	IIC_SCL_H;
	Tim5_delay_us(5);
	//����������  ���Բ�����ʼ�ź�
	IIC_SDA_OUT_L;
	Tim5_delay_us(5);
	
	//��ȫ
	IIC_SCL_L;
}



/***********************************************
*������    :iic_stop
*��������  :IICֹͣ�źź���
*��������  :��
*��������ֵ:��
*��������  :ʱ����Ϊ�ߵ�ƽ�������߳���������
************************************************/
void iic_stop(void)
{
	//׼������
	IIC_SCL_L;
	IIC_SDA_OUT_L;
	
	//����ֹͣ�ź�
	IIC_SCL_H;
	Tim5_delay_us(5);
	IIC_SDA_OUT_H;
	Tim5_delay_us(5);
}



/***********************************************
*������    :iic_send_ack
*��������  :����Ӧ��/��Ӧ���ź�
*��������  :u8 ack
*��������ֵ:��
*��������  :Ӧ�� ʱ����Ϊ�ߵ�ƽ���������Ѿ����ֺõ͵�ƽ��
						��Ӧ�� ʱ����Ϊ�ߵ�ƽ���������Ѿ����ֺøߵ�ƽ��				
						������0   ����Ӧ���ź�
						������1   ���Ͳ�Ӧ���ź�
************************************************/
void iic_send_ack(u8 ack)
{
	
	if(ack == 0)       //����Ӧ��
	{
		IIC_SCL_L;      //ʱ�������ͣ��ſ��Զ�������
		Tim5_delay_us(5); 
		IIC_SDA_OUT_L;  //���������ȱ��ֵ͵�ƽ
		Tim5_delay_us(5); 
		IIC_SCL_H;      //������Ӧ���ź�	
		Tim5_delay_us(5); 
	}
	
	else if (ack == 1) //���Ͳ�Ӧ���� 
	{
		IIC_SCL_L;      //ʱ�������ͣ��ſ��Զ�������
		Tim5_delay_us(5); 
		IIC_SDA_OUT_H;  //���������ȱ��ָߵ�ƽ
		Tim5_delay_us(5); 
		IIC_SCL_H;      //�����˲�Ӧ���ź�
		Tim5_delay_us(5); 
	}

	//��ȫ
	IIC_SCL_L;
}



/***********************************************
*������    :iic_get_ack
*��������  :���Ӧ��/��Ӧ���ź�
*��������  :��
*��������ֵ:u8
*��������  :
						����0 ��ʾӦ���ź�
						����1 ��ʾ��Ӧ���ź�
************************************************/
u8 iic_get_ack(void)
{
	u8 ack;
	
	/*������ҪΪ����*/
	IIC_SCL_L;
	IIC_SDA_OUT_H;
	
	/*���Ӧ��/��Ӧ���ź�*/
	IIC_SCL_L;   //���������ӻ�����ʱ���ߣ��ӻ��ſ��Զ������ߣ�������Ӧ��/��Ӧ���ź�
	Tim5_delay_us(5);  
	IIC_SCL_H;   //�����ſ��Զ�������
  Tim5_delay_us(5);  
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



/***********************************************
*������    :iic_send_byte
*��������  :IIC����һ���ֽں���
*��������  :u8 data
*��������ֵ:��
*��������  :
************************************************/
void iic_send_byte(u8 data)
{
	u8 i;
	for(i=0;i<8;i++)
	{
		//ʱ�������Ͳſ��Ըı�������
		IIC_SCL_L;
		Tim5_delay_us(5);  
		//���ݲ����Ķ�Ӧλ��0����1���������ߵĸߵ�
		if(data & 0x80)
		{
			IIC_SDA_OUT_H;
		}
		else
		{
			IIC_SDA_OUT_L;
		}
		Tim5_delay_us(5);  
		//ʱ�������ߣ������ӻ��������ӻ��ſ��Զ���һλ����
		IIC_SCL_H;
		Tim5_delay_us(5);  
		//��һλ���ݣ��θ�λ������λ
		data = data << 1;
	}
	//��ȫ
	IIC_SCL_L;
}




/*******************************************
*������    :iic_read_byte
*��������  :����ͨ��IIC����һ���ֽں���
*��������  :��
*��������ֵ:u8
*��������  :
*********************************************/
u8 iic_read_byte(void)
{
	u8 data;    
	u8 i;
	
	/*��IO���л�Ϊ����*/
	IIC_SCL_L;
	IIC_SDA_OUT_H;
	
	/*������*/
	for(i=0;i<8;i++)
	{
		IIC_SCL_L;             //���������ӻ�����ʱ����
		Tim5_delay_us(5);   
		IIC_SCL_H;             //�������Զ�����
		Tim5_delay_us(5);  
		data = data << 1;
		if(IIC_SDA_INT) 
			data= data | 0x01;		
	}
	
	//��ȫ
	IIC_SCL_L;
	
	return data;
}



/************************AT24C04****************************/


/********************************************************
*������    :at24c04_init
*��������  :��AT24C04�ĳ�ʼ��
*��������  :
*��������ֵ:
**********************************************************/
void at24c04_init(void)
{
	iic_IO_init();
}




/********************************************************
*������    :at24c04_write_byte
*��������  :��AT24C04��ĳ����ַ�ռ�дһ���ֽ�����
*��������  :u16 inner_addr     //�����ڲ��洢�ռ��ַ  0~511   
						u8 data           //Ҫд�������
*��������ֵ:ÿ�η��Ͷ�������Ӧ��ͨ������ֵ�жϾ����Ĵη��ͳ�����			
*��������  :�궨������־��
							AT24C04_NO_ERR 0;   //Ӧ��  �޴���
							AT24C04_ERR1   1;   //��Ӧ��
							AT24C04_ERR2   2;   //��Ӧ��
							AT24C04_ERR3   3;   //��Ӧ��
**********************************************************/
u8 AT24C04_write_byte(u16 inner_addr,u8 data)
{
	u8 ack;
	
	iic_star();
	
	if(inner_addr<=255)   //�浽��һ��
	{
		iic_send_byte(AT24C04_ADDR0_W);    //��һ���д������ַ
		
	}
	else                  //��ڶ���
	{
		iic_send_byte(AT24C04_ADDR1_W);   //255      256 0     257 1
		//inner_addr = inner_addr - 256;    //�ڶ����ʵ�ʵ�ַ
		//inner_addr = inner_addr & 0xff;    //�ڶ����ʵ�ʵ�ַ
	}
	ack = iic_get_ack();
	if(ack == 1)
	{
		iic_stop();
		return AT24C04_ERR1; 
	}
	
	/*�������Ե�ַ*/
	iic_send_byte(inner_addr & 0xff);     
	ack = iic_get_ack();
	if(ack == 1)
	{
		iic_stop();
		return AT24C04_ERR2; 
	}
	
	
	//��������
	iic_send_byte(data);
	ack = iic_get_ack();
	if(ack == 1)
	{
		iic_stop();
		return AT24C04_ERR3; 
	}
	iic_stop();
	Tim5_delay_ms(5);
	
	return AT24C04_NO_ERR;
}





/********************************************************
*������    :AT24C04_read_byte
*��������  :��AT24C04��ĳ����ַ�ռ��һ���ֽ�����
*��������  :u16 inner_addr     //�����ڲ��洢�ռ��ַ  0~511   
						u8 *data          //���������ݴ�ŵĵ�ַ
*��������ֵ:ÿ�η��Ͷ�������Ӧ��ͨ������ֵ�жϾ����Ĵη��ͳ�����			
*��������  :�궨������־��
							AT24C04_NO_ERR 0;   //Ӧ��  �޴���
							AT24C04_ERR1   1;   //��Ӧ��
							AT24C04_ERR2   2;   //��Ӧ��
							AT24C04_ERR3   3;   //��Ӧ��
**********************************************************/
u8 AT24C04_read_byte(u16 inner_addr,u8 *data)
{
	u8 ack;
	
	iic_star();
	if(inner_addr<=255)   //�浽��һ��
	{
		iic_send_byte(AT24C04_ADDR0_W);    //��һ���д������ַ
	}
	else                  //��ڶ���
	{
		iic_send_byte(AT24C04_ADDR1_W);    //�ڶ����д������ַ
		//inner_addr = inner_addr - 256;    //�ڶ����ʵ�ʾ��Ե�ַ
		//inner_addr = inner_addr & 0xff;    //�ڶ����ʵ�ʾ��Ե�ַ
	} 
	ack = iic_get_ack();
	if(ack == 1)
	{
		iic_stop();
		return AT24C04_ERR1; 
	}
	
	/*���;��Ե�ַ*/
	iic_send_byte(inner_addr&0xff);
	ack = iic_get_ack();
	if(ack == 1)
	{
		iic_stop();
		return AT24C04_ERR2; 
	}
	
	
	iic_star();
	if(inner_addr<=255)   //����һ��
	{
		iic_send_byte(AT24C04_ADDR0_R);    //��һ��Ķ�������ַ
	}
	else                  //���ڶ���
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



/********************************************************
*������    :AT24C04_read_bytes
*��������  :��AT24C04��ĳ����ַ�ռ俪ʼ����������ֽ�����
*��������  :u16 inner_addr     //�����ڲ��洢�ռ��ַ  0~511  
            u8 num            //Ҫ�����ٸ��ֽ�
						u8 *str           //���������ݴ�ŵĵ�ַ
*��������ֵ:ÿ�η��Ͷ�������Ӧ��ͨ������ֵ�жϾ����Ĵη��ͳ�����			
*��������  :�궨������־��
							AT24C04_NO_ERR 0;   //Ӧ��  �޴���
							AT24C04_ERR1   1;   //��Ӧ��
							AT24C04_ERR2   2;   //��Ӧ��
							AT24C04_ERR3   3;   //��Ӧ��
**********************************************************/
u8 AT24C04_read_bytes(u16 inner_addr,u8 num,u8 *str)
{
	u8 ack;
	
	iic_star();
	if(inner_addr<=255)   //�浽��һ��
	{
		iic_send_byte(AT24C04_ADDR0_W);    //��һ���д������ַ
		
	}
	else                  //��ڶ���
	{
		iic_send_byte(AT24C04_ADDR1_W);    //�ڶ����д������ַ
	} 
	ack = iic_get_ack();
	if(ack == 1)
	{
		iic_stop();
		return AT24C04_ERR1; 
	}	
	
	/*���;��Ե�ַ*/
	iic_send_byte(inner_addr&0xff);
	ack = iic_get_ack();
	if(ack == 1)
	{
		iic_stop();
		return AT24C04_ERR2; 
	}
	
	iic_star();
	if(inner_addr<=255)   //����һ��
	{
		iic_send_byte(AT24C04_ADDR0_R);    //��һ��Ķ�������ַ
	}
	else                  //���ڶ���
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



/********************************************************
*������    :AT24C04_write_page
*��������  :��AT24C04��ĳ����ַ�ռ俪ʼҳд
*��������  :u16 inner_addr     //�����ڲ��洢�ռ��ַ  0~511  
            u8 num            //Ҫ�����ٸ��ֽ�
						u8 *str           //Ҫд���ַ������׵�ַ
*��������ֵ:ÿ�η��Ͷ�������Ӧ��ͨ������ֵ�жϾ����Ĵη��ͳ�����			
*��������  :�궨������־��
							AT24C04_NO_ERR 0;   //Ӧ��  �޴���
							AT24C04_ERR1   1;   //��Ӧ��
							AT24C04_ERR2   2;   //��Ӧ��
							AT24C04_ERR3   3;   //��Ӧ��
              AT24C04_ERR4   4;   //��ҳ����
**********************************************************/
u8 AT24C04_write_page(u16 inner_addr,u8 num,u8 *str)
{
	u8 ack;
	
	/*�ж�Ҫд�������Ƿ���ҳ*/
	if(inner_addr / 16 != (inner_addr + num - 1) / 16)
	{
		return AT24C04_ERR4;
	}
	
	/*ҳд*/
	iic_star();
	if(inner_addr<=255)   //�浽��һ��
	{
		iic_send_byte(AT24C04_ADDR0_W);    //��һ���д������ַ
		
	}
	else                  //��ڶ���
	{
		iic_send_byte(AT24C04_ADDR1_W);    //�ڶ����д������ַ
	} 
	ack = iic_get_ack();
	if(ack == 1)
	{
		iic_stop();
		return AT24C04_ERR1; 
	}	
	
	/*���;��Ե�ַ*/
	iic_send_byte(inner_addr&0xff);
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
	Tim5_delay_ms(5);
	
	return AT24C04_NO_ERR;
}




/********************************************************
*������    :AT24C04_write_bytes
*��������  :��AT24C04��ĳ����ַ�ռ俪ʼ����д���Կ�ҳ
*��������  :u16 inner_addr     //�����ڲ��洢�ռ��ַ  0~511  
            u8 num            //Ҫ�����ٸ��ֽ�
						u8 *str           //Ҫд���ַ������׵�ַ
*��������ֵ:ÿ�η��Ͷ�������Ӧ��ͨ������ֵ�жϾ����Ĵη��ͳ�����			
*��������  :�궨������־��
							AT24C04_NO_ERR 0;   //Ӧ��  �޴���
							AT24C04_ERR1   1;   //��Ӧ��
							AT24C04_ERR2   2;   //��Ӧ��
							AT24C04_ERR3   3;   //��Ӧ��
**********************************************************/
u8 AT24C04_write_bytes(u16 inner_addr,u8 num,u8 *str)
{
	u8 less_num;
	
	while(1)
	{
		//���㱾ҳ��ʣ���ٸ��ռ����д
		less_num = 16 - inner_addr % 16;     
		//�ж��Ƿ���ҳ
		if(less_num >= num)       //����ҳ
		{
			//����ҳд������д��
			AT24C04_write_page(inner_addr,num,str);  
			//�˳�ѭ���ṹ
			break;
			
		}
		//��ҳ
		else
		{
			//�ȵ���ҳд�����ѱ�ҳд��
			AT24C04_write_page(inner_addr,less_num,str);  //253  3  
			//�洢оƬ�л�����ҳ����ʼλ��λ��
			inner_addr = inner_addr + less_num;   //253+3==256
			
			//�����ʣ��Ҫд�ĸ���
			num = num - less_num;                 //7
			
			//�����ʣ��Ҫд�����ݵĵ�ַ
			str = str + less_num;	
		}
	}		
	return AT24C04_NO_ERR;
}



















