#ifndef _AT24C04_H
#define _AT24C04_H


#include "stm32f4xx.h" 



//������ַ�궨��
#define AT24C04_ADDR0_W  0xa0    //��һ��
#define AT24C04_ADDR0_R  0xa1    //��һ��
#define AT24C04_ADDR1_W  0xa2    //�ڶ���
#define AT24C04_ADDR1_R  0xa3    //�ڶ���


#define AT24C04_NO_ERR 0   //Ӧ��  �޴���
#define	AT24C04_ERR1   1   //��Ӧ��
#define	AT24C04_ERR2   2   //��Ӧ��
#define	AT24C04_ERR3   3   //��Ӧ��
#define	AT24C04_ERR4   4














//IICʱ���ߺ������ߺ궨��
#define IIC_SCL_H    (GPIOA->ODR |= (1 << 8))
#define	IIC_SCL_L    (GPIOA->ODR &= ~(1 << 8))

#define IIC_SDA_OUT_H   (GPIOC->ODR |= (1 << 9))
#define	IIC_SDA_OUT_L   (GPIOC->ODR &= ~(1 << 9))

#define IIC_SDA_INT     (GPIOC->IDR & (1 << 9))


#define BS8116_IIC_SCL_H    (GPIOB->ODR |= (1 << 6))
#define	BS8116_IIC_SCL_L    (GPIOB->ODR &= ~(1 << 6))

#define BS8116_IIC_SDA_OUT_H   (GPIOB->ODR |= (1 << 7))
#define	BS8116_IIC_SDA_OUT_L   (GPIOB->ODR &= ~(1 << 7))

#define BS8116_IIC_SDA_INT     (GPIOB->IDR & (1 << 7))
#define BS8116_IIC_SCL_INT     (GPIOB->IDR & (1 << 6))



void iic_IO_init(void);
void iic_star(void);
void iic_stop(void);
void iic_send_ack(u8 ack);
u8 iic_get_ack(void);
void iic_send_byte(u8 data);
u8 iic_read_byte(void);




void at24c04_init(void);
u8 AT24C04_write_byte(u16 inner_addr,u8 data);
u8 AT24C04_read_byte(u16 inner_addr,u8 *data);
u8 AT24C04_write_page(u16 inner_addr,u8 num,u8 *str);
u8 AT24C04_read_bytes(u16 inner_addr,u8 num,u8 *str);
u8 AT24C04_write_bytes(u16 inner_addr,u8 num,u8 *str);







#endif

