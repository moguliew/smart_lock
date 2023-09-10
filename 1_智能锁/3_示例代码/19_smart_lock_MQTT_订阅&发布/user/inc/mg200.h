#ifndef _MG200_H
#define _MG200_H
//宏定义和函数声明
#include "stm32f4xx.h"


extern u8 mg200_rec_buff[10];
extern u8 mg200_rec_flag ;

extern u8 page_mode ;



#define MG200_DETECT  (GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_0))	 
#define MG200_PWR_ON  (GPIO_SetBits(GPIOC,GPIO_Pin_1))
#define MG200_PWR_OFF (GPIO_ResetBits(GPIOC,GPIO_Pin_1))


void mg200_init(void);
void mg200_write_cmd(u8 cmd,u8 parameter);
u8 mg200_read_cmd(u8 cmd,u8 *parameter,u8 *result);
u8 ReqCaptureAndExtract(u8 parameter);
u8 Enroll(u8 ID);
u8 Match_l_n(void);

u8 erase_one(u8 id);
u8 erase_all(void);



#endif


