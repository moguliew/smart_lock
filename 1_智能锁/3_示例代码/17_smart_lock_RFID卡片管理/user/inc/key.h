#ifndef _KEY_H
#define _KEY_H
//�궨��ͺ�������
#include "stm32f4xx.h"
#define KEY (GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13))


void key_init(void);
u8 key_scan(void);
#endif


