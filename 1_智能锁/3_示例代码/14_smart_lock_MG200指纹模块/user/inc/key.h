#ifndef __KEY_H_
#define __KEY_H_

#include "stm32f4xx.h"

#define KEY 	GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13)

void key_init(void);
uint8_t key_scan(void);






#endif


