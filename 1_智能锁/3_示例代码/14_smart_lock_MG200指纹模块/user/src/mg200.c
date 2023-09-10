#include "stm32f4xx.h"                  // Device header
#include "mg200.h" 
#include "stdio.h" 
#include "tim.h" 
#include "voice.h" 
#include "door.h" 
/**
 * 函数名    ：usart6_init
 * 函数功能  ：USART6初始化配置
 * 函数参数  ：uint32_t bands
 * 函数返回值：无
 * 函数描述  ：
 */
void usart6_init(uint32_t bands)
{
	/* IO */
	//1. 打开外设的时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	
	//2. 配置PC6和PC7
	GPIO_InitTypeDef  GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;//配置为复用
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;//推挽
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;//6和7号管脚
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;//无上下拉
	GPIO_InitStruct.GPIO_Speed = GPIO_Low_Speed;//低速
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	
	//3.把PC6和PC7复用到USART6中
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_USART6);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_USART6);
	
	
	/*配置USART6*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);
	
	USART_InitTypeDef  USART_InitStruct = {0};
	USART_InitStruct.USART_BaudRate = bands;//波特率
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件流控制
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//使能发送接收
	USART_InitStruct.USART_Parity = USART_Parity_No;//无奇偶校验
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b; 
	USART_Init(USART6,&USART_InitStruct);
	//使能中断
	USART_ITConfig(USART6,USART_IT_RXNE,ENABLE);//使能接收中断
	USART_ITConfig(USART6,USART_IT_IDLE,ENABLE);//使能空闲中断
		
	/*NVIC控制器配置*/
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = USART6_IRQn;    //中断源设置
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;      //响应通道使能
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;  //设置抢占优先级级别值
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;         //设置响应优先级级别值      
	NVIC_Init(&NVIC_InitStruct);
	
	
	//5.使能USART6
	USART_Cmd(USART6,ENABLE);
}


/**
 * 函数名    ：usart6_send_byte
 * 函数功能  ：串口6发送一个字节函数
 * 函数参数  ：uint8_t data
 * 函数返回值：无
 * 函数描述  ：
 */
void usart6_send_byte(uint8_t data)
{
	//等待上一位发送完成
	while(!(USART_GetFlagStatus(USART6,USART_FLAG_TC)));
	USART_SendData(USART6, data);
}

/**
 * 函数名    ：usart6_send_str
 * 函数功能  ：串口6发送多字节函数
 * 函数参数  ：uint8_t *str
 * 函数返回值：无
 * 函数描述  ：
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
 * 函数名    ：mg200_init
 * 函数功能  ：指纹模块初始化配置
 * 函数参数  ：无
 * 函数返回值：无
 * 函数描述  ：PC0-----Detect
 *             PC1-----PWR
 */
void mg200_init(void)
{
	/* 串口6配置 */
	usart6_init(115200);
	
	/* MG200_IO */
	//1. 打开外设的时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	
	//2. 配置PC0、PC1
	GPIO_InitTypeDef  GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;//配置为输入
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;//0号管脚
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;//无上下拉
	GPIO_InitStruct.GPIO_Speed = GPIO_Low_Speed;//低速
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;//配置为输出
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;//1号管脚
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	
	//开电源
	MG200_PWR_OFF;

}



/**
 * 函数名    ：mg200_write_cmd
 * 函数功能  ：MG20发送指令函数
 * 函数参数  ：uint8_t cmd,uint8_t parameter
 * 函数返回值：无
 * 函数描述  ：
 */
void mg200_write_cmd(uint8_t cmd,uint8_t parameter)
{
	uint8_t check_sum = 0;        //校验和变量
	
	usart6_send_byte(0x6c);
	usart6_send_byte(0x63);
	usart6_send_byte(0x62);
	usart6_send_byte(cmd);
	usart6_send_byte(parameter);
	usart6_send_byte(0);
	check_sum = (0x63 +0x62 + cmd +parameter) & 0xff;           
	usart6_send_byte(check_sum);
	//防止二次上电  会发送数据给MCU  意外导致标志位提前置1
	mg200_rec_flag = 0;
}


/**
 * 函数名    ：mg200_read_cmd
 * 函数功能  ：解析中断中接收到的数据包
 * 函数参数  ：cmd--之前发送的指令，要接收到的应答码对应
 * 			   *parameter--接收到的命令参数
 *             *result--接收到命令执行的效果
 * 函数返回值：成功返回0，失败返回错误码 
 * 函数描述  ：        
 */
uint8_t mg200_read_cmd(uint8_t cmd,uint8_t *parameter,uint8_t *result)
{
	
	/* 等待指令数据包在中断中接收完成 */
	while(!mg200_rec_flag);
	mg200_rec_flag = 0;               //标志位清零
	
	/* 验证接收到的数据包 */
	if(mg200_rec_buf[0] != 0x6c)   return 1;
	if(mg200_rec_buf[1] != 0x62)   return 2;
	if(mg200_rec_buf[2] != 0x63)   return 3;
	if(mg200_rec_buf[3] != cmd)    return 4;
	
	*result = mg200_rec_buf[4];               //获取到执行效果数据
	*parameter = mg200_rec_buf[5];            //获取到返回的参数
	//if(mg200_rec_buf[6] != 0x00)   return 5;//预留位有时候会用到，不一定是0x00  获取版本号
	
	uint8_t check_sum = (0x62+0x63+cmd+*result+*parameter + mg200_rec_buf[6]) & 0xff;
	if(mg200_rec_buf[7] != check_sum) 
		return 6;
		
	return 0;
}



/**
 * 函数名    ：ReqCaptureAndExtract
 * 函数功能  ：MG200采集指纹特征
 * 函数参数  ：指令的参数
 * 函数返回值：uint8_t 
 * 函数描述  ：返回0 表示成功
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
			case 0: 	printf("抓取指纹图像及特征量提取成功\r\n");break;
			case 0xb1:  printf("指纹过小(small finger)\r\n");break;
			case 0xb2:  printf("无指纹 (no finger)\r\n");break;
			case 0xb3: 	printf("指纹位置过于靠左，需要将手指靠右边摁按（ left finger）\r\n");break;
			case 0xb4:  printf("指纹位置过于靠右，需要将手指靠左边摁按（ right finger）\r\n");break;
			case 0xb5:  printf("指纹位置过于靠上，需要将手指靠下边摁按（ up finger）\r\n");break;
			case 0xb6: 	printf("指纹位置过于靠下，需要将手指靠上边摁按 (down finger）\r\n");break;
			case 0xb7:  printf("湿手指 （指纹表面出汗或者水渍过多） (wet finger)\r\n");break;
			case 0xb8:  printf("干燥手指(dry finger)\r\n");break;
			case 0xc0: 	printf("特征提取错误\r\n");break;
			case 0xc2: 	printf("特征提取错误 (图像质量差)\r\n");break;
			default :   printf("抓取指纹图像及特征量提取错误\r\n");break;
		}
	}
	else
	{
		printf("通信失败！\r\n");
		
		return 0xff;
	}
		
	return result;
	
}





/**
 * 函数名    ：Enroll
 * 函数功能  ：注册新用户的指纹
 * 函数参数  ：无
 * 函数返回值：uint8_t 
 * 函数描述  ：返回0 表示注册成功
 */
uint8_t Enroll(void)
{
	uint8_t ret;
	
	/* 提示开始注册，感应上电 */
	printf("请将手指放到指纹模块开始注册指纹\r\n");
	    
	/* 三次采集 */
	//第一次
	printf("第一次指纹采集\r\n");
	while(!MG200_DETECT); 
	MG200_PWR_ON;                  //开电源
	tim5_delay_ms(30);
	do
	{
		ret = ReqCaptureAndExtract(0x00);
	}while(ret);
	printf("请抬起手指，再次采集下一次指纹\r\n");
	while(MG200_DETECT);
	
	//第二次
	printf("第二次指纹采集\r\n");
	
	while(!MG200_DETECT);
	do
	{
		ret = ReqCaptureAndExtract(0x01);
	}while(ret);
	printf("请抬起手指，再次采集下一次指纹\r\n");
	while(MG200_DETECT);
	
	//第三次
	printf("第三次指纹采集\r\n");
	while(!MG200_DETECT);
	do
	{
		ret = ReqCaptureAndExtract(0x02);
	}while(ret);
	
	
	
	/* 注册 */
	mg200_write_cmd(0x7f,0);          //注册ID为自动分配
	uint8_t para,resu;
	
	if(mg200_read_cmd(0x7f,&para,&resu) != 0)
	{
		printf("通信失败导致注册失败！如需重新注册，请再次进入注册功能！\r\n");
		MG200_PWR_OFF;
		return 0xff;
	}
	else
	{
		switch(resu)
		{
			case 0: printf("注册成功\r\n");break;
			case 0x83: printf("ID 错误(ID < 0 或者 ID > 最大用户数)或者通信错误\r\n");break;
			case 0x91: printf("注册失败(用户区域已满)\r\n");break;
			case 0x93: printf("已经注册的 ID\r\n");break;
			case 0x94: printf("指纹提取次数 < 3\r\n");break;
		}
	}
	//关闭电源
	MG200_PWR_OFF;
	return 0;
}




/**
 * 函数名    ：Match_l_n
 * 函数功能  ：比对指纹
 * 函数参数  ：无
 * 函数返回值：uint8_t    ID号
 * 函数描述  ：对比成功，接收包的参数是ID号，失败0x00
 *			   对比成功，接收包的result是0x00，失败0x92
 */
uint8_t Match_l_n(void)
{
	uint8_t ret;
	uint8_t rec_parameter = 0xff;
	static uint8_t show_flag = 1;
	
	if(show_flag)
	{
		printf("请放下手指进入指纹对比开锁\r\n");
		show_flag = 0;
	}
	
	//触摸指纹,感应上电
	if(MG200_DETECT)
	{
		MG200_PWR_ON;
		tim5_delay_ms(30);
		
		/* 采集指纹 */
		do
		{
			ret = ReqCaptureAndExtract(0x00);
		}while(ret);
		
		
		/* 对比指纹 */
		mg200_write_cmd(0x71,0);//发送比对的信息
		if(mg200_read_cmd(0x71,&rec_parameter,&ret) != 0)
		{
			printf("通信失败,接收数据包错误\r\n");
			MG200_PWR_OFF;
			voice(DOOROPEN_FAIL); 
			return 0xff;
		}
		
		/* 根据对比结果执行开门 */
		if(ret == 0x00)
		{
			printf("比对成功\r\n");
			voice(DOOROPEN_SUCCESS);
			open_door();
			while(MG200_DETECT);              //抬起指纹，锁门
			close_door();
		}
		else//0x92
		{
			printf("比对失败\r\n");
			voice(DOOROPEN_FAIL);             //报警
		}
		
		printf("ID号为：%d\r\n",rec_parameter);

		//断电
		MG200_PWR_OFF;	
	}
	
	return rec_parameter;	

}






/**
 * 函数名    ：erase_one
 * 函数功能  ：删除指定ID指纹
 * 函数参数  ：uint8_t ID号
 * 函数返回值：uint8_t 
 * 函数描述  ：
 */
uint8_t erase_one(uint8_t id)
{
	uint8_t rec_parameter,ret;
	
	MG200_PWR_ON;
	tim5_delay_ms(30);
	mg200_write_cmd(0x73,id);
	if(mg200_read_cmd(0x73,&rec_parameter,&ret))
	{
		printf("通信失败,接收数据包错误\r\n");
		MG200_PWR_OFF;
		return 0xff;
	}
	
	else
	{
		switch(ret)
		{
			case 0:printf("删除成功\r\n");break;
			case 0x83:printf("参数错误\r\n");break;
			case 0x90:printf("未注册的用户\r\n");break;
			case 0xff:printf("写入ROM错误\r\n");break;
		}
	}
	
	
	MG200_PWR_OFF;
	
	return 0;
}



/**
 * 函数名    ：erase_all
 * 函数功能  ：删除所有指纹ID
 * 函数参数  ：无
 * 函数返回值：uint8_t 
 * 函数描述  ：
 */
uint8_t erase_all(void)
{
	uint8_t rec_parameter,ret;
	
	MG200_PWR_ON;
	tim5_delay_ms(30);
	mg200_write_cmd(0x54,0);
	if(mg200_read_cmd(0x54,&rec_parameter,&ret))
	{
		printf("通信失败,接收数据包错误\r\n");
		MG200_PWR_OFF;
		return 0xff;
	}
	
	else
	{
		switch(ret)
		{
			case 0:printf("删除成功\r\n");break;
			case 0x90:printf("未注册的用户\r\n");break;
			default:printf("删除失败\r\n");break;
		}
		
	}
	
	
	MG200_PWR_OFF;
	
	
	return 0;
}












