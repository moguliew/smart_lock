#include "key.h"

static void delay_ms(uint32_t ms)
{
	uint32_t i = 84 / 4 * 1000 * ms;
	while(i--);
}

/**
 * @brief:按键初始化
 * @note:KEY---PC13  低电平表示按键按下
 */
void key_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	//开GPIOB时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	
	//工作模式配置	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	
}

/**
 * @brief：按键扫描函数
 * @return：按下的按键
 * @note:有按键按下返回按下的按键，没有返回0       
 *       消抖,防止按下一次检测到很多次，加标志位。     
 */
uint8_t key_scan(void)
{
	static uint8_t key_flag = 0;
	if((!KEY) || key_flag == 0)
	{
		delay_ms(10);
		if(!KEY)
		{
			key_flag = 1;
			return 1;
		}
	}
	else
	{
		key_flag = 0;
	}
	return 0;
}

