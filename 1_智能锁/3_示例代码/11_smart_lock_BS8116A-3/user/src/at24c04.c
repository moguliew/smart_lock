#include "stm32f4xx.h"                  // Device header
#include "tim.h" 
#include "at24c04.h"

/************AT24C04_IIC**********************/
/**
 * 函数名    :iic_io_init
 * 函数功能  :IIC所用IO口初始化配置
 * 函数参数  :无
 * 函数返回值:无 
 * 函数描述  :PA8----SCL   通用推挽输出
 *            PC9----SDA   通用开漏输出
 */
void iic_io_init(void)
{
	//开GPIO时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	
	//工作模式
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
	
	//空闲状态
	IIC_SCL_H;
	IIC_SDA_OUT_H;
}


/**
 * 函数名    :iic_start
 * 函数功能  :iic起始信号函数
 * 函数参数  :无
 * 函数返回值:无
 * 函数描述  :时钟线为高电平，数据线出现下降沿
 */

void iic_start(void)
{
	//时钟线拉低，才可以动数据线
	IIC_SCL_L;  
	//数据线拉高，目的是为出现下降沿做准备
	IIC_SDA_OUT_H;   
	//时钟线拉高  产生起始信号的条件
	IIC_SCL_H;
	tim5_delay_us(5);
	//数据线拉低  可以产生起始信号
	IIC_SDA_OUT_L;
	tim5_delay_us(5);
	
	//安全
	IIC_SCL_L;
}



/**
 * 函数名    :iic_stop
 * 函数功能  :iic停止信号函数
 * 函数参数  :无
 * 函数返回值:无
 * 函数描述  :时钟线为高电平，数据线出现上升沿
 */
void iic_stop(void)
{
	//准备工作
	IIC_SCL_L;
	IIC_SDA_OUT_L;
	
	//产生停止信号
	IIC_SCL_H;
	tim5_delay_us(5);
	IIC_SDA_OUT_H;
	tim5_delay_us(5);
}



/**
 * 函数名    :iic_send_ack
 * 函数功能  :发送应答/不应答信号
 * 函数参数  :0--发送应答信号、1--发送不应答信号 
 * 函数返回值:无
 * 函数描述  :应答：时钟线为高电平，数据线已经保持好低电平了
 *			 不应答: 时钟线为高电平，数据线已经保持好高电平了				
 */
void iic_send_ack(uint8_t  ack)
{
	
	if(ack == 0)       		//发送应答
	{
		IIC_SCL_L;      	//时钟线拉低，才可以动数据线
		tim5_delay_us(5); 
		IIC_SDA_OUT_L;  	//让数据线先保持低电平
		tim5_delay_us(5); 
		IIC_SCL_H;      	//产生了应答信号	
		tim5_delay_us(5); 
	}
	else if (ack == 1) 		//发送不应答函数 
	{
		IIC_SCL_L;      	//时钟线拉低，才可以动数据线
		tim5_delay_us(5); 
		IIC_SDA_OUT_H;  	//让数据线先保持高电平
		tim5_delay_us(5); 
		IIC_SCL_H;      	//产生了不应答信号
		tim5_delay_us(5); 
	}

	//安全
	IIC_SCL_L;
}



/**
 * 函数名    :iic_get_ack
 * 函数功能  :检测应答/不应答信号
 * 函数参数  :无
 * 函数返回值:返回0 表示应答信号，返回1 表示不应答信号 
 * 函数描述  :		
 */
uint8_t  iic_get_ack(void)
{
	uint8_t  ack;
	
	/* 数据线要为输入 */
	IIC_SCL_L;
	IIC_SDA_OUT_H;
	
	/* 检测应答/不应答信号 */
	IIC_SCL_L;   			//主机帮助从机拉低时钟线，从机才可以动数据线，来表现应答/不应答信号
	tim5_delay_us(5);  
	IIC_SCL_H;   			//主机才可以读数据线
	tim5_delay_us(5);  
	if(IIC_SDA_INT == 0)
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
	IIC_SCL_L;
	return ack;
}



/*
*函数名    :iic_send_byte
*函数功能  :IIC发送一个字节函数
*函数参数  :uint8_t  data
*函数返回值:无
*函数描述  :
*/
void iic_send_byte(uint8_t  data)
{
	uint8_t  i;
	for(i = 0;i < 8;i++)
	{
		//时钟线拉低才可以改变数据线
		IIC_SCL_L;
		tim5_delay_us(5);  
		//根据参数的对应位是0还是1决定数据线的高低
		if(data & 0x80)
		{
			IIC_SDA_OUT_H;
		}
		else
		{
			IIC_SDA_OUT_L;
		}
		tim5_delay_us(5);  
		//时钟线拉高，帮助从机，这样从机才可以读这一位数据
		IIC_SCL_H;
		tim5_delay_us(5);  
		//下一位数据，次高位变成最高位
		data = data << 1;
	}
	
	//安全
	IIC_SCL_L;
}




/**
 * 函数名    :iic_read_byte
 * 函数功能  :主机通过IIC接收一个字节函数
 * 函数参数  :无
 * 函数返回值:uint8_t 
 * 函数描述  :
 */
uint8_t  iic_read_byte(void)
{
	uint8_t  data;    
	uint8_t  i;
	
	/* 将IO口切换为输入 */
	IIC_SCL_L;
	IIC_SDA_OUT_H;
	
	/* 读数据 */
	for(i = 0;i < 8;i++)
	{
		IIC_SCL_L;             //主机帮助从机拉低时钟线
		tim5_delay_us(5);   
		IIC_SCL_H;             //主机可以读数据
		tim5_delay_us(5);  
		data = data << 1;
		if(IIC_SDA_INT) 
			data = data | 0x01;		
	}
	
	//安全
	IIC_SCL_L;
	
	return data;
}



/************************AT24C04*************************/
/**
 * 函数名    :at24c04_init
 * 函数功能  :往AT24C04的初始化
 * 函数参数  :
 * 函数返回值:
 */
void at24c04_init(void)
{
	iic_io_init();
}


/**
 * 函数名    :at24c04字节写
 * 函数功能  :往AT24C04的某个地址空间写一个字节数据
 * 函数参数  :inner_addr--器件内部存储空间地址（0~511），data--要写入的数据
 * 函数返回值:每次发送都会检测响应，通过返回值判断具体哪次发送出问题			
 * 函数描述  :宏定义错误标志：
 *				AT24C04_NO_ERR 0;   //应答（无错误）
 *				AT24C04_ERR1   1;   //无应答
 *				AT24C04_ERR2   2;   //无应答
 *				AT24C04_ERR3   3;   //无应答
 */
uint8_t  at24c04_write_byte(uint16_t  inner_addr,uint8_t  data)
{
	uint8_t  ack;
	
	iic_start();
	
	if(inner_addr <= 255)//存到第一块
	{
		iic_send_byte(AT24C04_ADDR0_W);    //第一块的写器件地址
		
	}
	else//存第二块
	{
		iic_send_byte(AT24C04_ADDR1_W);   
		//inner_addr = inner_addr - 256;    //第二块的实际地址
		//inner_addr = inner_addr & 0xff;    //第二块的实际地址
	}
	ack = iic_get_ack();
	if(ack == 1)
	{
		iic_stop();
		return AT24C04_ERR1; 
	}
	
	/* 器件绝对地址 */
	iic_send_byte(inner_addr & 0xff);     
	ack = iic_get_ack();
	if(ack == 1)
	{
		iic_stop();
		return AT24C04_ERR2; 
	}
	
	/* 发送数据 */
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
 * 函数名    :at24c04_read_byte
 * 函数功能  :从AT24C04的某个地址空间读一个字节数据
 * 函数参数  :inner_addr--器件内部存储空间地址(0~511)，uint8_t *data--读到的数据存放的地址
 * 函数返回值:每次发送都会检测响应，通过返回值判断具体哪次发送出问题			
 * 函数描述  :宏定义错误标志：
 *				AT24C04_NO_ERR 0;   //应答（无错误）
 *				AT24C04_ERR1   1;   //无应答
 *				AT24C04_ERR2   2;   //无应答
 *				AT24C04_ERR3   3;   //无应答
 */
uint8_t  at24c04_read_byte(uint16_t  inner_addr,uint8_t  *data)
{
	uint8_t  ack;
	
	iic_start();
	if(inner_addr <= 255)//从第一块读
	{
		iic_send_byte(AT24C04_ADDR0_W);    //第一块的写器件地址
	}
	else//从第二块读
	{
		iic_send_byte(AT24C04_ADDR1_W);    //第二块的写器件地址
	} 
	ack = iic_get_ack();
	if(ack == 1)
	{
		iic_stop();
		return AT24C04_ERR1; 
	}
	
	/* 发送绝对地址 */
	iic_send_byte(inner_addr & 0xff);
	ack = iic_get_ack();
	if(ack == 1)
	{
		iic_stop();
		return AT24C04_ERR2; 
	}
	
	
	iic_start();
	if(inner_addr <= 255)//读第一块
	{
		iic_send_byte(AT24C04_ADDR0_R);    //第一块的读器件地址
	}
	else//读第二块
	{
		iic_send_byte(AT24C04_ADDR1_R);    //第二块的读器件地址   
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
 * 函数名    :at24c04_read_bytes
 * 函数功能  :从AT24C04的某个地址空间开始连续读多个字节数据
 * 函数参数  :inner_addr-----器件内部存储空间地址（0~511），
 *			  num------------要读多少个字节，
 *            uint8_t  *str--读到的数据存放的地址
 * 函数返回值:每次发送都会检测响应，通过返回值判断具体哪次发送出问题			
 * 函数描述  :宏定义错误标志：
 *				AT24C04_NO_ERR 0;   //应答（无错误）
 *				AT24C04_ERR1   1;   //无应答
 *				AT24C04_ERR2   2;   //无应答
 *				AT24C04_ERR3   3;   //无应答
 */
uint8_t  at24c04_read_bytes(uint16_t  inner_addr,uint8_t  num,uint8_t  *str)
{
	uint8_t  ack;
	
	iic_start();
	if(inner_addr <= 255)//存到第一块
	{
		iic_send_byte(AT24C04_ADDR0_W);    //第一块的写器件地址
	}
	else//存第二块
	{
		iic_send_byte(AT24C04_ADDR1_W);    //第二块的写器件地址
	} 
	ack = iic_get_ack();
	if(ack == 1)
	{
		iic_stop();
		return AT24C04_ERR1; 
	}	
	
	/* 发送绝对地址 */
	iic_send_byte(inner_addr & 0xff);
	ack = iic_get_ack();
	if(ack == 1)
	{
		iic_stop();
		return AT24C04_ERR2; 
	}
	
	iic_start();
	if(inner_addr <= 255)//读第一块
	{
		iic_send_byte(AT24C04_ADDR0_R);    //第一块的读器件地址
	}
	else//读第二块
	{
		iic_send_byte(AT24C04_ADDR1_R);    //第二块的读器件地址   
	} 
	ack = iic_get_ack();
	if(ack == 1)
	{
		iic_stop();
		return AT24C04_ERR1; 
	}	
	
	//循环读数据
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
 * 函数名    :at24c04_write_page
 * 函数功能  :往AT24C04的某个地址空间开始页写
 * 函数参数  :uint16_t  inner_addr--器件内部存储空间地址（0~511）  
 *            uint8_t  num----------要写多少个字节
 *			  uint8_t  *str---------要写的字符串的首地址
 * 函数返回值:每次发送都会检测响应，通过返回值判断具体哪次发送出问题			
 * 函数描述  :宏定义错误标志：
 *				AT24C04_NO_ERR 0;   //应答（无错误）
 *				AT24C04_ERR1   1;   //无应答
 *				AT24C04_ERR2   2;   //无应答
 *				AT24C04_ERR3   3;   //无应答
 *				AT24C04_ERR4   4;   //跨页错误
 */
uint8_t  at24c04_write_page(uint16_t  inner_addr,uint8_t  num,uint8_t  *str)
{
	uint8_t  ack;
	
	/* 判断要写的内容是否会跨页 */
	if(inner_addr / 16 != (inner_addr + num - 1) / 16)
	{
		return AT24C04_ERR4;
	}
	
	/* 页写 */
	iic_start();
	if(inner_addr <= 255)//存到第一块
	{
		iic_send_byte(AT24C04_ADDR0_W);    //第一块的写器件地址
	}
	else//存第二块
	{
		iic_send_byte(AT24C04_ADDR1_W);    //第二块的写器件地址
	} 
	ack = iic_get_ack();
	if(ack == 1)
	{
		iic_stop();
		return AT24C04_ERR1; 
	}	
	
	/* 发送绝对地址 */
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
	
	//写周期
	tim5_delay_ms(5);
	
	return AT24C04_NO_ERR;
}




/**
 * 函数名    :at24c04_write_bytes
 * 函数功能  :往AT24C04的某个地址空间开始连续写可以跨页
 * 函数参数  :uint16_t  inner_addr--器件内部存储空间地址  0~511  
 *            uint8_t  num----------要读多少个字节
 *			  uint8_t  *str---------要写的字符串的首地址
 * 函数返回值:每次发送都会检测响应，通过返回值判断具体哪次发送出问题			
 * 函数描述  :宏定义错误标志：
 *				AT24C04_NO_ERR 0;   //应答（无错误）
 *				AT24C04_ERR1   1;   //无应答
 *				AT24C04_ERR2   2;   //无应答
 *				AT24C04_ERR3   3;   //无应答
 */
uint8_t  at24c04_write_bytes(uint16_t  inner_addr,uint8_t  num,uint8_t *str)
{
	uint8_t  less_num;
	
	while(1)
	{	
		less_num = 16 - (inner_addr % 16);//计算本页还剩多少个空间可以写     
		/* 判断是否会跨页 */
		if(less_num >= num)//不跨页
		{
			at24c04_write_page(inner_addr,num,str);//调用页写函数将写完  
			break;//退出循环结构	
		}		
		else//跨页
		{
			//先调用页写函数把本页写完
			at24c04_write_page(inner_addr,less_num,str);
			//存储芯片切换到下页的起始位置位置
			inner_addr = inner_addr + less_num;
			
			//计算出剩余要写的个数
			num = num - less_num;                 //7
			//计算出剩余要写的内容的地址
			str = str + less_num;	
		}
	}		
	return AT24C04_NO_ERR;
}



















