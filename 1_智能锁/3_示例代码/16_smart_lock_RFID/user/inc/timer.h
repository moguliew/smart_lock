#ifndef _TIM_H
#define _TIM_H
//宏定义和函数声明
#include "stm32f4xx.h"


void Tim5_delay_ms(u32 ms);
void Tim5_delay_us(u32 us);
void Tim9_IT_ms(u32 ms);
void TIM4_ch4_Led2_Pwm(void);
#endif


