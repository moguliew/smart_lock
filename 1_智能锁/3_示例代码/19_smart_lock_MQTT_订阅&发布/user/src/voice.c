#include "stm32f4xx.h"                  // Device header
#include "voice.h"
#include "timer.h"

/***********************************************
*函数名    ：voice_init
*函数功能  ：对语音模块的管脚初始haul配置
*函数参数  ：无
*函数返回值：无
*函数描述  ：VOICE_BUSY------------PC4      //通用输入
						VOICE_DATA------------PC5      //通用推挽输出
***********************************************/
void voice_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	//端口时钟使能
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;          //通用输出
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;         //推挽
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5 ;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;       //无上下拉
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;       //速度
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;          //通用输入
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4 ;
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	
	//空闲低
	GPIO_ResetBits(GPIOC, GPIO_Pin_5);
	
	
	
	//需要上电时间
	//Tim5_delay_ms(100);
}



/***********************************************
*函数名    ：voice_head
*函数功能  ：同步头
*函数参数  ：无
*函数返回值：无
*函数描述  ：
***********************************************/
void voice_head(void)
{
	VOICE_DATA_H;
	Tim5_delay_ms(8);
	VOICE_DATA_L;
	Tim5_delay_ms(1);
	
}


/***********************************************
*函数名    ：voice_low
*函数功能  ：数据0
*函数参数  ：无
*函数返回值：无
*函数描述  ：
***********************************************/
void voice_low(void)
{
	VOICE_DATA_H;
	Tim5_delay_us(500);
	VOICE_DATA_L;
	Tim5_delay_us(1500);
	
}

/***********************************************
*函数名    ：voice_hig
*函数功能  ：数据1
*函数参数  ：无
*函数返回值：无
*函数描述  ：
***********************************************/
void voice_hig(void)
{
	VOICE_DATA_H;
	Tim5_delay_us(1500);
	VOICE_DATA_L;
	Tim5_delay_us(500);
}



/***********************************************
*函数名    ：voice
*函数功能  ：播报语音
*函数参数  ：u8 cmd
*函数返回值：无
*函数描述  ：
***********************************************/
void voice(u8 cmd)
{
	while(VOICE_BUSY);
	//同步头
	voice_head();
	//传送八位数据命令
	for(u8 i=0;i<8;i++)
	{
		if(cmd  &  (0x80 >> i))
		{
			voice_hig();
		}
		else
		{
			voice_low();
		}
	}
}


































