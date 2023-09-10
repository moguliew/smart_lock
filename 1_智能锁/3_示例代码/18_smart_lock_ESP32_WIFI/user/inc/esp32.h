#ifndef _ESP32_H
#define _ESP32_H
#include "stm32f4xx.h"                  // Device header

void usart2_esp32_Init(void);
void usart2_esp32_send_str(u8 *data);


u8 Esp32_SendandReceive(u8 *cmd,u8 *respond,u32 wait_ms);
void Exit_Cipmode(void);
u8 ESP32_Restore(void);
void Esp32_Init(void);
u8 Esp32_Wificonnect(u8 *user,u8*password);
u8 Esp32_Wificlose(void);
u8 Esp32_TCPconnect(u8 *remote_ip,u32 com);



#endif


