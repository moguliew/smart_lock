#ifndef _USART_H
#define _USART_H
//�궨��ͺ�������
#include "stm32f4xx.h"



void usart1_init(u32 baud);
void usart1_send_byte(u8 data);
void usart1_send_str(u8 *str);
#endif





