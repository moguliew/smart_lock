#ifndef _USART_H
#define _USART_H
//宏定义和函数声明
#include "stm32f4xx.h"



void usart1_init(u32 baud);
void usart1_send_byte(u8 data);
void usart1_send_str(u8 *str);
#endif





