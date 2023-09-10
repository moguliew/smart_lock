#include "stm32f4xx.h"                  // Device header
#include "bs8116.h"
#include "tim.h"
#include "stdio.h"
#include "w25q64.h"
#include "at24c04.h"
#include "lcd.h"
#include "door.h"
#include "string.h"
#include "voice.h"
/********************************************IIC************************************/
/**
 * ������    :bs8116_iic_io_init
 * ��������  :IIC����IO�ڳ�ʼ������
 * ��������  :��
 * ��������ֵ:��
 * ��������  : PB6----SCL   ͨ�ÿ�©���
 *             PB7----SDA   ͨ�ÿ�©���
 */
void bs8116_iic_io_init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStruct;
	
	//�˿�ʱ��ʹ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;          	//ͨ�����
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;         	//��©
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 ;  	//6��7
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;       	//��������
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;       	//�ٶ�
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	
	//����״̬
	GPIO_SetBits(GPIOB, GPIO_Pin_6);//SCL
	GPIO_SetBits(GPIOB, GPIO_Pin_7);//SDA
	
}



/**
 * ������    :iic_start
 * ��������  :IIC��ʼ�źź���
 * ��������  :��
 * ��������ֵ:��
 * ��������  :ʱ����Ϊ�ߵ�ƽ�������߳����½���
 */
void bs8116_iic_start(void)
{
	//ʱ�������ͣ��ſ��Զ�������
	BS8116_IIC_SCL_L; 
	tim5_delay_us(5);
	//���������ߣ�Ŀ����Ϊ�����½�����׼��
	BS8116_IIC_SDA_OUT_H;   
	//ʱ��������  ������ʼ�źŵ�����
	BS8116_IIC_SCL_H;
	tim5_delay_us(5);
	//����������  ���Բ�����ʼ�ź�
	BS8116_IIC_SDA_OUT_L;
	tim5_delay_us(5);
	
	//��ȫ
	//BS8116_IIC_SCL_L;
	
}



/**
 * ������    :iic_stop
 * ��������  :IICֹͣ�źź���
 * ��������  :��
 * ��������ֵ:��
 * ��������  :ʱ����Ϊ�ߵ�ƽ�������߳���������
 */
void bs8116_iic_stop(void)
{
	
	//׼������
	BS8116_IIC_SCL_L;
	tim5_delay_us(5);
	BS8116_IIC_SDA_OUT_L;
	
	//����ֹͣ�ź�
	BS8116_IIC_SCL_H;
	tim5_delay_us(5);
	BS8116_IIC_SDA_OUT_H;
	tim5_delay_us(5);

}



/**
 * ������    :iic_send_ack
 * ��������  :����Ӧ��/��Ӧ���ź�
 * ��������  :0--����Ӧ���ź�,1--���Ͳ�Ӧ���ź�
 * ��������ֵ:��
 * ��������  :Ӧ�� ʱ����Ϊ�ߵ�ƽ���������Ѿ����ֺõ͵�ƽ��
 *					��Ӧ�� ʱ����Ϊ�ߵ�ƽ���������Ѿ����ֺøߵ�ƽ��				
 */
void bs8116_iic_send_ack(uint8_t ack)
{
	BS8116_IIC_SCL_L;	  		//ʱ�������ͣ��ſ��Զ�������
	tim5_delay_us(5);
	if(ack == 0)	   			//����Ӧ��
	{
		BS8116_IIC_SDA_OUT_L;   //���������ȱ��ֵ͵�ƽ
	}
	else if(ack == 1)
	{
		BS8116_IIC_SDA_OUT_H;   //���������ȱ��ָߵ�ƽ
	}
	tim5_delay_us(5);
	BS8116_IIC_SCL_H;	  	    //������Ӧ���ź�	
	tim5_delay_us(5);

	//��ȫ
	//IIC_SCL_L;	
}



/**
 * ������    :iic_get_ack
 * ��������  :���Ӧ��/��Ӧ���ź�
 * ��������  :��
 * ��������ֵ:����0--��ʾӦ���ź�,����1--��ʾ��Ӧ���ź�
 * ��������  :			
 */
uint8_t bs8116_iic_get_ack(void)
{	
	uint8_t ack = 0;
	
	//������ҪΪ����
	//BS8116_IIC_SCL_L;
	//�������л�����״̬
	BS8116_IIC_SDA_OUT_H;    
	
	//���Ӧ��/��Ӧ���ź�
	BS8116_IIC_SCL_L;   //���������ӻ�����ʱ���ߣ��ӻ��ſ��Զ������ߣ�������Ӧ��/��Ӧ���ź�
	tim5_delay_us(5);  
	BS8116_IIC_SCL_H;   //�����ſ��Զ�������
	tim5_delay_us(5);
	if(BS8116_IIC_SDA_INT == 0)
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
	//BS8116_IIC_SCL_L;
	return ack;
		
}



/**
 * ������    :iic_send_byte
 * ��������  :IIC����һ���ֽں���
 * ��������  :uint8_t data
 * ��������ֵ:��
 * ��������  :
 */
void bs8116_iic_send_byte(uint8_t data)
{
	uint8_t i;
	for(i = 0;i < 8;i++)
	{
		//ʱ�������Ͳſ��Ըı�������
		BS8116_IIC_SCL_L;
		tim5_delay_us(5);  
		//���ݲ����Ķ�Ӧλ��0����1���������ߵĸߵ�
		if(data & 0x80)
		{
			BS8116_IIC_SDA_OUT_H;
		}
		else
		{
			BS8116_IIC_SDA_OUT_L;
		}
		tim5_delay_us(5);  
		//ʱ�������ߣ������ӻ��������ӻ��ſ��Զ���һλ����
		BS8116_IIC_SCL_H;
		tim5_delay_us(5);  
		//��һλ���ݣ��θ�λ������λ
		data = data << 1;
	}
	//��ȫ
	//BS8116_IIC_SCL_L;
	
}




/**
 * ������    :iic_read_byte
 * ��������  :����ͨ��IIC����һ���ֽں���
 * ��������  :��
 * ��������ֵ:uint8_t
 * ��������  :
 */
uint8_t bs8116_iic_read_byte(void)
{
	
	uint8_t data;    
	uint8_t i;
	
	//��IO���л�Ϊ����
	//BS8116_IIC_SCL_L;
	BS8116_IIC_SDA_OUT_H;
	
	//������
	for(i = 0;i < 8;i++)
	{
		BS8116_IIC_SCL_L;             //���������ӻ�����ʱ����
		tim5_delay_us(5);   
		BS8116_IIC_SCL_H;             //�������Զ�����
		tim5_delay_us(5);
		data = data << 1;
		if(BS8116_IIC_SDA_INT) 
			data= data | 0x01;	
		
	}
	
	//��ȫ
	//BS8116_IIC_SCL_L;
	
	return data;
	
}


/***************************************BS8116************************************/
/**
 * ������    :bs8116_init
 * ��������  :д�Ĵ������������ȡ��͹���
 * ��������  :��
 * ��������ֵ:��
 * ��������  :IRQ-----PA1
 */
void bs8116_init(void)
{
//	uint8_t register_buf[21] = {0};
	
	uint8_t write_buf[21] = {	0x00,0x00,0x83,0xf3,0x98,
								0x10,0x10,0x10,0x10,0x10,
								0x10,0x10,0x10,0x10,0x10,
								0x10,0x10,0x10,0x10,0x10,0x50};	
	
	//IICͨ��IO��ʼ��
	bs8116_iic_io_init();
	
	//BS8116_IRQ--PA1
	RCC->AHB1ENR |= 0x01<<0;			//�˿�ʱ��ʹ��
	GPIOA->MODER &= ~(3<<2);
												
	
	//����21���Ĵ�����ֵ
//	bs8116_read_register(register_buf);
//	for(uint8_t i = 0;i < 21;i++)
//	{
//		printf("%x:0x%x\r\n",0xB0+i,register_buf[i]);
//	}
	
	//д�����üĴ�������ֵ
	bs8116_write_register(write_buf);
	
}



/**
 * ������    :bs8116_read_register
 * ��������  :BS8116���Ĵ�����ֵ
 * ��������  :uint8_t *buf 
 * ��������ֵ:uint8_t 
 * ��������  :������21���Ĵ�����ֵ
 */
uint8_t bs8116_read_register(uint8_t *buf)
{

	bs8116_iic_start();
	bs8116_iic_send_byte(BS8116ADDR_W);  
	if(bs8116_iic_get_ack())
	{
		bs8116_iic_stop( );
		return 1;
	}
	//�ӻ���ʱ�����ͱ�ʾ���ڴ�������
	//������Ҫ�ȴ��ӻ���ʱ��������
	while(!BS8116_IIC_SCL_INT);
	
	bs8116_iic_send_byte(0xB0);     //�ӿ�ʼ��
	if(bs8116_iic_get_ack())
	{
		bs8116_iic_stop( );
		return 2;
	}
	//�ȴ��ӻ���æ
	while(!BS8116_IIC_SCL_INT);
	
	bs8116_iic_start();
	bs8116_iic_send_byte(BS8116ADDR_R);  
	if(bs8116_iic_get_ack())
	{
		bs8116_iic_stop( );
		return 3;
	}
	//�ȴ��ӻ���æ
	while(!BS8116_IIC_SCL_INT);
	
	//��21�����üĴ�������
	for(uint8_t i = 1;i < 22;i++)      
	{
		*buf = bs8116_iic_read_byte();
		if(i == 21) 
			bs8116_iic_send_ack(1);
		else     
			bs8116_iic_send_ack(0);
		buf++;
	}
	bs8116_iic_stop();
	
	return 0;
}



/**
 * ������    :bs8116_write_register
 * ��������  :BS8116д�Ĵ�����ֵ
 * ��������  :uint8_t *cmd_buf
 * ��������ֵ:uint8_t      //ack
 * ��������  :
 */
uint8_t bs8116_write_register(uint8_t *cmd_buf)
{
	uint32_t check_sum = 0;
	
	bs8116_iic_start();
	bs8116_iic_send_byte(BS8116ADDR_W);  
	if(bs8116_iic_get_ack())
	{
		bs8116_iic_stop( );
		return 1;
	}
	//�ȴ��ӻ���æ
	while(!BS8116_IIC_SCL_INT);
	
	bs8116_iic_send_byte(0xB0);    //�Ĵ�������ʼ��ַ 
	if(bs8116_iic_get_ack())
	{
		bs8116_iic_stop( );
		return 2;
	}
	//�ȴ��ӻ���æ
	while(!BS8116_IIC_SCL_INT);
	
	/* �������� */
	//����21���Ĵ������ݺ�һ���ۼӺ�����
	for(uint8_t i = 1;i <= 22;i++)
	{
		//��22�η����ۼӺ�
		if(i == 22)
		{
			bs8116_iic_send_byte(check_sum & 0xff);     //����У��� �Ͱ�λ
		}
		//21֮ǰҪ���ͼĴ������ݲ����ۼ�
		else
		{
			bs8116_iic_send_byte(*cmd_buf);
			check_sum = check_sum + *cmd_buf;
			cmd_buf++;
		}
		
		if(bs8116_iic_get_ack())
		{
			bs8116_iic_stop( );
			return 3;
		}
		
		//�ȴ��ӻ���æ
		while(!BS8116_IIC_SCL_INT);
	}
	
	bs8116_iic_stop( );
	
	return 0;
}





/**
 * ������    :bs8116_read_key
 * ��������  :BS8116����ֵ
 * ��������  :��
 * ��������ֵ:uint8_t
 * ��������  : 0xff������Чֵ
 */
uint8_t bs8116_read_key(void)
{
	uint16_t key = 0xff;
	
	bs8116_iic_start();
	bs8116_iic_send_byte(BS8116ADDR_W);  
	if(bs8116_iic_get_ack())
	{
		bs8116_iic_stop( );
		return 1;
	}
	while(!BS8116_IIC_SCL_INT);//�ȴ��ӻ���æ

	bs8116_iic_send_byte(0x08);    //���ͼ�ֵ�Ĵ����ĵ�ַ
	if(bs8116_iic_get_ack())
	{
		bs8116_iic_stop( );
		return 2;
	}
	while(!BS8116_IIC_SCL_INT);//�ȴ��ӻ���æ
	
	bs8116_iic_start();
	bs8116_iic_send_byte(BS8116ADDR_R);
	if(bs8116_iic_get_ack())
	{
		bs8116_iic_stop( );
		return 3;
	} 
	while(!BS8116_IIC_SCL_INT);//�ȴ��ӻ���æ
	
	key = bs8116_iic_read_byte();
	bs8116_iic_send_ack(0);        //������
	key = bs8116_iic_read_byte() << 8 | key;
	bs8116_iic_send_ack(1);//����Ҫ������
	bs8116_iic_stop( );
	//keyֵ���Ǽ�ֵ�Ĵ�������������
	
	//printf("key:0x%x\r\n",key);
	switch(key)
	{
		case 0X0001:return   '1';break;
		case 0X0400:return   '2';break;
		case 0X0080:return   '3';break;     
		case 0X0002:return   '4';break;
		case 0x0800:return   '5';break;
		case 0X0040:return   '6';break;
		case 0X0008:return   '7';break;
		case 0X0100:return   '8';break;
		case 0X0020:return   '9';break;
		case 0X0004:return   '*';break;
		case 0X0200:return   '0';break;
		case 0X0010:return   '#';break;
	}
	
	return 0xff;
}




/**
 * ������    :bs8116_key_scan
 * ��������  :BS8116����ɨ��
 * ��������  :��
 * ��������ֵ:uint8_t
 * ��������  :0xff������Чֵ
 */
uint8_t bs8116_key_scan(void)
{
	static uint8_t bs8116_key_flag = 1;
	uint8_t key = 0xff;
	
	if(!BS8116_IRQ  && bs8116_key_flag)
	{
		key = bs8116_read_key(); 
		//��ⰴ����ȡֵ�Ƿ�Ϊ��Чֵ,��Чֵ�Ͳ�������־λ,��Чֵ��������־λ
		if(key != 0xff)
		{
			bs8116_key_flag = 0;
		}
	}
	
	if(BS8116_IRQ)
	{
		bs8116_key_flag = 1;
	}
	
	return key;
}







/**
 * ������    :open_passwd
 * ��������  :���뿪��
 * ��������  :���µİ���
 * ��������ֵ:uint8_t
 * ��������  :AT24C04��100λ�ô洢open_flag,101��ʼ�洢open_passwd1��open_passwd2
 */
void open_passwd(uint8_t bs8116_key)
{
	static uint8_t at24_flag = 1;
	static uint8_t open_flag = 0;                   	   //��һ�ο�����־
	static uint8_t open_passwd1[10] = {0};                 
	static uint8_t open_passwd2[10] = {0};                 
	static uint8_t passwd_i = 0;                    	   
	static uint8_t open_door_flag = 1;
		
	if(at24_flag)											//������ѭ����ִֻ��һ��
	{
		//��AT24C04 0�ŵ�ַ �Ƿ�Ϊ 0xff
		at24c04_read_byte(0x00,&open_flag);
		if(open_flag != 0xff)   
			voice(0x02);        		 //���ÿ�������
		at24_flag = 0;
	}
	
	//��һ�ο�������������
	if(open_flag != 0xff)
	{
		lcd_flash_show_32(0,40,(uint8_t *)"�����ÿ�������",RED,WHITE);
		lcd_flash_show_32(0,75,(uint8_t *)"��һ����������",RED,WHITE);
		if(bs8116_key != 0xff)
		{
			//��һ����������
			if(passwd_i <= 5)
			{	
				open_passwd1[passwd_i] = bs8116_key;
				lcd_flash_show_32(passwd_i*32,110,(uint8_t *)"*",RED,WHITE);
				passwd_i++;
				if(passwd_i == 6)
				{
					lcd_flash_show_32(0,145,(uint8_t *)"�ڶ�����������",RED,WHITE);
					voice(0x0e);			//���ٴ�����������
				}
			}
			//�ڶ�����������
			else if(passwd_i >= 6 && passwd_i <= 11)
			{
				open_passwd2[passwd_i - 6] = bs8116_key;
				lcd_flash_show_32((passwd_i - 6)*32,180,(uint8_t *)"*",RED,WHITE);
				passwd_i++;
			}
			//��������������ɽ��жԱ�
			if(passwd_i == 12)
			{
				if(strcmp((char *)open_passwd1,(char *)open_passwd2) == 0)
				{
					//������洢��AT24C04��0x01��ַ
					at24c04_write_bytes(0x01,sizeof(open_passwd1),open_passwd1);
					//�ѿ�����־0xff�洢��AT24C04��0x00��ַ
					at24c04_write_byte(0x00,0xff);
					lcd_clear(0x0000);
					lcd_flash_show_32(0,0,(uint8_t *)"�������óɹ�",RED,WHITE);	
					voice(0x11);
					at24_flag = 1;                         //�ٴζ�AT24C04�Ŀ�����־λ
				}
				else
				{
					lcd_clear(0x0000);
					lcd_flash_show_32(0,0,(uint8_t *)"�������벻һ��",RED,WHITE);
					voice(0x10);
					passwd_i = 0;
				}
			}
			
		}
	}
	
	//���ǵ�һ�ο�����ʶ�����뿪��
	else
	{
		if(open_door_flag)
		{
			lcd_clear(0x0000);
			lcd_flash_show_32(0,0,(uint8_t *)"����������",RED,WHITE);	
			open_door_flag = 0;
		}
		
		if(bs8116_key != 0xff)
		{
			if(passwd_i <= 5)
			{	
				open_passwd1[passwd_i] = bs8116_key;
				lcd_flash_show_32(passwd_i*32,110,(uint8_t *)"*",RED,WHITE);
				passwd_i++;
				if(passwd_i == 6)
				{
					//����AT24C02ԭʼ����
					at24c04_read_bytes(0x01,sizeof(open_passwd1),open_passwd2);
					//�Ա�
					if(strcmp((char *)open_passwd1,(char *)open_passwd2) == 0)
					{
						lcd_clear(0x0000);
						lcd_flash_show_32(50,100,(uint8_t *)"��ӭ�ؼ�",RED,WHITE);
						//����
						open_door();
						tim5_delay_ms(300);
						close_door();
						lcd_clear(0x0000);
						passwd_i = 0;
						open_door_flag = 1;
						voice(0X18);               //��ӭ�ؼ�
					}
					else
					{
						lcd_clear(0x0000);
						lcd_flash_show_32(50,100,(uint8_t *)"�������",RED,WHITE);	
						passwd_i = 0;
						open_door_flag = 1;
						voice(0X19);               //����ʧ��
					}	
				}
			}
		}	
	}
}






