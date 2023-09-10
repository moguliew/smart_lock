#ifndef _MG200_H
#define _MG200_H
//宏定义和函数声明
#include "stm32f4xx.h"


extern uint8_t mg200_rec_buf[10];
extern uint8_t mg200_rec_flag ;

#define MG200_DETECT  (GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_0))	 
#define MG200_PWR_ON  (GPIO_SetBits(GPIOC,GPIO_Pin_1))
#define MG200_PWR_OFF (GPIO_ResetBits(GPIOC,GPIO_Pin_1))


void mg200_init(void);
void mg200_write_cmd(uint8_t cmd,uint8_t parameter);
uint8_t mg200_read_cmd(uint8_t cmd,uint8_t *parameter,uint8_t *result);
uint8_t ReqCaptureAndExtract(uint8_t parameter);
uint8_t Enroll(void);
uint8_t Match_l_n(void);

uint8_t erase_one(uint8_t id);
uint8_t erase_all(void);



#endif


