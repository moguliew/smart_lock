#include "led.h" 
#include "key.h"
#include "door.h"
#include "usart.h"
#include "string.h"

static void delay_ms(uint32_t ms)
{
	uint32_t i= 84 / 4 * 1000 * ms;
	while(i)
	{
		i--;
	}
}


int main(void)
{
	usart1_init(115200);
	door_init();
	led_init();
	
	while(1)
	{	
		LED1_TOGGLE;
		delay_ms(200);
	}
	return 0;
}

















