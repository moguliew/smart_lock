#include "stm32f4xx.h"                  // Device header
#include "lcd.h"
#include "tim.h"

/**
 * 函数名    ：lcd_spi_io_init
 * 函数功能  ：模拟SPI的管脚初始函数
 * 函数参数  ：无
 * 函数返回值：无
 * 函数描述  ：LCD_CLK----------------------------PB13     
 *			  LCD_SDA----------------------------PB15        
 */
void lcd_spi_io_init(void)
{
	//开时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	//工作模式配置
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Pin =  GPIO_Pin_13 | GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Fast_Speed;
	GPIO_Init(GPIOB, &GPIO_InitStruct);	
	
}

/**
 * 函数名    ：lcd_io_init
 * 函数功能  ：LCD的管脚初始函数
 * 函数参数  ：无 
 * 函数返回值：无
 * 函数描述  ： LEDK-----PB1        背光灯引脚        //通用输出
 *			RESET----PB10        复位引脚          //通用输出
 *			LCD_CS---PB12        片选引脚          //通用输出	
 *			LCD_D/C--PB14        数据命令选择线    //通用输出		  
 */
void lcd_io_init(void)
{
	//GPIO初始化
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//1.开时钟
	
	//推挽输出
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin =  GPIO_Pin_1 | GPIO_Pin_10 | GPIO_Pin_12 | GPIO_Pin_14;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStruct);	
	
	//初始状态
	LCD_LED_OFF;               //背光关
	LCD_CS_H;                  //片选拉高
}



/**
 * 函数名    ：lcd_spi_byte
 * 函数功能  ：SPI传输一个字节函数
 * 函数参数  ：无
 * 函数返回值：无
 * 函数描述  ：    
 */
void lcd_spi_byte(uint8_t data)
{
	uint8_t i;
	//空闲
	LCD_SPI_SCL_L;
	LCD_MOSI_H;
	
	/*传输数据*/
	for(i=0;i<8;i++)
	{
		LCD_SPI_SCL_L;    					
		if(data & 0x80) 
			LCD_MOSI_H; 
		else 
			LCD_MOSI_L;
		LCD_SPI_SCL_H;   
		data = data << 1;
	}	
}



/*
 * 函数名    ：st7789vm_write_command
 * 函数功能  ：对LCD写命令函数
 * 函数参数  ：uint8_t cmd
 * 函数返回值：无
 * 函数描述  ：    
 */
void st7789vm_write_command(uint8_t cmd)
{
	LCD_CS_L;			//先拉低片选
	LCD_CMD;			//表示发送的是命令
	lcd_spi_byte(cmd);	//具体发送的8位 命令
	LCD_CS_H;			//拉高片选
}


/*
 * 函数名    ：st7789vm_write_command_parameter
 * 函数功能  ：对LCD写命令参数函数
 * 函数参数  ：uint8_t cmd_data
 * 函数返回值：无
 * 函数描述  ：    
 */
void st7789vm_write_command_parameter(uint8_t cmd_data)
{
	LCD_CS_L;					//先拉低片选
	LCD_DATA;					//表示发送的是数据
	lcd_spi_byte(cmd_data);		//具体发送的8位 命令参数
	LCD_CS_H;					//拉高片选
}



/**
 * 函数名    ：st7789vm_write_data
 * 函数功能  ：对LCD写数据函数
 * 函数参数  ：uint16_t data
 * 函数返回值：无
 * 函数描述  ：坐标值，颜色值   
 */
void st7789vm_write_data(uint16_t data)
{
	LCD_CS_L;					//先拉低片选
	LCD_DATA;					//表示发送的是数据
	lcd_spi_byte(data>>8);		//具体发送的8位 命令参数
	lcd_spi_byte(data & 0x00ff);
	LCD_CS_H;					//拉高片选
}

/**
 * 函数名    ：st7789vm_rest
 * 函数功能  ：对LCD复位函数
 * 函数参数  ：无
 * 函数返回值：无
 * 函数描述  ：  
 */
void st7789vm_rest(void)
{
	LCD_RES_L;
	tim5_delay_ms(100);
	LCD_RES_H;
	//Tim5_delay_ms(100);
}









/**
 * 函数名    ：LCD_init
 * 函数功能  ：对LCD初始函数
 * 函数参数  ：无
 * 函数返回值：无
 * 函数描述  ：  
 */
void lcd_init(void)
{
	//LCD_IO初始化
	lcd_io_init();
	//SPI_IO初始化
	lcd_spi_io_init();
	//LCD复位
	st7789vm_rest();
	//IC驱动代码移植
	/* start initial sequence */ 
	st7789vm_write_command(0x36);
	st7789vm_write_command_parameter(0x00);

	st7789vm_write_command(0x3A); 
	st7789vm_write_command_parameter(0x05);

	st7789vm_write_command(0xB2);
	st7789vm_write_command_parameter(0x0C);
	st7789vm_write_command_parameter(0x0C);
	st7789vm_write_command_parameter(0x00);
	st7789vm_write_command_parameter(0x33);
	st7789vm_write_command_parameter(0x33); 

	st7789vm_write_command(0xB7); 
	st7789vm_write_command_parameter(0x35);  

	st7789vm_write_command(0xBB);
	st7789vm_write_command_parameter(0x19);

	st7789vm_write_command(0xC0);
	st7789vm_write_command_parameter(0x2C);

	st7789vm_write_command(0xC2);
	st7789vm_write_command_parameter(0x01);

	st7789vm_write_command(0xC3);
	st7789vm_write_command_parameter(0x12);   

	st7789vm_write_command(0xC4);
	st7789vm_write_command_parameter(0x20);  

	st7789vm_write_command(0xC6); 
	st7789vm_write_command_parameter(0x0F);    

	st7789vm_write_command(0xD0); 
	st7789vm_write_command_parameter(0xA4);
	st7789vm_write_command_parameter(0xA1);

	st7789vm_write_command(0xE0);
	st7789vm_write_command_parameter(0xD0);
	st7789vm_write_command_parameter(0x04);
	st7789vm_write_command_parameter(0x0D);
	st7789vm_write_command_parameter(0x11);
	st7789vm_write_command_parameter(0x13);
	st7789vm_write_command_parameter(0x2B);
	st7789vm_write_command_parameter(0x3F);
	st7789vm_write_command_parameter(0x54);
	st7789vm_write_command_parameter(0x4C);
	st7789vm_write_command_parameter(0x18);
	st7789vm_write_command_parameter(0x0D);
	st7789vm_write_command_parameter(0x0B);
	st7789vm_write_command_parameter(0x1F);
	st7789vm_write_command_parameter(0x23);

	st7789vm_write_command(0xE1);
	st7789vm_write_command_parameter(0xD0);
	st7789vm_write_command_parameter(0x04);
	st7789vm_write_command_parameter(0x0C);
	st7789vm_write_command_parameter(0x11);
	st7789vm_write_command_parameter(0x13);
	st7789vm_write_command_parameter(0x2C);
	st7789vm_write_command_parameter(0x3F);
	st7789vm_write_command_parameter(0x44);
	st7789vm_write_command_parameter(0x51);
	st7789vm_write_command_parameter(0x2F);
	st7789vm_write_command_parameter(0x1F);
	st7789vm_write_command_parameter(0x1F);
	st7789vm_write_command_parameter(0x20);
	st7789vm_write_command_parameter(0x23);

	st7789vm_write_command(0x21); 
	st7789vm_write_command(0x11); 
	st7789vm_write_command(0x29); 
	
	//开背光
	LCD_LED_ON;
	//清屏
	lcd_clear(0x0000);
}




/**
 * 函数名    ：lcd_clear
 * 函数功能  ：清屏
 * 函数参数  ：uint16_t color
 * 函数返回值：无
 * 函数描述  ：  
 */
void lcd_clear(uint16_t color)
{
	/*确定区域*/
	//确定x方向
	st7789vm_write_command(0x2A);     //横坐标命令
	//起始横坐标
	st7789vm_write_data(0);     
	//结束横坐标
	st7789vm_write_data(LCD_W-1);
	
	//确定Y方向
	st7789vm_write_command(0x2B);     //纵坐标命令
	//起始纵坐标
	st7789vm_write_data(0);
	//结束纵坐标
	st7789vm_write_data(LCD_H-1);
	
	/*确定颜色*/
	st7789vm_write_command(0x2C);     //颜色命令
	for(uint32_t i=0;i<LCD_W*LCD_H;i++)
	{
		st7789vm_write_data(color);
	}
}




/**
 * 函数名    ：LCD_xy_clear
 * 函数功能  ：清某个区域
 * 函数参数  ：uint16_t x,uint16_t y,uint16_t w,uint16_t h,uint16_t color
 * 函数返回值：无
 * 函数描述  ：  
 */
void lcd_xy_clear(uint16_t x,uint16_t y,uint16_t w,uint16_t h,uint16_t color)
{
	/*确定区域*/
	//确定x方向
	st7789vm_write_command(0x2A);     //横坐标命令
	//起始横坐标
	st7789vm_write_data(x);     
	//结束横坐标
	st7789vm_write_data(x+w-1);
	
	//确定Y方向
	st7789vm_write_command(0x2B);     //纵坐标命令
	//起始纵坐标
	st7789vm_write_data(y);
	//结束纵坐标
	st7789vm_write_data(y+h-1);
	
	
	/*确定颜色*/
	st7789vm_write_command(0x2C);     //颜色命令
	for(uint32_t i=0;i<w*h;i++)
	{
		st7789vm_write_data(color);
	}
}








/**
 * 函数名    ：lcd_draw_point
 * 函数功能  ：打点函数
 * 函数参数  ：uint16_t x,uint16_t y,uint16_t color
 * 函数返回值：无
 * 函数描述  ：  
 */
void lcd_draw_point(uint16_t x,uint16_t y,uint16_t color)
{
	/*确定区域*/
	//确定x方向
	st7789vm_write_command(0x2A);     //横坐标命令
	//起始横坐标
	st7789vm_write_data(x);     
	//结束横坐标
	st7789vm_write_data(x);
	
	//确定Y方向
	st7789vm_write_command(0x2B);     //纵坐标命令
	//起始纵坐标
	st7789vm_write_data(y);
	//结束纵坐标
	st7789vm_write_data(y);
	
	
	/*确定颜色*/
	st7789vm_write_command(0x2C);     //颜色命令
	st7789vm_write_data(color);
	
}


/**
 * 函数名    ：LCD_Ring
 * 函数功能  ：LCD屏幕画圆函数
 * 函数参数  ：uint16_t x,uint16_t y,uint16_t r,uint16_t color 
 * 函数返回值：无
 * 函数描述  ：
 *		     （x - x0）^2 + (y - y0)^2  == r*r 
 */
void lcd_circle(uint16_t x,uint16_t y,uint16_t r,uint16_t color)
{
	uint16_t i,j;
	
	for(i=x-r;i<=x+r;i++)         //遍历横坐标
	{
		for(j=y-r;j<=y+r;j++)       //遍历纵坐标
		{
			if((i-x)*(i-x) + (j-y)*(j-y) >= (r-3)*(r-3) && (i-x)*(i-x) + (j-y)*(j-y) <= r*r)   //判断遍历的点是否符合条件
			{
				//打点函数
				lcd_draw_point(i,j,color); //将遍历到的点打出来
			}
		}
	}
}


/**
 * 函数名    ：LCD_Ring_shi
 * 函数功能  ：LCD屏幕画圆函数实心圆
 * 函数参数  ：uint16_t x,uint16_t y,uint16_t r,uint16_t color 
 * 函数返回值：无
 * 函数描述  ：
 *			（x - x0）^2 + (y - y0)^2  <= r*r 
 */
void lcd_solid_circle(uint16_t x,uint16_t y,uint16_t r,uint16_t color)
{
	uint16_t i,j;
	
	for(i=x-r;i<=x+r;i++)         //遍历横坐标
	{
		for(j=y-r;j<=y+r;j++)       //遍历纵坐标
		{
			if((i-x)*(i-x) + (j-y)*(j-y) <= r*r)   //判断遍历的点是否符合条件
			{
				//打点函数
				lcd_draw_point(i,j,color); //将遍历到的点打出来
			}
		}
	}
}














