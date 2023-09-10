#include "stm32f4xx.h"                  // Device header
#include "lcd.h"
#include "tim.h"
#include "font.h"
#include "w25q64.h"

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
 * 函数名    ：lcd_circle
 * 函数功能  ：LCD屏幕画圆函数
 * 函数参数  ：r--半径
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
 * 函数名    ：lcd_solid_circle
 * 函数功能  ：LCD屏幕画圆函数实心圆
 * 函数参数  ：r--半径 
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








/**
 * 函数名    ：lcd_show_16ch
 * 函数功能  ：LCD屏幕显示16*16的字符
 * 函数参数  ：
 * 函数返回值：无
 * 函数描述  ：8*16		逐行式		字模一行1byte，一共16byte
 */
void lcd_show_16ch(uint16_t x,uint16_t y,uint8_t ch,uint16_t f_color,uint16_t b_color)
{
	uint8_t n,i,j;
	uint8_t temp;
	
	/*计算要显示的字符与空格的偏移值 n*/
	n = ch - ' ';
	
	/*显示*/
	for(i=0;i<16;i++)     //子一共占的行数
	{
		//将要显示的字符的模数据取出来放到一个变量里
		temp = asc16[n*16+i];
		for(j=0;j<8;j++)   //每行遍历像素点个数
		{
			if(temp & (0x80 >> j))
			{
				lcd_draw_point(x+j,y+i,f_color);
			}
			else   //背景色
			{
				lcd_draw_point(x+j,y+i,b_color);
			}
			
		}
	}
}



/*
 * 函数名    ：lcd_show_32ch
 * 函数功能  ：LCD屏幕显示32*32的字符
 * 函数参数  ：
 * 函数返回值：无
 * 函数描述  ：16 * 32		逐行式		字模一行2byte，一共64byte	
*/
void lcd_show_32ch(uint16_t x,uint16_t y,uint8_t ch,uint16_t f_color,uint16_t b_color)
{
	uint8_t n,i,j;
	uint16_t temp;
	
	/*计算要显示的字符与空格的偏移值 n*/
	n = ch - ' ';
	
	/*显示*/
	for(i=0;i<32;i++)
	{
		//将要显示的字符的模数据取出来放到一个变量里
		temp = (asc32[n*64+2*i] << 8)  |  asc32[n*64+2*i+1];
		      
		for(j=0;j<16;j++)
		{
			if(temp & (0x8000 >> j))
			{
				lcd_draw_point(x+j,y+i,f_color);
			}
			else    //背景色
			{
				lcd_draw_point(x+j,y+i,b_color);
			}
			
		}
	}
}



/**
 * 函数名    ：lcd_show_16hz
 * 函数功能  ：LCD屏幕显示16*16的汉字
 * 函数参数  ：
 * 函数返回值：无
 * 函数描述  ：16 * 16		逐行式		字模一行2byte，一共32byte
 *						
 */
void lcd_show_16hz(uint16_t x,uint16_t y,uint8_t *str,uint16_t f_color,uint16_t b_color)
{
	uint8_t n = 0;
	uint8_t i,j;
	uint16_t temp;
	
	/*计算要显示的字符与空格的偏移值 n*/
	while(1)
	{
		if(*str==table[2*n] && *(str+1)==table[2*n+1])
		{
			break;
		}
		n++;
	}
	//n就是要显示的汉字与第一个汉纸的偏移值
	
	
	/*显示*/
	for(i=0;i<16;i++)
	{
		//将要显示的字符的模数据取出来放到一个变量里
		temp = hz16[n*32+2*i]<<8 | hz16[n*32+2*i+1];
		for(j=0;j<16;j++)
		{
			if(temp & (0x8000 >> j))
			{
				lcd_draw_point(x+j,y+i,f_color);
			}
			else  //背景色
			{
				lcd_draw_point(x+j,y+i,b_color);
			}
			
		}
	}
}


/**
 * 函数名    ：lcd_show_32hz
 * 函数功能  ：LCD屏幕显示32*32的汉字
 * 函数参数  ：
 * 函数返回值：无
 * 函数描述  ：32 * 32		逐行式		字模一行4byte，一共128byte 						
 */
void lcd_show_32hz(uint16_t x,uint16_t y,uint8_t *str,uint16_t f_color,uint16_t b_color)
{
	uint8_t n = 0;
	uint8_t i,j;
	uint32_t temp;
	
	/*计算要显示的字符与空格的偏移值 n*/
	while(1)
	{
		if(*str==table[2*n] && *(str+1)==table[2*n+1])
		{
			break;
		}
		n++;
	}
	//n就是偏移值
	
	
	/*显示*/
	for(i=0;i<32;i++)
	{
		//将要显示的字符的模数据取出来放到一个变量里
		temp = hz32[n*128+4*i] << 24  | 
		       hz32[n*128+4*i+1] <<16 |
		       hz32[n*128+4*i+2] << 8 |
					 hz32[n*128+4*i+3];
		for(j=0;j<32;j++)
		{
			if(temp & (0x80000000 >> j))
			{
				lcd_draw_point(x+j,y+i,f_color);
			}
			else  //背景色
			{
				lcd_draw_point(x+j,y+i,b_color);
			}
			
		}
	}
}



/**
 * 函数名    ：lcd_show_16
 * 函数功能  ：LCD屏幕显示16*16的汉字和字符混合
 * 函数参数  ：uint16_t x,uint16_t y,uint16_t color,uint8_t *str,uint8_t mode,uint16_t b_color  
 * 函数返回值：无
 * 函数描述  ：
 */
void lcd_show_16(uint16_t x,uint16_t y,uint8_t *str,uint16_t f_color,uint16_t b_color)
{
	while(*str != '\0')
	{
		//判断是字符
		if(*str <= 127)
		{
			lcd_show_16ch(x,y,*str,f_color,b_color);
			str++;
			x+=8;
			if(x>=LCD_W-8)    //换行
			{
				x=0;
				y+=16;
			}
		}
		//判断是汉字
		else
		{
			lcd_show_16hz(x,y,str,f_color,b_color);
			str+=2;
			x+=16;
			if(x>=LCD_W-16)
			{
				x=0;
				y+=16;
			}
		}
		
	}
	
}




/**
 * 函数名    ：lcd_show_32
 * 函数功能  ：LCD屏幕显示32*32的汉字和字符混合
 * 函数参数  ：
 * 函数返回值：无
 * 函数描述  ：			
 */
void lcd_show_32(uint16_t x,uint16_t y,uint8_t *str,uint16_t f_color,uint16_t b_color)
{
	while(*str != '\0')
	{
		//判断是字符
		if(*str <= 127)
		{
			lcd_show_32ch(x,y,*str,f_color,b_color);
			str++;
			x+=16;
			if(x>=LCD_W-16)    //换行
			{
				x=0;
				y+=32;
			}
		}
		//判断是汉字
		else
		{
			lcd_show_32hz(x,y,str,f_color,b_color);
			str+=2;
			x+=32;
			if(x>=LCD_W-32)
			{
				x=0;
				y+=32;
			}
		}
		
	}
	
}






/**
 * 函数名    ：lcd_show_pic
 * 函数功能  ：LCD屏幕显示图片
 * 函数参数  ：
 * 函数返回值：无
 * 函数描述  ：				
 */
void lcd_show_pic(uint16_t x,uint16_t y,const uint8_t *pic)
{
	
	uint32_t i;
	HEADCOLOR *pic_head;     				//结构体指针
	uint16_t *p;
	
	//把图片的头数据拿出来
	pic_head = (HEADCOLOR *)pic;
	//计算图片一共有多少个像素点
	i = (pic_head->w )  * (pic_head->h);
	
	//用指针p去拿数组中的颜色数据
	p = (uint16_t *)(pic + sizeof(HEADCOLOR));  //把头数据偏移
	
	/*显示*/
	/*设定清屏范围*/
	//发送横坐标命令 ：0x2a
	st7789vm_write_command(0x2a);
	//发送命令参数   ：     起始坐标     终点坐标
	st7789vm_write_data(x);
	st7789vm_write_data(x+(pic_head->w)-1);
	//发送纵坐标命令 ：0x2b
	st7789vm_write_command(0x2b);
	//发送命令参数   ：     起始坐标     终点坐标
	st7789vm_write_data(y);
	st7789vm_write_data(y+(pic_head->h)-1);
	
	/*设定清屏颜色*/
	//发送颜色命令   ：0x2c
	st7789vm_write_command(0x2c);
	while(i)
	{
		st7789vm_write_data(*p);
		i--;
		p++;
	}
	
}








/*******************************************************************字库******************************************************************/
/**
 * 函数名    ：lcd_flash_show_16ch
 * 函数功能  ：LCD屏幕显示16*16的字符
 * 函数参数  ：uint16_t x,uint16_t y,uint16_t color,uint16_t b_color,uint8_t ch,uint8_t mode
 * 函数返回值：无
 * 函数描述  ：烧录地址：0x000000	
 *			 ASC16.bin             0x00000000             
 *			 ASC32.bin             0x00000806             
 *			 HZK16.bin             0x0000280C             
 *			 HZK32.bin             0x00042652 
 */
void lcd_flash_show_16ch(uint16_t x,uint16_t y,uint8_t ch,uint16_t f_color,uint16_t b_color)
{
	uint8_t n,i,j;
	uint8_t temp;
	uint8_t buf[16] = {0};
	
	/*计算要显示的字符与0的偏移值 n*/
	n = ch - 0;
	/*从w25q64中将要显示的字符的字模数据全部读出来 16个*/
	w25q64_read(BASE_ADDR + ASC16_ADDR + n*16,16,buf);
	
	
	/*显示*/
	for(i=0;i<16;i++)
	{
		//将要显示的字符的模数据取出来放到一个变量里
		temp = buf[i];
		for(j=0;j<8;j++)
		{
			if(temp & (0x80 >> j))
			{
				lcd_draw_point(x+j,y+i,f_color);
			}
			else//背景颜色
			{
				lcd_draw_point(x+j,y+i,b_color);
			}
			
		}
	}
}




/**
 * 函数名    ：lcd_flash_show_32ch
 * 函数功能  ：LCD屏幕显示32*32的字符
 * 函数参数  ：uint16_t x,uint16_t y,uint16_t color,uint16_t b_color,uint8_t ch 
 * 函数返回值：无
 * 函数描述  ：烧录地址：0x000000	
				 ASC16.bin             0x00000000             
				 ASC32.bin             0x00000806             
				 HZK16.bin             0x0000280C             
				 HZK32.bin             0x00042652                         
 */
void lcd_flash_show_32ch(uint16_t x,uint16_t y,uint8_t ch,uint16_t f_color,uint16_t b_color)
{
	uint8_t n,i,j;
	uint16_t temp;
	uint8_t buf[64] = {0};
	
	/*计算要显示的字符与空格的偏移值 n*/
	n = ch - 0;
	/*从w25q64中将要显示的字符的字模数据全部读出来 16个*/
	w25q64_read(BASE_ADDR + ASC32_ADDR + n*64,64,buf);
	
	/*显示*/
	for(i = 0;i < 32;i++)
	{
		//将要显示的字符的模数据取出来放到一个变量里
		temp = buf[2*i] << 8 | buf[2*i+1];
		for(j=0;j<16;j++)
		{
			if(temp & (0x8000 >> j))
			{
				lcd_draw_point(x+j,y+i,f_color);
			}
			else//背景颜色
			{
				lcd_draw_point(x+j,y+i,b_color);
			}
			
		}
	}
}




/**
 * 函数名    lcd_flash_show_16hz
 * 函数功能  ：LCD屏幕显示16*16的汉字
 * 函数参数  ：uint16_t x,uint16_t y,uint16_t color,uint16_t b_color,uint8_t *str
 * 函数返回值：无
 * 函数描述  ：烧录地址：0x000000	
			 ASC16.bin             0x00000000             
			 ASC32.bin             0x00000806             
			 HZK16.bin             0x0000280C             
			 HZK32.bin             0x00042652  
 */
void lcd_flash_show_16hz(uint16_t x,uint16_t y,uint8_t *str,uint16_t f_color,uint16_t b_color)
{
	uint32_t n;
	uint8_t buf[32] = {0};
	uint16_t temp;
	uint8_t i,j;
	
	/*计算要显示的汉字与第一个汉字的偏移量*/
	n = (*str - 0xA1) * 94 +  (*(str + 1) - 0xA1);
	
	/*从w25q64中读出要显示的汉字的字模数据放在一个数组中*/
	w25q64_read(BASE_ADDR + HZK16_ADDR + n*32,32,buf);

	/*打点显示*/
	for(i = 0;i < 16;i++)
	{
		//把每一行的数据取出来 2个字节
		temp = buf[2*i] << 8 | buf[2*i+1];
		for(j = 0;j < 16;j++)
		{
			if(temp & (0x8000 >> j))
			{
				lcd_draw_point(x+j,y+i,f_color);
			}
			else
			{
				lcd_draw_point(x+j,y+i,b_color);
			}
		}
		
	}
	
}




/**
 * 函数名    ：lcd_flash_show_32hz
 * 函数功能  ：LCD屏幕显示32*32的汉字
 * 函数参数  ：uint16_t x,uint16_t y,uint16_t color,uint16_t b_color,uint8_t *str,uint8_t mode
 * 函数返回值：无
 * 函数描述  ：烧录地址：0x000000	
				 ASC16.bin             0x00000000             
				 ASC32.bin             0x00000806             
				 HZK16.bin             0x0000280C             
				 HZK32.bin             0x00042652    
 */
void lcd_flash_show_32hz(uint16_t x,uint16_t y,uint8_t *str,uint16_t f_color,uint16_t b_color)
{
	uint32_t n;
	uint8_t buf[128] = {0};
	uint32_t temp;
	uint8_t i,j;
	
	/* 计算要显示的汉字的偏移量 */
	n = (*str - 0xA1) * 94 +  (*(str + 1) - 0xA1);
	
	/*从w25q64中读出要显示的汉字的字模数据放在一个数组中*/
	w25q64_read(BASE_ADDR + HZK32_ADDR + n*128,128,buf);

	/*打点显示*/
	for(i = 0;i < 32;i++)
	{
		//把每一行的数据取出来 2个字节
		temp = buf[4*i]<<24 | buf[4*i+1]<<16 | buf[4*i+2]<<8 | buf[4*i+3];
		for(j = 0;j < 32;j++)
		{
			if(temp & (0x80000000 >> j))
			{
				lcd_draw_point(x+j,y+i,f_color);
			}
			else
			{
				lcd_draw_point(x+j,y+i,b_color);
			}
		}
		
	}
}



/**
 * 函数名    ：lcd_flash_show_16
 * 函数功能  ：LCD屏幕显示16*16的汉字和字符混合
 * 函数参数  ：uint16_t x,uint16_t y,uint16_t color,uint8_t *str,uint8_t mode 
 * 函数返回值：无
 * 函数描述  ：				
 */
void lcd_flash_show_16(uint16_t x,uint16_t y,uint8_t *str,uint16_t f_color,uint16_t b_color)
{
	while(*str != '\0')
	{
		//判断是字符
		if(*str <= 127)
		{
			lcd_flash_show_16ch(x,y,*str,f_color,b_color);
			str++;
			x += 8;
			if(x >= LCD_W-8)//换行
			{
				x = 0;
				y += 16;
			}
		}
		//判断是汉字
		else
		{
			lcd_flash_show_16hz(x,y,str,f_color,b_color);
			str += 2;
			x += 16;
			if(x >= LCD_W-16)
			{
				x = 0;
				y += 16;
			}
		}
		
	}
	
}



/**
 * 函数名    ：lcd_flash_show_32
 * 函数功能  ：LCD屏幕显示32*32的汉字和字符混合
 * 函数参数  ：uint16_t x,uint16_t y,uint16_t color,uint8_t *str,uint8_t mode 
 * 函数返回值：无
 * 函数描述  ：				
 */
void lcd_flash_show_32(uint16_t x,uint16_t y,uint8_t *str,uint16_t f_color,uint16_t b_color)
{
	while(*str != '\0')
	{
		//判断是字符
		if(*str <= 127)
		{
			lcd_flash_show_32ch(x,y,*str,f_color,b_color);
			str++;
			x += 16;
			if(x >= LCD_W-16)//换行
			{
				x = 0;
				y += 32;
			}
		}
		//判断是汉字
		else
		{
			lcd_flash_show_32hz(x,y,str,f_color,b_color);
			str += 2;
			x += 32;
			if(x >= LCD_W-32)
			{
				x = 0;
				y += 32;
			}
		}
		
	}
	
}











