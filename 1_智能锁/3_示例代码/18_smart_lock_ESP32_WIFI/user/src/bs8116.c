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
*函数名    :bs8116_iic_IO_init
*函数功能  :IIC所用IO口初始化配置
*函数参数  :无
*函数返回值:无
*函数描述  : PB6----SCL   通用开漏输出
            PB7----SDA   通用开漏输出
************************************************/
void bs8116_iic_IO_init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStruct;
	
	//端口时钟使能
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;          //通用输出
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;         //开漏
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 ;  // 6    7
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;       //无上下拉
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;       //速度
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	
	//空闲状态
	GPIO_SetBits(GPIOB, GPIO_Pin_6);//SCL
	GPIO_SetBits(GPIOB, GPIO_Pin_7);//SDA
	
	
}




/***********************************************
*函数名    :iic_star
*函数功能  :IIC起始信号函数
*函数参数  :无
*函数返回值:无
*函数描述  :时钟线为高电平，数据线出现下降沿
************************************************/

void bs8116_iic_star(void)
{
	//时钟线拉低，才可以动数据线
	BS8116_IIC_SCL_L; 
	Tim5_delay_us(5);
	//数据线拉高，目的是为出现下降沿做准备
	BS8116_IIC_SDA_OUT_H;   
	//时钟线拉高  产生起始信号的条件
	BS8116_IIC_SCL_H;
	Tim5_delay_us(5);
	//数据线拉低  可以产生起始信号
	BS8116_IIC_SDA_OUT_L;
	Tim5_delay_us(5);
	
	//安全
	//BS8116_IIC_SCL_L;
	
}



/***********************************************
*函数名    :iic_stop
*函数功能  :IIC停止信号函数
*函数参数  :无
*函数返回值:无
*函数描述  :时钟线为高电平，数据线出现上升沿
************************************************/
void bs8116_iic_stop(void)
{
	
	//准备工作
	BS8116_IIC_SCL_L;
	Tim5_delay_us(5);
	BS8116_IIC_SDA_OUT_L;
	
	//产生停止信号
	BS8116_IIC_SCL_H;
	Tim5_delay_us(5);
	BS8116_IIC_SDA_OUT_H;
	Tim5_delay_us(5);

}



/***********************************************
*函数名    :iic_send_ack
*函数功能  :发送应答/不应答信号
*函数参数  :u8 ack
*函数返回值:无
*函数描述  :应答： 时钟线为高电平，数据线已经保持好低电平了
						不应答 时钟线为高电平，数据线已经保持好高电平了				
						参数：0   发送应答信号
						参数：1   发送不应答信号
************************************************/
void bs8116_iic_send_ack(u8 ack)
{
		BS8116_IIC_SCL_L;      //时钟线拉低，才可以动数据线
		Tim5_delay_us(5);
		if(ack == 0)       //发送应答
		{
			BS8116_IIC_SDA_OUT_L;  //让数据线先保持低电平
		}
		else if(ack == 1)
		{
			BS8116_IIC_SDA_OUT_H;  //让数据线先保持高电平
		}
		Tim5_delay_us(5);
		BS8116_IIC_SCL_H;      //产生了应答信号	
		Tim5_delay_us(5);
	
	
	//安全
	//IIC_SCL_L;
	
}



/***********************************************
*函数名    :iic_get_ack
*函数功能  :检测应答/不应答信号
*函数参数  :无
*函数返回值:u8
*函数描述  :
						返回0 表示应答信号
						返回1 表示不应答信号
************************************************/
u8 bs8116_iic_get_ack(void)
{
	
	u8 ack = 0;
	
	//数据线要为输入
	//BS8116_IIC_SCL_L;
	//数据线切换输入状态
	BS8116_IIC_SDA_OUT_H;    
	
	//检测应答/不应答信号
	BS8116_IIC_SCL_L;   //主机帮助从机拉低时钟线，从机才可以动数据线，来表现应答/不应答信号
	Tim5_delay_us(5);  
	BS8116_IIC_SCL_H;   //主机才可以读数据线
	Tim5_delay_us(5);
	if(BS8116_IIC_SDA_INT == 0)
	{
		//低电平——应答信号
		ack = 0;
	}
	else
	{
		//高电平----不应答信号
		ack = 1;
	}
	
	//安全
	//BS8116_IIC_SCL_L;
	return ack;
	
	
	
}



/***********************************************
*函数名    :iic_send_byte
*函数功能  :IIC发送一个字节函数
*函数参数  :u8 data
*函数返回值:无
*函数描述  :
************************************************/
void bs8116_iic_send_byte(u8 data)
{
	u8 i;
	for(i=0;i<8;i++)
	{
		//时钟线拉低才可以改变数据线
		BS8116_IIC_SCL_L;
		Tim5_delay_us(5);  
		//根据参数的对应位是0还是1决定数据线的高低
		if(data & 0x80)
		{
			BS8116_IIC_SDA_OUT_H;
		}
		else
		{
			BS8116_IIC_SDA_OUT_L;
		}
		Tim5_delay_us(5);  
		//时钟下拉高，帮助从机，这样从机才可以读这一位数据
		BS8116_IIC_SCL_H;
		Tim5_delay_us(5);  
		//下一位数据，次高位变成最高位
		data = data << 1;
	}
	//安全
	//BS8116_IIC_SCL_L;
	
}




/*******************************************
*函数名    :iic_read_byte
*函数功能  :主机通过IIC接收一个字节函数
*函数参数  :无
*函数返回值:u8
*函数描述  :
*********************************************/
u8 bs8116_iic_read_byte(void)
{
	
	u8 data;    
	u8 i;
	
	//将IO口切换为输入
	//BS8116_IIC_SCL_L;
	BS8116_IIC_SDA_OUT_H;
	
	//读数据
	for(i=0;i<8;i++)
	{
		BS8116_IIC_SCL_L;             //主机帮助从机拉低时钟线
		Tim5_delay_us(5);   
		BS8116_IIC_SCL_H;             //主机可以读数据
		Tim5_delay_us(5);
		data = data << 1;
		if(BS8116_IIC_SDA_INT) 
			data= data | 0x01;	
		
	}
	
	//安全
	//BS8116_IIC_SCL_L;
	
	return data;
	
}





















/***********************************************
*函数名    :BS8116_init
*函数功能  :BS8116初始化配置
*函数参数  :无
*函数返回值:无
*函数描述  :IRQ-----PA1
************************************************/
void BS8116_init(void)
{
	u8 register_buff[21] = {0};
	
	u8 write_buff[21] = {	0x00,0x00,0x83,0xf3,0x98,
												0x10,0x10,0x10,0x10,0x10,
												0x10,0x10,0x10,0x10,0x10,
												0x10,0x10,0x10,0x10,0x10,0x50};	
	
	//IIC通信IO初始化
	bs8116_iic_IO_init();
	//BS8116_IRQ-PA1
	RCC->AHB1ENR |=0x01<<0;//端口时钟使能
	GPIOA->MODER &= ~(3<<2);
												
	
	//读出21个寄存器的值
//	BS8116_Readregister(register_buff);
//	for(u8 i=0;i<21;i++)
//	{
//		printf("%x:0x%x\r\n",0xB0+i,register_buff[i]);
//	}
	
	//写入配置寄存器设置值
	BS8116_Writeregister(write_buff);
	
}



/***********************************************
*函数名    :BS8116_Readregister
*函数功能  :BS8116读寄存器的值
*函数参数  :u8 *buff 
*函数返回值:u8 
*函数描述  :读到的21个寄存器的值
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
	//从机把时钟拉低表示正在处理数据
	//主机需要等待从机把时钟线拉高
	while(!BS8116_IIC_SCL_INT);
	
	bs8116_iic_send_byte(0xB0);     //从开始读
	if(bs8116_iic_get_ack())
	{
		bs8116_iic_stop( );
		return 2;
	}
	//等待从机不忙
	while(!BS8116_IIC_SCL_INT);
	
	bs8116_iic_star();
	bs8116_iic_send_byte(BS8116ADDR_R);  
	if(bs8116_iic_get_ack())
	{
		bs8116_iic_stop( );
		return 3;
	}
	//等待从机不忙
	while(!BS8116_IIC_SCL_INT);
	
	//读21个配置寄存器数据
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
*函数名    :BS8116_Writeregister
*函数功能  :BS8116写寄存器的值
*函数参数  :u8 *cmd_buff
*函数返回值:u8      //ack
*函数描述  :
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
	//等待从机不忙
	while(!BS8116_IIC_SCL_INT);
	
	bs8116_iic_send_byte(0xB0);    //寄存器的起始地址 
	if(bs8116_iic_get_ack())
	{
		bs8116_iic_stop( );
		return 2;
	}
	//等待从机不忙
	while(!BS8116_IIC_SCL_INT);
	
	/*发送数据*/
	//发送21个寄存器数据和一个累加和数据
	for(u8 i=1;i<=22;i++)
	{
		//第22次发送累加和
		if(i==22)
		{
			bs8116_iic_send_byte(check_sum & 0xff);     //发送校验和 低八位
		}
		//21之前要发送寄存器数据并且累加
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
		
		//等待从机不忙
		while(!BS8116_IIC_SCL_INT);
	}
	
	bs8116_iic_stop( );
	
	return 0;
}





/***********************************************
*函数名    :BS8116_ReadKey
*函数功能  :BS8116读键值
*函数参数  :无
*函数返回值:u8
*函数描述  :
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
	//等待从机不忙
	while(!BS8116_IIC_SCL_INT);

	bs8116_iic_send_byte(0x08);    //发送键值寄存器的地址
	if(bs8116_iic_get_ack())
	{
		bs8116_iic_stop( );
		return 2;
	}
	//等待从机不忙
	while(!BS8116_IIC_SCL_INT);
	
	
	bs8116_iic_star();
	bs8116_iic_send_byte(BS8116ADDR_R);
	if(bs8116_iic_get_ack())
	{
		bs8116_iic_stop( );
		return 3;
	}  
	//等待从机不忙
	while(!BS8116_IIC_SCL_INT);
	
	key=bs8116_iic_read_byte();
	bs8116_iic_send_ack(0);        //继续读
	key = bs8116_iic_read_byte()<<8 | key;
	bs8116_iic_send_ack(1);        //不需要数据了
	bs8116_iic_stop( );
	//key值就是键值寄存器的总体数据
	
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
*函数名    :BS8116_Key_scan
*函数功能  :BS8116按键扫描
*函数参数  :无
*函数返回值:u8
*函数描述  :
************************************************/
u8 BS8116_Key_scan(void)
{
	static u8 bs8116_key_flag = 1;
	u8 key = 0xff;
	
	if(!BS8116_IRQ  && bs8116_key_flag)
	{
		key = BS8116_ReadKey(); 
		//检测按键获取值是否为有效值,无效值就不锁定标志位,有效值才锁定标志位
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
*函数名    :open_passward
*函数功能  :密码开锁
*函数参数  :u8 bs8116_key
*函数返回值:u8
*函数描述  :
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
		//读AT24C04 100号地址 是否为 0xfe
		AT24C04_read_byte(100,&open_flag);
		if(open_flag != 0xfe)   voice(0x02);
		at24_flag = 0;
	}
	
	
	//如果是第一次开机 设置密码
	if(open_flag != 0xfe)
	{
		LCD_flash_dis_32(0,40,RED,RED,"请设置开锁密码",0);
		LCD_flash_dis_32(0,75,RED,RED,"输入第一次密码",0);
		if(bs8116_key != 0xff)
		{
			//第一次输入密码
			if(passward_i<=5)
			{	
				open_passward1[passward_i] = bs8116_key;
				LCD_flash_dis_32(passward_i*32,110,RED,RED,"*",0);
				passward_i++;
				if(passward_i == 6)
				{
					LCD_flash_dis_32(0,145,RED,RED,"输入第二次密码",0);
					voice(0x0e);
				}
			}
			//第二次输入密码
			else if(passward_i>=6 && passward_i<=11)
			{
				open_passward2[passward_i-6] = bs8116_key;
				LCD_flash_dis_32((passward_i-6)*32,180,RED,RED,"*",0);
				passward_i++;
			}
			//两次密码输入完成进行对比
			if(passward_i == 12)
			{
				if(strcmp((char *)open_passward1,(char *)open_passward2) == 0)
				{
					//把密码存储到AT24C04
					AT24C04_write_bytes(101,sizeof(open_passward1),open_passward1);
					//开机标志位
					AT24C04_write_byte(100,0xfe);
					LCD_clear(0x0000);
					LCD_flash_dis_32(0,0,RED,RED,"密码设置成功",0);	
					voice(0x11);
					at24_flag = 1;
					
					
				}
				else
				{
					LCD_clear(0x0000);
					LCD_flash_dis_32(0,0,RED,RED,"两次密码不一致",0);
					voice(0x10);
					passward_i = 0;
				}
			}
			
		}
	}
	
	//不是第一次开机  识别密码开门
	else
	{
		if(open_door_flag)
		{
			LCD_flash_dis_32(0,0,RED,RED,"请输入密码",0);	
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
					//读出AT24C02原始密码
					AT24C04_read_bytes(101,sizeof(open_passward1),open_passward2);
					//对比
					if(strcmp((char *)open_passward1,(char *)open_passward2) == 0)
					{
						LCD_clear(0x0000);
						LCD_flash_dis_32(50,100,RED,RED,"欢迎回家",0);
						//开门
						open_door();
						Tim5_delay_ms(300);
						close_door();
						LCD_clear(0x0000);
						passward_i = 0;
						open_door_flag = 1;
						voice(0X18);
						
						//进入子界面标志
						page_mode = 2;
					}
					else
					{
						LCD_clear(0x0000);
						LCD_flash_dis_32(50,100,RED,RED,"密码错误",0);	
						passward_i = 0;
						open_door_flag = 1;
						voice(0X19);
					}	
				}
			}
			
		}
		
		
		
		
		
	}
	
	
	
	
}



























































