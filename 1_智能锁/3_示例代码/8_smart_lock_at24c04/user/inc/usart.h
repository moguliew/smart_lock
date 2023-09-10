#ifndef _USART_H
#define _USART_H
//宏定义和函数声明
#include "stm32f4xx.h"

void usart1_init(uint32_t baud);
void usart1_send_byte(uint8_t data);
void usart1_send_str(uint8_t *str);

#endif





