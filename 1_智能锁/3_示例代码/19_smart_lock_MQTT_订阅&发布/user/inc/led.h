#ifndef _LED_H
#define _LED_H
//宏定义和函数声明

#define LED1_ON (GPIO_ResetBits(GPIOB, GPIO_Pin_8))
#define LED2_ON (GPIO_ResetBits(GPIOB, GPIO_Pin_9))

#define LED1_OFF (GPIO_SetBits(GPIOB, GPIO_Pin_8))
#define LED2_OFF (GPIO_SetBits(GPIOB, GPIO_Pin_9))

#define LED1_OVERTURN (GPIO_ToggleBits(GPIOB, GPIO_Pin_8))
#define LED2_OVERTURN (GPIO_ToggleBits(GPIOB, GPIO_Pin_9))

void led_init(void);

#endif



