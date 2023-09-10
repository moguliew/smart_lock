#include "stm32f4xx.h"                  // Device header
#include "bs8116.h"
#include "timer.h"
#include "stdio.h"
#include "voice.h"
#include "at24c04.h"
#include "lcd.h"
#include "string.h"
#include "door.h"
#include "mg200.h"
/***********************************************
*������    :bs8116_iic_IO_init
*��������  :IIC����IO�ڳ�ʼ������
*��������  :��
*��������ֵ:��
*��������  : PB6----SCL   ͨ�ÿ�©���
            PB7----SDA   ͨ�ÿ�©���
************************************************/
void bs8116_iic_IO_init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStruct;
	
	//�˿�ʱ��ʹ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;          //ͨ�����
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;         //��©
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 ;  // 6    7
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;       //��������
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;       //�ٶ�
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	
	//����״̬
	GPIO_SetBits(GPIOB, GPIO_Pin_6);//SCL
	GPIO_SetBits(GPIOB, GPIO_Pin_7);//SDA
	
	
}




/***********************************************
*������    :iic_star
*��������  :IIC��ʼ�źź���
*��������  :��
*��������ֵ:��
*��������  :ʱ����Ϊ�ߵ�ƽ�������߳����½���
************************************************/

void bs8116_iic_star(void)
{
	//ʱ�������ͣ��ſ��Զ�������
	BS8116_IIC_SCL_L; 
	Tim5_delay_us(5);
	//���������ߣ�Ŀ����Ϊ�����½�����׼��
	BS8116_IIC_SDA_OUT_H;   
	//ʱ��������  ������ʼ�źŵ�����
	BS8116_IIC_SCL_H;
	Tim5_delay_us(5);
	//����������  ���Բ�����ʼ�ź�
	BS8116_IIC_SDA_OUT_L;
	Tim5_delay_us(5);
	
	//��ȫ
	//BS8116_IIC_SCL_L;
	
}



/***********************************************
*������    :iic_stop
*��������  :IICֹͣ�źź���
*��������  :��
*��������ֵ:��
*��������  :ʱ����Ϊ�ߵ�ƽ�������߳���������
************************************************/
void bs8116_iic_stop(void)
{
	
	//׼������
	BS8116_IIC_SCL_L;
	Tim5_delay_us(5);
	BS8116_IIC_SDA_OUT_L;
	
	//����ֹͣ�ź�
	BS8116_IIC_SCL_H;
	Tim5_delay_us(5);
	BS8116_IIC_SDA_OUT_H;
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
void bs8116_iic_send_ack(u8 ack)
{
		BS8116_IIC_SCL_L;      //ʱ�������ͣ��ſ��Զ�������
		Tim5_delay_us(5);
		if(ack == 0)       //����Ӧ��
		{
			BS8116_IIC_SDA_OUT_L;  //���������ȱ��ֵ͵�ƽ
		}
		else if(ack == 1)
		{
			BS8116_IIC_SDA_OUT_H;  //���������ȱ��ָߵ�ƽ
		}
		Tim5_delay_us(5);
		BS8116_IIC_SCL_H;      //������Ӧ���ź�	
		Tim5_delay_us(5);
	
	
	//��ȫ
	//IIC_SCL_L;
	
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
u8 bs8116_iic_get_ack(void)
{
	
	u8 ack = 0;
	
	//������ҪΪ����
	//BS8116_IIC_SCL_L;
	//�������л�����״̬
	BS8116_IIC_SDA_OUT_H;    
	
	//���Ӧ��/��Ӧ���ź�
	BS8116_IIC_SCL_L;   //���������ӻ�����ʱ���ߣ��ӻ��ſ��Զ������ߣ�������Ӧ��/��Ӧ���ź�
	Tim5_delay_us(5);  
	BS8116_IIC_SCL_H;   //�����ſ��Զ�������
	Tim5_delay_us(5);
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



/***********************************************
*������    :iic_send_byte
*��������  :IIC����һ���ֽں���
*��������  :u8 data
*��������ֵ:��
*��������  :
************************************************/
void bs8116_iic_send_byte(u8 data)
{
	u8 i;
	for(i=0;i<8;i++)
	{
		//ʱ�������Ͳſ��Ըı�������
		BS8116_IIC_SCL_L;
		Tim5_delay_us(5);  
		//���ݲ����Ķ�Ӧλ��0����1���������ߵĸߵ�
		if(data & 0x80)
		{
			BS8116_IIC_SDA_OUT_H;
		}
		else
		{
			BS8116_IIC_SDA_OUT_L;
		}
		Tim5_delay_us(5);  
		//ʱ�������ߣ������ӻ��������ӻ��ſ��Զ���һλ����
		BS8116_IIC_SCL_H;
		Tim5_delay_us(5);  
		//��һλ���ݣ��θ�λ������λ
		data = data << 1;
	}
	//��ȫ
	//BS8116_IIC_SCL_L;
	
}




/*******************************************
*������    :iic_read_byte
*��������  :����ͨ��IIC����һ���ֽں���
*��������  :��
*��������ֵ:u8
*��������  :
*********************************************/
u8 bs8116_iic_read_byte(void)
{
	
	u8 data;    
	u8 i;
	
	//��IO���л�Ϊ����
	//BS8116_IIC_SCL_L;
	BS8116_IIC_SDA_OUT_H;
	
	//������
	for(i=0;i<8;i++)
	{
		BS8116_IIC_SCL_L;             //���������ӻ�����ʱ����
		Tim5_delay_us(5);   
		BS8116_IIC_SCL_H;             //�������Զ�����
		Tim5_delay_us(5);
		data = data << 1;
		if(BS8116_IIC_SDA_INT) 
			data= data | 0x01;	
		
	}
	
	//��ȫ
	//BS8116_IIC_SCL_L;
	
	return data;
	
}





















/***********************************************
*������    :BS8116_init
*��������  :BS8116��ʼ������
*��������  :��
*��������ֵ:��
*��������  :IRQ-----PA1
************************************************/
void BS8116_init(void)
{
	u8 register_buff[21] = {0};
	
	u8 write_buff[21] = {	0x00,0x00,0x83,0xf3,0x98,
												0x10,0x10,0x10,0x10,0x10,
												0x10,0x10,0x10,0x10,0x10,
												0x10,0x10,0x10,0x10,0x10,0x50};	
	
	//IICͨ��IO��ʼ��
	bs8116_iic_IO_init();
	//BS8116_IRQ-PA1
	RCC->AHB1ENR |=0x01<<0;//�˿�ʱ��ʹ��
	GPIOA->MODER &= ~(3<<2);
												
	
	//����21���Ĵ�����ֵ
//	BS8116_Readregister(register_buff);
//	for(u8 i=0;i<21;i++)
//	{
//		printf("%x:0x%x\r\n",0xB0+i,register_buff[i]);
//	}
	
	//д�����üĴ�������ֵ
	BS8116_Writeregister(write_buff);
	
}



/***********************************************
*������    :BS8116_Readregister
*��������  :BS8116���Ĵ�����ֵ
*��������  :u8 *buff 
*��������ֵ:u8 
*��������  :������21���Ĵ�����ֵ
************************************************/
u8 BS8116_Readregister(u8 *buff)
{

	bs8116_iic_star();
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
	
	bs8116_iic_star();
	bs8116_iic_send_byte(BS8116ADDR_R);  
	if(bs8116_iic_get_ack())
	{
		bs8116_iic_stop( );
		return 3;
	}
	//�ȴ��ӻ���æ
	while(!BS8116_IIC_SCL_INT);
	
	//��21�����üĴ�������
	for(u8 i=1;i<22;i++)      
	{
		*buff = bs8116_iic_read_byte();
		if(i==21) bs8116_iic_send_ack(1);
		else     bs8116_iic_send_ack(0);
		buff++;
	}
	bs8116_iic_stop( );
	
	return 0;
}



/***********************************************
*������    :BS8116_Writeregister
*��������  :BS8116д�Ĵ�����ֵ
*��������  :u8 *cmd_buff
*��������ֵ:u8      //ack
*��������  :
************************************************/
u8 BS8116_Writeregister(u8 *cmd_buff)
{
	u32 check_sum = 0;
	
	bs8116_iic_star();
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
	
	/*��������*/
	//����21���Ĵ������ݺ�һ���ۼӺ�����
	for(u8 i=1;i<=22;i++)
	{
		//��22�η����ۼӺ�
		if(i==22)
		{
			bs8116_iic_send_byte(check_sum & 0xff);     //����У��� �Ͱ�λ
		}
		//21֮ǰҪ���ͼĴ������ݲ����ۼ�
		else
		{
			bs8116_iic_send_byte(*cmd_buff);
			check_sum = check_sum + *cmd_buff;
			cmd_buff++;
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





/***********************************************
*������    :BS8116_ReadKey
*��������  :BS8116����ֵ
*��������  :��
*��������ֵ:u8
*��������  :
************************************************/
u8 BS8116_ReadKey(void)
{
	u16 key = 0xff;
	
	
	bs8116_iic_star();
	bs8116_iic_send_byte(BS8116ADDR_W);  
	if(bs8116_iic_get_ack())
	{
		bs8116_iic_stop( );
		return 1;
	}
	//�ȴ��ӻ���æ
	while(!BS8116_IIC_SCL_INT);

	bs8116_iic_send_byte(0x08);    //���ͼ�ֵ�Ĵ����ĵ�ַ
	if(bs8116_iic_get_ack())
	{
		bs8116_iic_stop( );
		return 2;
	}
	//�ȴ��ӻ���æ
	while(!BS8116_IIC_SCL_INT);
	
	
	bs8116_iic_star();
	bs8116_iic_send_byte(BS8116ADDR_R);
	if(bs8116_iic_get_ack())
	{
		bs8116_iic_stop( );
		return 3;
	}  
	//�ȴ��ӻ���æ
	while(!BS8116_IIC_SCL_INT);
	
	key=bs8116_iic_read_byte();
	bs8116_iic_send_ack(0);        //������
	key = bs8116_iic_read_byte()<<8 | key;
	bs8116_iic_send_ack(1);        //����Ҫ������
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




/***********************************************
*������    :BS8116_Key_scan
*��������  :BS8116����ɨ��
*��������  :��
*��������ֵ:u8
*��������  :
************************************************/
u8 BS8116_Key_scan(void)
{
	static u8 bs8116_key_flag = 1;
	u8 key = 0xff;
	
	if(!BS8116_IRQ  && bs8116_key_flag)
	{
		key = BS8116_ReadKey(); 
		//��ⰴ����ȡֵ�Ƿ�Ϊ��Чֵ,��Чֵ�Ͳ�������־λ,��Чֵ��������־λ
		if(key != 0xff)
		{
			voice(0x1d);
			bs8116_key_flag = 0;
		}
	}
	
	if(BS8116_IRQ)
	{
		bs8116_key_flag = 1;
	}
	
	return key;
}





/***********************************************
*������    :open_passward
*��������  :���뿪��
*��������  :u8 bs8116_key
*��������ֵ:u8
*��������  :
************************************************/
void open_passward(u8 bs8116_key)
{
	static u8 at24_flag = 1;
	static u8 open_flag = 0;
	static u8 open_passward1[10] = {0};
	static u8 open_passward2[10] = {0};
	static u8 passward_i = 0;
	static u8 open_door_flag = 1;
	
	
	if(at24_flag)
	{
		//��AT24C04 100�ŵ�ַ �Ƿ�Ϊ 0xfe
		AT24C04_read_byte(100,&open_flag);
		if(open_flag != 0xfe)   voice(0x02);
		at24_flag = 0;
	}
	
	
	//����ǵ�һ�ο��� ��������
	if(open_flag != 0xfe)
	{
		LCD_flash_dis_32(0,40,RED,RED,"�����ÿ�������",0);
		LCD_flash_dis_32(0,75,RED,RED,"�����һ������",0);
		if(bs8116_key != 0xff)
		{
			//��һ����������
			if(passward_i<=5)
			{	
				open_passward1[passward_i] = bs8116_key;
				LCD_flash_dis_32(passward_i*32,110,RED,RED,"*",0);
				passward_i++;
				if(passward_i == 6)
				{
					LCD_flash_dis_32(0,145,RED,RED,"����ڶ�������",0);
					voice(0x0e);
				}
			}
			//�ڶ�����������
			else if(passward_i>=6 && passward_i<=11)
			{
				open_passward2[passward_i-6] = bs8116_key;
				LCD_flash_dis_32((passward_i-6)*32,180,RED,RED,"*",0);
				passward_i++;
			}
			//��������������ɽ��жԱ�
			if(passward_i == 12)
			{
				if(strcmp((char *)open_passward1,(char *)open_passward2) == 0)
				{
					//������洢��AT24C04
					AT24C04_write_bytes(101,sizeof(open_passward1),open_passward1);
					//������־λ
					AT24C04_write_byte(100,0xfe);
					LCD_clear(0x0000);
					LCD_flash_dis_32(0,0,RED,RED,"�������óɹ�",0);	
					voice(0x11);
					at24_flag = 1;
					
					
				}
				else
				{
					LCD_clear(0x0000);
					LCD_flash_dis_32(0,0,RED,RED,"�������벻һ��",0);
					voice(0x10);
					passward_i = 0;
				}
			}
			
		}
	}
	
	//���ǵ�һ�ο���  ʶ�����뿪��
	else
	{
		if(open_door_flag)
		{
			LCD_flash_dis_32(0,0,RED,RED,"����������",0);	
			voice(0X0c);
			open_door_flag = 0;
		}
		
		if(bs8116_key != 0xff)
		{
			if(passward_i<=5)
			{	
				open_passward1[passward_i] = bs8116_key;
				LCD_flash_dis_32(passward_i*32,110,RED,RED,"*",0);
				passward_i++;
				if(passward_i == 6)
				{
					//����AT24C02ԭʼ����
					AT24C04_read_bytes(101,sizeof(open_passward1),open_passward2);
					//�Ա�
					if(strcmp((char *)open_passward1,(char *)open_passward2) == 0)
					{
						LCD_clear(0x0000);
						LCD_flash_dis_32(50,100,RED,RED,"��ӭ�ؼ�",0);
						//����
						open_door();
						Tim5_delay_ms(300);
						close_door();
						LCD_clear(0x0000);
						passward_i = 0;
						open_door_flag = 1;
						voice(0X18);
						
						//�����ӽ����־
						page_mode = 2;
					}
					else
					{
						LCD_clear(0x0000);
						LCD_flash_dis_32(50,100,RED,RED,"�������",0);	
						passward_i = 0;
						open_door_flag = 1;
						voice(0X19);
					}	
				}
			}
			
		}
		
		
		
		
		
	}
	
	
	
	
}



























































