#ifndef _NVIC_H
#define _NVIC_H
//�궨��ͺ�������
#include "stm32f4xx.h"


extern u8 zk_flag;

typedef struct{
	u8 buff[1024];//������Ҫ���ַ���
	u16 len; //�յ������ݵĳ���
	u8 flag;//��ʾ���ݽ�����ɵı�־
}USART2_RECSTR;

extern USART2_RECSTR esp32rec;






#endif





