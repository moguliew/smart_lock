#ifndef _W25Q64_H
#define _W25Q64_H

#include "stm32f4xx.h"

#define W25Q64_CS_H (GPIO_SetBits(GPIOA, GPIO_Pin_4))
#define W25Q64_CS_L (GPIO_ResetBits(GPIOA, GPIO_Pin_4))


void w25q64_init(void);
void write_status(void);
void w25q64_read(u32 inner_addr,u32 num_byte,u8 *str);
u8 w25q64_page_write(u32 inner_addr,u8 num_byte,u8 *str);
void w25q64_sector_erase(u32 inner_addr);
void w25q64_block_erase(u32 inner_addr);
void w25q64_chip_erase(void);
void w25q64_blocks(u32 inner_addr,u8 blocks);



void w25q64_spi_init(void);
u8 w25q64_Spi_bayte(u8 data);


void zk_updata(void);

#endif



