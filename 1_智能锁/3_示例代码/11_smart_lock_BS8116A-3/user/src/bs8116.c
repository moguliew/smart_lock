#include "stm32f4xx.h"                  // Device header
#include "bs8116.h"
#include "tim.h"
#include "stdio.h"
/********************************************IIC************************************/
/**
 * 函数名    :bs8116_iic_io_init
 * 函数功能  :IIC所用IO口初始化配置
 * 函数参数  :无
 * 函数返回值:无
 * 函数描述  : PB6----SCL   通用开漏输出
 *             PB7----SDA   通用开漏输出
 */
void bs8116_iic_io_init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStruct;
	
	//端口时钟使能
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;          	//通用输出
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;         	//开漏
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 ;  	//6、7
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;       	//无上下拉
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;       	//速度
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	
	//空闲状态
	GPIO_SetBits(GPIOB, GPIO_Pin_6);//SCL
	GPIO_SetBits(GPIOB, GPIO_Pin_7);//SDA
	
}



/**
 * 函数名    :iic_start
 * 函数功能  :IIC起始信号函数
 * 函数参数  :无
 * 函数返回值:无
 * 函数描述  :时钟线为高电平，数据线出现下降沿
 */
void bs8116_iic_start(void)
{
	//时钟线拉低，才可以动数据线
	BS8116_IIC_SCL_L; 
	tim5_delay_us(5);
	//数据线拉高，目的是为出现下降沿做准备
	BS8116_IIC_SDA_OUT_H;   
	//时钟线拉高  产生起始信号的条件
	BS8116_IIC_SCL_H;
	tim5_delay_us(5);
	//数据线拉低  可以产生起始信号
	BS8116_IIC_SDA_OUT_L;
	tim5_delay_us(5);
	
	//安全
	//BS8116_IIC_SCL_L;
	
}



/**
 * 函数名    :iic_stop
 * 函数功能  :IIC停止信号函数
 * 函数参数  :无
 * 函数返回值:无
 * 函数描述  :时钟线为高电平，数据线出现上升沿
 */
void bs8116_iic_stop(void)
{
	
	//准备工作
	BS8116_IIC_SCL_L;
	tim5_delay_us(5);
	BS8116_IIC_SDA_OUT_L;
	
	//产生停止信号
	BS8116_IIC_SCL_H;
	tim5_delay_us(5);
	BS8116_IIC_SDA_OUT_H;
	tim5_delay_us(5);

}



/**
 * 函数名    :iic_send_ack
 * 函数功能  :发送应答/不应答信号
 * 函数参数  :0--发送应答信号,1--发送不应答信号
 * 函数返回值:无
 * 函数描述  :应答： 时钟线为高电平，数据线已经保持好低电平了
 *					不应答 时钟线为高电平，数据线已经保持好高电平了				
 */
void bs8116_iic_send_ack(uint8_t ack)
{
	BS8116_IIC_SCL_L;	  		//时钟线拉低，才可以动数据线
	tim5_delay_us(5);
	if(ack == 0)	   			//发送应答
	{
		BS8116_IIC_SDA_OUT_L;   //让数据线先保持低电平
	}
	else if(ack == 1)
	{
		BS8116_IIC_SDA_OUT_H;   //让数据线先保持高电平
	}
	tim5_delay_us(5);
	BS8116_IIC_SCL_H;	  	    //产生了应答信号	
	tim5_delay_us(5);

	//安全
	//IIC_SCL_L;	
}



/**
 * 函数名    :iic_get_ack
 * 函数功能  :检测应答/不应答信号
 * 函数参数  :无
 * 函数返回值:返回0--表示应答信号,返回1--表示不应答信号
 * 函数描述  :			
 */
uint8_t bs8116_iic_get_ack(void)
{	
	uint8_t ack = 0;
	
	//数据线要为输入
	//BS8116_IIC_SCL_L;
	//数据线切换输入状态
	BS8116_IIC_SDA_OUT_H;    
	
	//检测应答/不应答信号
	BS8116_IIC_SCL_L;   //主机帮助从机拉低时钟线，从机才可以动数据线，来表现应答/不应答信号
	tim5_delay_us(5);  
	BS8116_IIC_SCL_H;   //主机才可以读数据线
	tim5_delay_us(5);
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



/**
 * 函数名    :iic_send_byte
 * 函数功能  :IIC发送一个字节函数
 * 函数参数  :uint8_t data
 * 函数返回值:无
 * 函数描述  :
 */
void bs8116_iic_send_byte(uint8_t data)
{
	uint8_t i;
	for(i = 0;i < 8;i++)
	{
		//时钟线拉低才可以改变数据线
		BS8116_IIC_SCL_L;
		tim5_delay_us(5);  
		//根据参数的对应位是0还是1决定数据线的高低
		if(data & 0x80)
		{
			BS8116_IIC_SDA_OUT_H;
		}
		else
		{
			BS8116_IIC_SDA_OUT_L;
		}
		tim5_delay_us(5);  
		//时钟下拉高，帮助从机，这样从机才可以读这一位数据
		BS8116_IIC_SCL_H;
		tim5_delay_us(5);  
		//下一位数据，次高位变成最高位
		data = data << 1;
	}
	//安全
	//BS8116_IIC_SCL_L;
	
}




/**
 * 函数名    :iic_read_byte
 * 函数功能  :主机通过IIC接收一个字节函数
 * 函数参数  :无
 * 函数返回值:uint8_t
 * 函数描述  :
 */
uint8_t bs8116_iic_read_byte(void)
{
	
	uint8_t data;    
	uint8_t i;
	
	//将IO口切换为输入
	//BS8116_IIC_SCL_L;
	BS8116_IIC_SDA_OUT_H;
	
	//读数据
	for(i = 0;i < 8;i++)
	{
		BS8116_IIC_SCL_L;             //主机帮助从机拉低时钟线
		tim5_delay_us(5);   
		BS8116_IIC_SCL_H;             //主机可以读数据
		tim5_delay_us(5);
		data = data << 1;
		if(BS8116_IIC_SDA_INT) 
			data= data | 0x01;	
		
	}
	
	//安全
	//BS8116_IIC_SCL_L;
	
	return data;
	
}


/***************************************BS8116************************************/
/**
 * 函数名    :bs8116_init
 * 函数功能  :写寄存器设置灵敏度、低功耗
 * 函数参数  :无
 * 函数返回值:无
 * 函数描述  :IRQ-----PA1
 */
void bs8116_init(void)
{
//	uint8_t register_buf[21] = {0};
	
	uint8_t write_buf[21] = {	0x00,0x00,0x83,0xf3,0x98,
								0x10,0x10,0x10,0x10,0x10,
								0x10,0x10,0x10,0x10,0x10,
								0x10,0x10,0x10,0x10,0x10,0x50};	
	
	//IIC通信IO初始化
	bs8116_iic_io_init();
	
	//BS8116_IRQ--PA1
	RCC->AHB1ENR |= 0x01<<0;			//端口时钟使能
	GPIOA->MODER &= ~(3<<2);
												
	
	//读出21个寄存器的值
//	bs8116_read_register(register_buf);
//	for(uint8_t i = 0;i < 21;i++)
//	{
//		printf("%x:0x%x\r\n",0xB0+i,register_buf[i]);
//	}
	
	//写入配置寄存器设置值
	bs8116_write_register(write_buf);
	
}



/**
 * 函数名    :bs8116_read_register
 * 函数功能  :BS8116读寄存器的值
 * 函数参数  :uint8_t *buf 
 * 函数返回值:uint8_t 
 * 函数描述  :读到的21个寄存器的值
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
	
	bs8116_iic_start();
	bs8116_iic_send_byte(BS8116ADDR_R);  
	if(bs8116_iic_get_ack())
	{
		bs8116_iic_stop( );
		return 3;
	}
	//等待从机不忙
	while(!BS8116_IIC_SCL_INT);
	
	//读21个配置寄存器数据
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
 * 函数名    :bs8116_write_register
 * 函数功能  :BS8116写寄存器的值
 * 函数参数  :uint8_t *cmd_buf
 * 函数返回值:uint8_t      //ack
 * 函数描述  :
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
	
	/* 发送数据 */
	//发送21个寄存器数据和一个累加和数据
	for(uint8_t i = 1;i <= 22;i++)
	{
		//第22次发送累加和
		if(i == 22)
		{
			bs8116_iic_send_byte(check_sum & 0xff);     //发送校验和 低八位
		}
		//21之前要发送寄存器数据并且累加
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
		
		//等待从机不忙
		while(!BS8116_IIC_SCL_INT);
	}
	
	bs8116_iic_stop( );
	
	return 0;
}





/**
 * 函数名    :bs8116_read_key
 * 函数功能  :BS8116读键值
 * 函数参数  :无
 * 函数返回值:uint8_t
 * 函数描述  :
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
	while(!BS8116_IIC_SCL_INT);//等待从机不忙

	bs8116_iic_send_byte(0x08);    //发送键值寄存器的地址
	if(bs8116_iic_get_ack())
	{
		bs8116_iic_stop( );
		return 2;
	}
	while(!BS8116_IIC_SCL_INT);//等待从机不忙
	
	bs8116_iic_start();
	bs8116_iic_send_byte(BS8116ADDR_R);
	if(bs8116_iic_get_ack())
	{
		bs8116_iic_stop( );
		return 3;
	} 
	while(!BS8116_IIC_SCL_INT);//等待从机不忙
	
	key = bs8116_iic_read_byte();
	bs8116_iic_send_ack(0);        //继续读
	key = bs8116_iic_read_byte() << 8 | key;
	bs8116_iic_send_ack(1);//不需要数据了
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
		default:break;
	}
	
	return 0xff;
}




/**
 * 函数名    :bs8116_key_scan
 * 函数功能  :BS8116按键扫描
 * 函数参数  :无
 * 函数返回值:uint8_t
 * 函数描述  :加标志位，按1次，返回按下的按键。
 */
uint8_t bs8116_key_scan(void)
{
	static uint8_t bs8116_key_flag = 1;
	uint8_t key = 0xff;
	
	if(!BS8116_IRQ  && bs8116_key_flag)
	{
		key = bs8116_read_key(); 
		//检测按键获取值是否为有效值,无效值就不锁定标志位,有效值才锁定标志位
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











































