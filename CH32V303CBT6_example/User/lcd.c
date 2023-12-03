#include <math.h>
#include <stdint.h>


#include "debug.h"
#include "stdint.h"
#include "lcd.h"
#include "font8x8_basic.h"
#include "string.h"

//#define SPI_GPIO
#define SPI_HW

#define USE_HORIZONTAL 0


#define LCD_W 240
#define LCD_H 240

#define WHITE            0xFFFF
#define BLACK            0x0000
#define BLUE             0x001F
#define BRED             0XF81F
#define GRED                   0XFFE0
#define GBLUE                  0X07FF
#define RED              0xF800
#define MAGENTA          0xF81F
#define GREEN            0x07E0
#define CYAN             0x7FFF
#define YELLOW           0xFFE0
#define BROWN                0XBC40 //×ØÉ«
#define BRRED                0XFC07 //×ØºìÉ«
#define GRAY                 0X8430 //»ÒÉ«
#define DARKBLUE         0X01CF //ÉîÀ¶É«
#define LIGHTBLUE        0X7D7C //Ç³À¶É«
#define GRAYBLUE         0X5458 //»ÒÀ¶É«
#define LIGHTGREEN       0X841F //Ç³ÂÌÉ«
#define LGRAY                0XC618 //Ç³»ÒÉ«(PANNEL),´°Ìå±³¾°É«
#define LGRAYBLUE        0XA651 //Ç³»ÒÀ¶É«(ÖÐ¼ä²ãÑÕÉ«)
#define LBBLUE           0X2B12 //Ç³×ØÀ¶É«(Ñ¡ÔñÌõÄ¿µÄ·´É«)

#define LCD_SCLK_Clr() GPIO_ResetBits(GPIOB,GPIO_Pin_13)//SCL=SCLK
#define LCD_SCLK_Set() GPIO_SetBits(GPIOB,GPIO_Pin_13)

#define LCD_MOSI_Clr() GPIO_ResetBits(GPIOB,GPIO_Pin_15)//SDA=MOSI
#define LCD_MOSI_Set() GPIO_SetBits(GPIOB,GPIO_Pin_15)

#define LCD_RES_Clr()  GPIO_ResetBits(GPIOB,GPIO_Pin_11)//RES
#define LCD_RES_Set()  GPIO_SetBits(GPIOB,GPIO_Pin_11)

#define LCD_DC_Clr()   GPIO_ResetBits(GPIOB,GPIO_Pin_10)//DC
#define LCD_DC_Set()   GPIO_SetBits(GPIOB,GPIO_Pin_10)

#define LCD_CS_Clr()   GPIO_ResetBits(GPIOB,GPIO_Pin_12)//CS
#define LCD_CS_Set()   GPIO_SetBits(GPIOB,GPIO_Pin_12)

#define LCD_BLK_Clr()  GPIO_ResetBits(GPIOB,GPIO_Pin_9)//BLK
#define LCD_BLK_Set()  GPIO_SetBits(GPIOB,GPIO_Pin_9)


void LCD_Writ_Bus(u8 dat)
{
#if defined(SPI_GPIO)
    u8 i;
    LCD_CS_Clr();
    for(i=0;i<8;i++)
    {
        LCD_SCLK_Clr();
        if(dat&0x80)
        {
           LCD_MOSI_Set();
        }
        else
        {
           LCD_MOSI_Clr();
        }
        LCD_SCLK_Set();
        dat<<=1;
    }
  LCD_CS_Set();
#elif defined(SPI_HW)
    LCD_CS_Clr();
    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI2,dat);
    Delay_Us(1);
    LCD_CS_Set();
#endif
}

void LCD_WR_DATA8(u8 dat)
{
    LCD_Writ_Bus(dat);
}

void LCD_WR_DATA(u16 dat)
{
    LCD_Writ_Bus(dat>>8);
    LCD_Writ_Bus(dat);
}

void LCD_WR_REG(u8 dat)
{
    LCD_DC_Clr();
    LCD_Writ_Bus(dat);
    LCD_DC_Set();
}

void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2)
{
    if(USE_HORIZONTAL==0)
    {
        LCD_WR_REG(0x2a);//ÁÐµØÖ·ÉèÖÃ
        LCD_WR_DATA(x1);
        LCD_WR_DATA(x2);
        LCD_WR_REG(0x2b);//ÐÐµØÖ·ÉèÖÃ
        LCD_WR_DATA(y1);
        LCD_WR_DATA(y2);
        LCD_WR_REG(0x2c);//´¢´æÆ÷Ð´
    }
    else if(USE_HORIZONTAL==1)
    {
        LCD_WR_REG(0x2a);//ÁÐµØÖ·ÉèÖÃ
        LCD_WR_DATA(x1);
        LCD_WR_DATA(x2);
        LCD_WR_REG(0x2b);//ÐÐµØÖ·ÉèÖÃ
        LCD_WR_DATA(y1+80);
        LCD_WR_DATA(y2+80);
        LCD_WR_REG(0x2c);//´¢´æÆ÷Ð´
    }
    else if(USE_HORIZONTAL==2)
    {
        LCD_WR_REG(0x2a);//ÁÐµØÖ·ÉèÖÃ
        LCD_WR_DATA(x1);
        LCD_WR_DATA(x2);
        LCD_WR_REG(0x2b);//ÐÐµØÖ·ÉèÖÃ
        LCD_WR_DATA(y1);
        LCD_WR_DATA(y2);
        LCD_WR_REG(0x2c);//´¢´æÆ÷Ð´
    }
    else
    {
        LCD_WR_REG(0x2a);//ÁÐµØÖ·ÉèÖÃ
        LCD_WR_DATA(x1+80);
        LCD_WR_DATA(x2+80);
        LCD_WR_REG(0x2b);//ÐÐµØÖ·ÉèÖÃ
        LCD_WR_DATA(y1);
        LCD_WR_DATA(y2);
        LCD_WR_REG(0x2c);//´¢´æÆ÷Ð´
    }
}


int lcd_gpio_init()
{
#if defined(SPI_GPIO)
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    LCD_SCLK_Set();

#elif defined(SPI_HW)
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    SPI_InitTypeDef  SPI_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

    /* DC (PB10) & RESET (PB11) & CS (PB12) */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    //GPIO_SetBits(GPIOA, GPIO_Pin_2);

    /* SPI_CLK */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* SPI_MISO */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* SPI_MOSI */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4; /* APB1_CLK (72MHz) */
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI2, &SPI_InitStructure);

    SPI_Cmd(SPI2, ENABLE);
#endif


    LCD_CS_Set();

    return 0;
}

void LCD_Fill_Slow(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color)
{
    u16 i,j;
    LCD_Address_Set(xsta,ysta,xend-1,yend-1);//ÉèÖÃÏÔÊ¾·¶Î§
    for(i=ysta;i<yend;i++)
    {
        for(j=xsta;j<xend;j++)
        {
            LCD_WR_DATA(color);
        }
    }
}



void LCD_Send_Start()
{
    Delay_Us(1);
	LCD_CS_Clr();

}

void LCD_Send_Stop()
{
    Delay_Us(1);
    LCD_CS_Set();
}


void LCD_SendDataForFast(uint16_t color)
{
	 while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
	            SPI_I2S_SendData(SPI2,color >> 8);
	            //Delay_Us(1);
	 while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
	            SPI_I2S_SendData(SPI2,color);

}


void LCD_Fill_Fast(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color)
{
    u16 i,j;

    LCD_Address_Set(xsta,ysta,xend,yend);//ÉèÖÃÏÔÊ¾·¶Î§


    //LCD_CS_Clr();
    LCD_Send_Start() ;

    for(i=ysta;i<yend;i++)
    {
        for(j=xsta;j<xend;j++)
        {
            //LCD_WR_DATA(color);

            while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
            SPI_I2S_SendData(SPI2,color >> 8);
            //Delay_Us(1);
            while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
            SPI_I2S_SendData(SPI2,color);
        	//LCD_SendDataForFast(color) ;
        }
    }

    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);

    //Delay_Us(1);
    //LCD_CS_Set();
    LCD_Send_Stop() ;

}



void LCD_ShowPicture_Slow(u16 x,u16 y,u16 length,u16 width,const u8 pic[])
{
    u16 i,j;
    u32 k=0;
    LCD_Address_Set(x,y,x+length-1,y+width-1);
    for(i=0;i<length;i++)
    {
        for(j=0;j<width;j++)
        {
            LCD_WR_DATA8(pic[k*2]);
            LCD_WR_DATA8(pic[k*2+1]);
            k++;
        }
    }
}

void LCD_ShowPicture_Fast(u16 x,u16 y,u16 length,u16 width,const u8 pic[])
{
    u16 i,j;
    u32 k=0;
    LCD_Address_Set(x,y,x+length-1,y+width-1);

    LCD_CS_Clr();

    for(i=0;i<length;i++)
    {
        for(j=0;j<width;j++)
        {
            //LCD_WR_DATA8(pic[k*2]);
            //LCD_WR_DATA8(pic[k*2+1]);

            while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
            SPI_I2S_SendData(SPI2,pic[k*2]);
            //Delay_Us(1);
            while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
            SPI_I2S_SendData(SPI2,pic[k*2+1]);

            k++;
        }
    }

    LCD_CS_Set();
}

int lcd_init()
{
    lcd_gpio_init();

    //LCD_RES_Clr();//¸´Î»
    Delay_Ms(100);

    LCD_RES_Set();
    Delay_Ms(100);

    LCD_BLK_Set();//´ò¿ª±³¹â
    Delay_Ms(100);

    //************* Start Initial Sequence **********//
    LCD_WR_REG(0x11); //Sleep out
    Delay_Ms(120);              //Delay 120ms
    //************* Start Initial Sequence **********//
    LCD_WR_REG(0x36);
    if(USE_HORIZONTAL==0)LCD_WR_DATA8(0x00);
    else if(USE_HORIZONTAL==1)LCD_WR_DATA8(0xC0);
    else if(USE_HORIZONTAL==2)LCD_WR_DATA8(0x70);
    else LCD_WR_DATA8(0xA0);

    LCD_WR_REG(0x3A);
    LCD_WR_DATA8(0x05);

    LCD_WR_REG(0xB2);
    LCD_WR_DATA8(0x0C);
    LCD_WR_DATA8(0x0C);
    LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(0x33);
    LCD_WR_DATA8(0x33);

    LCD_WR_REG(0xB7);
    LCD_WR_DATA8(0x35);

    LCD_WR_REG(0xBB);
    LCD_WR_DATA8(0x32); //Vcom=1.35V

    LCD_WR_REG(0xC2);
    LCD_WR_DATA8(0x01);

    LCD_WR_REG(0xC3);
    LCD_WR_DATA8(0x15); //GVDD=4.8V  ÑÕÉ«Éî¶È

    LCD_WR_REG(0xC4);
    LCD_WR_DATA8(0x20); //VDV, 0x20:0v

    LCD_WR_REG(0xC6);
    LCD_WR_DATA8(0x0F); //0x0F:60Hz

    LCD_WR_REG(0xD0);
    LCD_WR_DATA8(0xA4);
    LCD_WR_DATA8(0xA1);

    LCD_WR_REG(0xE0);
    LCD_WR_DATA8(0xD0);
    LCD_WR_DATA8(0x08);
    LCD_WR_DATA8(0x0E);
    LCD_WR_DATA8(0x09);
    LCD_WR_DATA8(0x09);
    LCD_WR_DATA8(0x05);
    LCD_WR_DATA8(0x31);
    LCD_WR_DATA8(0x33);
    LCD_WR_DATA8(0x48);
    LCD_WR_DATA8(0x17);
    LCD_WR_DATA8(0x14);
    LCD_WR_DATA8(0x15);
    LCD_WR_DATA8(0x31);
    LCD_WR_DATA8(0x34);

    LCD_WR_REG(0xE1);
    LCD_WR_DATA8(0xD0);
    LCD_WR_DATA8(0x08);
    LCD_WR_DATA8(0x0E);
    LCD_WR_DATA8(0x09);
    LCD_WR_DATA8(0x09);
    LCD_WR_DATA8(0x15);
    LCD_WR_DATA8(0x31);
    LCD_WR_DATA8(0x33);
    LCD_WR_DATA8(0x48);
    LCD_WR_DATA8(0x17);
    LCD_WR_DATA8(0x14);
    LCD_WR_DATA8(0x15);
    LCD_WR_DATA8(0x31);
    LCD_WR_DATA8(0x34);
    LCD_WR_REG(0x21);

    LCD_WR_REG(0x29);

    Delay_Ms(100);
/*    LCD_Fill_Fast(0,0,LCD_W,LCD_H,BLUE);
    //Delay_Ms(100);
    LCD_Fill_Fast(0,0,LCD_W,LCD_H,RED);

    LCD_Fill_Fast(0,0,LCD_W,LCD_H,MAGENTA);
    LCD_Fill_Fast(0,0,LCD_W,LCD_H,CYAN);


    //LCD_ShowPicture_Slow(29,48,182,144,gImage_test);
    LCD_ShowPicture_Fast(29,48,182,144,gImage_test);
*/


    LCD_Fill_Fast(0,0,LCD_W,LCD_H,BLACK);
    return 0;
}

void LCDTextXY(int textX, int textY, char *myString, int color, int fontSize)
{

    for (int strI=0; strI<strlen(myString); strI++)
    {
        char asciiCode=myString[strI];
        int charDisp=(strI*8*fontSize); // displacement in the screen of a char in a string

        for (int  yI=0; yI<8; yI++) {
            int fontData=font8x8_basic[asciiCode][yI];

            if (fontData&1) LCD_Fill_Fast(textX+charDisp+(0*fontSize), textY+(yI*fontSize), textX+charDisp+(0*fontSize)+fontSize, textY+(yI*fontSize)+fontSize, color);
            if (fontData&2) LCD_Fill_Fast(textX+charDisp+(1*fontSize), textY+(yI*fontSize), textX+charDisp+(1*fontSize)+fontSize, textY+(yI*fontSize)+fontSize, color);
            if (fontData&4) LCD_Fill_Fast(textX+charDisp+(2*fontSize), textY+(yI*fontSize), textX+charDisp+(2*fontSize)+fontSize, textY+(yI*fontSize)+fontSize, color);
            if (fontData&8) LCD_Fill_Fast(textX+charDisp+(3*fontSize), textY+(yI*fontSize), textX+charDisp+(3*fontSize)+fontSize, textY+(yI*fontSize)+fontSize, color);
            if (fontData&16) LCD_Fill_Fast(textX+charDisp+(4*fontSize), textY+(yI*fontSize), textX+charDisp+(4*fontSize)+fontSize, textY+(yI*fontSize)+fontSize, color);
            if (fontData&32) LCD_Fill_Fast(textX+charDisp+(5*fontSize), textY+(yI*fontSize), textX+charDisp+(5*fontSize)+fontSize, textY+(yI*fontSize)+fontSize, color);
            if (fontData&64) LCD_Fill_Fast(textX+charDisp+(6*fontSize), textY+(yI*fontSize), textX+charDisp+(6*fontSize)+fontSize, textY+(yI*fontSize)+fontSize, color);
            if (fontData&128) LCD_Fill_Fast(textX+charDisp+(7*fontSize), textY+(yI*fontSize), textX+charDisp+(7*fontSize)+fontSize, textY+(yI*fontSize)+fontSize, color);
        }
    }


}



void LCDPutPixel(int X, int Y, int color) {
    if ((X>LCD_W)||(Y>LCD_H))
        return;
    LCD_Fill_Fast(X, Y, X+1, Y+1, color);
}


void LCDDrawLine(int x0, int y0, int x1, int y1, int color) {
    //https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
    int dx = abs(x1 - x0);
    int SX=0;
    if (x0<x1) {
        SX=1;
    }
    else {
        SX=-1;
    }
    int dy = -abs(y1 - y0);
    int SY;
    if (y0<y1) {
        SY=1;
    }
    else {
        SY=-1;
    }
    int error = dx + dy;

    for (;;){
        LCDPutPixel(x0, y0, color);
        if ((x0==x1)&&(y0==y1)) break;
        int e2 = 2 * error;
        if (e2 >= dy) {
            if (x0 == x1) break;
            error = error + dy;
            x0 = x0 + SX;
        }
        if (e2 <= dx) {
            if (y0 == y1) break;
            error = error + dx;
            y0 = y0 + SY;
        }
    }
}



/*void LCDDrawLine(int x1, int y1, int x2, int y2, int color)
{
// determinant
int32_t D;
// deltas
int32_t delta_x, delta_y;
// steps
int32_t trace_x = 1, trace_y = 1;

uint32_t DeltaSumm ;

// delta x
delta_x = x2 - x1;
// delta y
delta_y = y2 - y1;

// check if x2 > x1
if (delta_x < 0) {
  // negate delta x
  delta_x = -delta_x;
  // negate step x
  trace_x = -trace_x;
}

// check if y2 > y1
if (delta_y < 0) {
  // negate detla y
  delta_y = -delta_y;
  // negate step y
  trace_y = -trace_y;
}

	// Bresenham condition for m < 1 (dy < dx)
	if (delta_y < delta_x)
	{

		DeltaSumm = 0 ;
		// draw first pixel
		LCDPutPixel(x1, y1,color);
		// check if x1 equal x2
		while (x1 != x2)
		{
			// update x1
			x1 += trace_x;

			DeltaSumm += delta_y ;
			// check if determinant is positive
			if (DeltaSumm >= delta_x)
			{
				// update y1
				y1 += trace_y;
				// update determinant
				DeltaSumm -= delta_x;
			}

			// update deteminant
			//D += 2*delta_y;
			// draw next pixel
			LCDPutPixel(x1, y1, color);

		}
		// for m > 1 (dy > dx)
	} else
	{
		DeltaSumm = 0 ;

		// draw first pixel
		LCDPutPixel(x1, y1, color);
		// check if y2 equal y1
		while (y1 != y2)
		{
			// update y1
			y1 += trace_y;
			// update deteminant
			DeltaSumm += delta_x ;

			// check if determinant is positive
			if (DeltaSumm >= delta_y)
			{
				// update y1
				x1 += trace_x;
				// update determinant
				DeltaSumm -= delta_y ;
			}

			// draw next pixel
			LCDPutPixel (x1, y1, color);
		}
	}
// success return

}
*/
