#ifndef _USART_H
#define _USART_H
//�궨��ͺ�������
#include "stm32f4xx.h"

void usart1_init(uint32_t baud);
void usart1_send_byte(uint8_t data);
void usart1_send_str(uint8_t *str);

#endif





