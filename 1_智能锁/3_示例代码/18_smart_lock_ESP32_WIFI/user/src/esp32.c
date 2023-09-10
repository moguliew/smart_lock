#include "stm32f4xx.h"                  // Device header
#include "nvic.h"
#include "string.h"
#include "timer.h"
#include "stdio.h"
/*****************************************************
函数功能：usart2初始化
函数形参：void
函数返回值：void
函数说明：
		PA2
		PA3
*****************************************************/
void usart2_esp32_Init(void)
{
	//1. 打开外设的时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	
	//2. 配置PB8和PB9
	GPIO_InitTypeDef  GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;//配置为复用
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;//推挽
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;//6和7号管脚
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;//无上下拉
	GPIO_InitStruct.GPIO_Speed = GPIO_Low_Speed;//低速
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	//3.把PC6和PC7复用到USART6中
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2);
	
	//4. 配置USART1
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
	USART_InitTypeDef  USART_InitStruct = {0};
	USART_InitStruct.USART_BaudRate = 115200;//波特率
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件流控制
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//使能发送接收
	USART_InitStruct.USART_Parity = USART_Parity_No;//无奇偶校验
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b; 
	USART_Init(USART2,&USART_InitStruct);
	
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);//使能接收中断
	USART_ITConfig(USART2,USART_IT_IDLE,ENABLE);//使能空闲中断
	
	NVIC_SetPriority(USART2_IRQn,NVIC_EncodePriority(7-2,1,1));
	NVIC_EnableIRQ(USART2_IRQn);
	
	//5.使能USART1
	USART_Cmd(USART2,ENABLE);
	
}


/*****************************************************
函数名    : usart2_esp32_send_str
函数功能  ：串口2发送字符串到ESP32
函数形参  ：u8 *data
函数返回值：void
函数说明  ：		
*****************************************************/
void usart2_esp32_send_str(u8 *data)
{
	while(*data != '\0')
	{
		while((USART2->SR & (0X1 << 6)) == 0);
		USART2->DR = *data++;
	}
}






/*****************************************************
函数名称  : Esp32_SendandReceive
函数功能  ：ESP32判断是否接收到想要的回应信号
函数形参  ：
			      u8 *cmd  -- 发送的指令
			      u8 *respond -- 回应的信息  "OK"
            u32 wait_ms  -- 设定超时时间
函数返回值：u8
函数说明  ：正常就返回0，否则返回1		
*****************************************************/
u8 Esp32_SendandReceive(u8 *cmd,u8 *respond,u32 wait_ms)  
{
	
	u32 esp32_cont = 0; 
	
	//发送指令
	usart2_esp32_send_str(cmd);
	//延时100ms  指令间隔时间
	Tim5_delay_ms(100);
	
	//判断指令执行是否成功,不成功超时退出
	while(1)
	{
		//等待中断里面的接收完成标志位置1
		while(!esp32rec.flag)      
		{
			//超时检测
			esp32_cont++;
			Tim5_delay_ms(1);
			if(esp32_cont == wait_ms)
			{
				//超时,命令失败
				return 1;     
			}
		}
		esp32rec.flag = 0;     //下一次接收会重新置1
		//判断通过中断返回来的数据包,来确定指令是否执行成功
		if(strstr((const char *)esp32rec.buff, (const char *)respond) != NULL)
		{
			//找到了   响应成功
			return 0;
		} 	
	}
}








/***********************************************
*函数名    ：Exit_Cipmode
*函数功能  ：退出透传模式,进入设置模式
*函数参数  ：无
*函数返回值：无
*函数描述  ：从发送数据模式进入到设置WIFI的时候
             需要用此函数
***********************************************/
void Exit_Cipmode(void)
{
	//发送退出透传命令
	usart2_esp32_send_str("+++");
	Tim5_delay_ms(2000); 
	printf("透传模式以退出,进入设置模式\r\n");
}




/***********************************************
*函数名    ：ESP32_Restore
*函数功能  ：恢复出厂设置
*函数参数  ：无
*函数返回值：u8   返回0:成功      返回1:失败
*函数描述  ：
***********************************************/
u8 ESP32_Restore(void)
{
	u8 sta;
	
	sta = Esp32_SendandReceive("AT+RESTORE\r\n","ready",3000);
	if(sta==0)
	{
		printf("恢复出厂设置成功\r\n");
	}
	else
	{
		printf("恢复出厂设置失败\r\n");
	}
	
	return sta;
}




/***********************************************
*函数名    ：ESP32_init
*函数功能  ：ESP32初始化配置函数
*函数参数  ：无
*函数返回值：
*函数描述  ：
***********************************************/
void Esp32_Init(void)
{
	/*串口2初始化*/
	usart2_esp32_Init();
	
	/*WIFI相关初始配置*/
	//初始命令,无效
	Esp32_SendandReceive((u8 *)"AT\r\n",(u8 *)"OK",10); 
	
	if(Esp32_SendandReceive((u8 *)"AT\r\n",(u8 *)"OK",10) != 0 )
	{
		//退出透传
		Exit_Cipmode();  //+++
	}
	
	//恢复出厂设置模块
	ESP32_Restore();
	//设置位客户端模式
	if(Esp32_SendandReceive((u8 *)"AT+CWMODE=1\r\n",(u8 *)"OK",10) == 0)
	{
		printf("客户端模式指令执行成功\r\n");
	}
	else  printf("客户端模指令执行失败\r\n");
	
	/*TCP相关初始配置*/
	//透传模式设定
	if(Esp32_SendandReceive((u8 *)"AT+CIPMODE=1\r\n",(u8 *)"OK",10) == 0)
	{
		printf("透传指令执行成功\r\n");
		
	}
	else  printf("透传指令执行失败\r\n");	
}



/***********************************************
*函数名    ：Esp32_Wificonnect
*函数功能  ：WIFI连接函数
*函数参数  ：u8 *user,u8*password
*函数返回值：u8
*函数描述  ：返回0连接成功,返回1连接失败
***********************************************/
u8 Esp32_Wificonnect(u8 *user,u8*password)
{											
	u8 wifi_buff[50] = "AT+CWJAP=\"";
	/*拼接用户名和密码,成为规定的格式*/
	strcat((char*)wifi_buff,(char*)user);
	// AT+CWJAP=\"LL
	
	strcat((char*)wifi_buff,"\",\"");		
	//AT+CWJAP=\"LL\",\"
	
	strcat((char*)wifi_buff,(char *)password);
	//AT+CWJAP=\"LL\",\"12345678
	
	strcat((char*)wifi_buff,"\"\r\n");
	//AT+CWJAP=\"LL\",\"12345678\"\r\n
	 
	/*连接WIFI*/
	if(Esp32_SendandReceive(wifi_buff ,(u8 *)"OK",5000) == 0)
	{
		printf("WIFI连接指令执行成功\r\n");
		return 0;
	}
	else  printf("WIFI连接指令执行失败\r\n");
	return 1;
}



/***********************************************
*函数名    ：Esp32_Wificlose
*函数功能  ：WIFI关闭函数
*函数参数  ：void
*函数返回值：u8
*函数描述  ：返回0连接成功,返回1连接失败
***********************************************/
u8 Esp32_Wificlose(void)
{
	
	//检测WIFI是否在传输模式
	if(Esp32_SendandReceive((u8 *)"AT\r\n",(u8 *)"OK",10) != 0 )
	{
		//退出透传
		Exit_Cipmode();  //+++
	}
	
	
	//关闭WIFI
	if(Esp32_SendandReceive((u8 *)"AT+CWQAP\r\n",(u8 *)"OK",10) == 0)
	{
		printf("断开WIFI指令执行成功\r\n");
		return 0;
	}
	else  printf("断开WIFI指令执行失败\r\n");
	return 1;	
	
}






/***********************************************
*函数名    ：Esp32_TCPconnect
*函数功能  ：TCP连接函数
*函数参数  ：u8 *remote_ip,u32 com
*函数返回值：u8
*函数描述  ：返回0连接成功,返回1连接失败
***********************************************/
u8 Esp32_TCPconnect(u8 *remote_ip,u32 com)
{
	
	u8 tcp_buff[80] = {0};
	
	
	//将IP和端口号拼接成一个字符串
	sprintf((char *)tcp_buff,"AT+CIPSTART=\"TCP\",\"%s\",%d\r\n",remote_ip,com);
	
	
	
	
	//连接TCP
	if(Esp32_SendandReceive((u8 *)tcp_buff,(u8 *)"OK",5000) == 0)
	{
		printf("TCP连接指令执行成功\r\n");
		if(Esp32_SendandReceive((u8 *)"AT+CIPSEND\r\n",(u8 *)"OK",10) == 0)
		{
			printf("发送数据指令执行成功\r\n");
			printf("服务器连接成功,进入传输数据模式\r\n");
			return 0;
		}
		else
		{
			printf("发送数据指令执行失败\r\n");
			return 2;
		}
	}
	
	
	
	else  
	{
		printf("TCP连接指令执行失败\r\n");
		return 1;
		
	}
	
	 
	
	
	
}













