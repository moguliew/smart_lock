#ifndef _W25Q64_H
#define _W25Q64_H

#include "stm32f4xx.h"

#define W25Q64_CS_H (GPIO_SetBits(GPIOA, GPIO_Pin_4))
#define W25Q64_CS_L (GPIO_ResetBits(GPIOA, GPIO_Pin_4))


void w25q64_init(void);
void write_status(void);
void w25q64_read(uint32_t inner_addr,uint32_t num_byte,uint8_t *str);
uint8_t w25q64_page_write(uint32_t inner_addr,uint8_t num_byte,uint8_t *str);
void w25q64_sector_erase(uint32_t inner_addr);
void w25q64_block_erase(uint32_t inner_addr);
void w25q64_chip_erase(void);
void w25q64_blocks(uint32_t inner_addr,uint8_t blocks);


void w25q64_spi_init(void);
uint8_t w25q64_spi_byte(uint8_t data);




#endif



