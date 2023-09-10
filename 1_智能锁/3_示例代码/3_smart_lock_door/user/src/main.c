#include "led.h" 
#include "key.h"
#include "door.h"

static void delay_ms(uint32_t ms)
{
	uint32_t i = 84 / 4 * 1000 * ms;
	while(i)
	{
		i--;
	}
}


int main(void)
{
	uint8_t key;
	uint8_t door_flag = 0;
	
	key_init();
	door_init();
	
	while(1)
	{	
		key = key_scan();
		if(key == 1)
		{
			door_flag  = !door_flag;
			
			if(door_flag == 1)
			{
				open_door();
			}
			else
			{
				close_door();
			}	
		}
	}
	
	return 0;
}

















