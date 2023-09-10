#include "stm32f4xx.h"                  // Device header
#include "led.h" 
#include "key.h"


int main(void)
{
	uint8_t key;
	
	led_init();
	key_init();

	while(1)
	{	
		
		key = key_scan();
		if(key == 1)
		{
			LED1_TOGGLE ;
			LED2_TOGGLE ;
		}
		
	}
	
	return 0;
}

















