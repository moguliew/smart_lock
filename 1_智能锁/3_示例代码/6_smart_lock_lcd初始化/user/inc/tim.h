#ifndef _TIM_H
#define _TIM_H
//�궨��ͺ�������
#include "stm32f4xx.h"


void tim5_delay_ms(uint32_t ms);
void tim5_delay_us(uint32_t us);
void tim9_it_ms(uint32_t ms);
void tim4_ch4_pwm(void);

#endif


