#ifndef _NVIC_H
#define _NVIC_H
//宏定义和函数声明
#include "stm32f4xx.h"


extern u8 zk_flag;

typedef struct{
	u8 buff[1024];//接收需要的字符串
	u16 len; //收到的数据的长度
	u8 flag;//表示数据接收完成的标志
}USART2_RECSTR;

extern USART2_RECSTR esp32rec;






#endif





