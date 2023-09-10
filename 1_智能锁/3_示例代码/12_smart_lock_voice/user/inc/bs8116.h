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


void bs8116_init(void);
uint8_t bs8116_read_key(void);
uint8_t bs8116_write_register(uint8_t *cmd_buff);
uint8_t bs8116_read_register(uint8_t *buff);
uint8_t bs8116_key_scan(void);

/*************IIC***************/
void bs8116_iic_io_init(void);
void bs8116_iic_start(void);
void bs8116_iic_stop(void);
void bs8116_iic_send_ack(uint8_t ack);
uint8_t bs8116_iic_get_ack(void);
void bs8116_iic_send_byte(uint8_t data);
uint8_t bs8116_iic_read_byte(void);





#endif




