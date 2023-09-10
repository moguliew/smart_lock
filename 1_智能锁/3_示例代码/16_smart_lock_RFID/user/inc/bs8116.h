#ifndef _BS8116_H
#define _BS8116_H

#include "stm32f4xx.h" 




#define BS8116_IIC_SCL_H    (GPIOB->ODR |= (1 << 6))
#define	BS8116_IIC_SCL_L    (GPIOB->ODR &= ~(1 << 6))

#define BS8116_IIC_SDA_OUT_H   (GPIOB->ODR |= (1 << 7))
#define	BS8116_IIC_SDA_OUT_L   (GPIOB->ODR &= ~(1 << 7))

#define BS8116_IIC_SDA_INT     (GPIOB->IDR & (1 << 7))

#define BS8116_IIC_SCL_INT     (GPIOB->IDR & (1 << 6))


#define BS8116ADDR_W (0x50<<1)
#define BS8116ADDR_R (0x50<<1 | 0x01)

#define BS8116_IRQ (GPIOA->IDR & (1<<1))


void BS8116_init(void);
uint8_t BS8116_ReadKey(void);
u8 BS8116_Writeregister(u8 *cmd_buff);
u8 BS8116_Readregister(u8 *buff);
u8 BS8116_Key_scan(void);




void bs8116_iic_IO_init(void);
void bs8116_iic_star(void);
void bs8116_iic_stop(void);
void bs8116_iic_send_ack(u8 ack);
u8 bs8116_iic_get_ack(void);
void bs8116_iic_send_byte(u8 data);
u8 bs8116_iic_read_byte(void);



void open_passward(u8 bs8116_key);



#endif




