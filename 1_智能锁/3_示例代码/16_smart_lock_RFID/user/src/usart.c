#include "stm32f4xx.h"                  // Device header
#include "stdio.h" 
/***********************************************
*函数名    ：usart1_init
*函数功能  ：对USART1初始化配置
*函数参数  ：u32 baud
*函数返回值：无
*函数描述  ：PA9-----Tx
             PA10----Rx
***********************************************/
void usart1_init(u32 baud)
{

	/*IO控制器配置*/
	//端口时钟使能
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;          //通用输出
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;         //推挽
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; // 8 9
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;       //无上下拉
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;       //速度
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	//复用功能配置
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9 ,GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10 ,GPIO_AF_USART1);
	
	/*串口控制器配置*/
	//串口时钟使能
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	
	 USART_InitTypeDef USART_InitStruct;
	 USART_InitStruct.USART_BaudRate = baud;                                     //波特率设置
	 USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件流控制器
	 USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                //发送接收使能
	 USART_InitStruct.USART_Parity = USART_Parity_No;                            //无奇偶校验
	 USART_InitStruct.USART_StopBits = USART_StopBits_1;                         //一个停止位
	 USART_InitStruct.USART_WordLength = USART_WordLength_8b;                    //8位字长
	 USART_Init(USART1, &USART_InitStruct);
	 
	 //中断使能
	 USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);                                //接收中断使能
	 USART_ITConfig(USART1,USART_IT_IDLE,ENABLE);                                //空闲中断使能
		

	/*NVIC控制器配置*/
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;    //中断源设置
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;      //响应通道使能
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;  //设置抢占优先级级别值
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;         //设置响应优先级级别值      
	NVIC_Init(&NVIC_InitStruct);
	
	//串口使能
	USART_Cmd(USART1,ENABLE);
}




/***********************************************
*函数名    ：usart1_send_byte
*函数功能  ：串口1发送一个字节函数
*函数参数  ：u8 data
*函数返回值：无
*函数描述  ：
***********************************************/
void usart1_send_byte(u8 data)
{
	//等待上一位发送完成
	while(!(USART_GetFlagStatus(USART1,USART_FLAG_TC)));
	USART_SendData(USART1, data);
}



/***********************************************
*函数名    ：usart1_send_str
*函数功能  ：串口1发送多字节函数
*函数参数  ：u8 *str
*函数返回值：无
*函数描述  ：
***********************************************/
void usart1_send_str(u8 *str)
{
	while(*str != '\0')
	{
		usart1_send_byte(*str);
		str++;
	}
}





#if 1
	#pragma import(__use_no_semihosting)             
	//标准库需要的支持函数                 
	struct __FILE 
	{ 
		int handle; 
		/* Whatever you require here. If the only file you are using is */ 
		/* standard output using printf() for debugging, no file handling */ 
		/* is required. */ 
	}; 
	/* FILE is typedef’ d in stdio.h. */ 
	FILE __stdout;       
	//定义_sys_exit()以避免使用半主机模式    
	int _sys_exit(int x) 
	{ 
		x = x; 
	} 
	//重定义fputc函数 
	int fputc(int ch, FILE *f)
	{      
		while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
		USART1->DR = (u8) ch;      
		return ch;
	}
	#endif 








