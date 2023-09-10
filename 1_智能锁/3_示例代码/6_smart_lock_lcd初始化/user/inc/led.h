#ifndef __LED_H_
#define __LED_H_

#include "stm32f4xx.h"

#define LED1_ON 	GPIO_ResetBits(GPIOB, GPIO_Pin_8)
#define LED1_OFF	GPIO_SetBits(GPIOB, GPIO_Pin_8)
#define LED1_TOGGLE	GPIO_ToggleBits(GPIOB, GPIO_Pin_8)
#define LED2_ON		GPIO_ResetBits(GPIOB, GPIO_Pin_9)
#define LED2_OFF    GPIO_SetBits(GPIOB, GPIO_Pin_9)
#define LED2_TOGGLE	GPIO_ToggleBits(GPIOB, GPIO_Pin_9)

void led_init(void);






#endif


