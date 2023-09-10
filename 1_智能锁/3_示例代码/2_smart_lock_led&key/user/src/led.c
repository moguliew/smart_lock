#include "led.h"

/** 
 * @brief:LED初始化
 * @note:
 * 		 LED1---PB8---低电平亮
 *		 LED2---PB9---低电平亮
 */
void led_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	//开GPIOB时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	//工作模式配置	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP; 
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Fast_Speed;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	//初始化工作模式
	LED1_OFF;
	LED2_OFF;
	
}






