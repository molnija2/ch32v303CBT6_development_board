/*
 * lcd_text.c
 *
 *  Created on: Oct 21, 2023
 *      Author: sap
 */

#include <stdint.h>
#include <string.h>

#include "lcd.h"

#include "fonts/font_Nimb_Regular_14_22_hx.h"
#include "fonts/font_Nimb_Regular_Oblique_16_21_hx.h"
#include "fonts/font_Nimb_Bold_15_21_hx.h"
#include "fonts/font_Nimb_Bold_Oblique_16_21_hx.h"



#include "fonts/font_Nimb_Bold_Oblique_15_20_hx.h"
#include "fonts/font_Nimb_Bold_Oblique_13_18_hx.h"

#include "fonts/font_Deja_Book_15_27_hx.h"
#include "fonts/font_Deja_Bold_15_27_hx.h"
#include "fonts/font_Deja_Oblique_16_27_hx.h"
#include "fonts/font_Deja_Bold_Oblique_16_27_hx.h"


#include "fonts/font_Deja_Book_10_19_hx.h"
#include "fonts/font_Deja_Oblique_10_18_hx.h"
#include "fonts/font_Deja_Bold_11_19_hx.h"
#include "fonts/font_Deja_Bold_Oblique_11_19_hx.h"


#include "fonts/font_Droi_Regular_8_15_hx.h"
#include "fonts/font_Deja_Book_9_16_hx.h"
#include "fonts/font_Deja_Oblique_9_16_hx.h"
#include "fonts/font_Deja_Bold_9_15_hx.h"
#include "fonts/font_Deja_Bold_Oblique_9_15_hx.h"


#include "fonts/font_Nimb_Regular_Oblique_12_16_hx.h"


#include "fonts/font_Nimb_Regular_7_12_hx.h"
#include "fonts/font_Nimb_Regular_Oblique_8_12_hx.h"
#include "fonts/font_Nimb_Bold_8_11_hx.h"
#include "fonts/font_Nimb_Bold_Oblique_9_11_hx.h"





typedef struct{
	uint16_t Height ;
	uint16_t Width ;
	uint16_t Style ;
	uint16_t Dir ;
	char name[64] ;
	uint16_t *DATA ;
} FONT_INFO;


static FONT_INFO FontsBase[10] , FontTmp ;
static FONT_INFO *CurrentFont ;

static int iFontsNumber = 0 ;


static int CurrentDrawX, CurrentDrawY, CurrentColor, CurrentTextBkColor ;


static unsigned char iDecodeRusTable[256] = {
    0x0,  0x1,  0x2,  0x3,  0x4,  0x5,  0x6,  0x7,
    0x8,  0x9,  0xA,  0xB,  0xC,  0xD,  0xE,  0xF,
    0x10,  0x11,  0x12,  0x13,  0x14,  0x15,  0x16,  0x17,
    0x18,  0x19,  0x1A,  0x1B,  0x1C,  0x1D,  0x1E,  0x1F,
    0x20,  0x21,  0xDD,  0x23,  0x24,  0x25,  0x3F,  0xFD,
    0x28,  0x29,  0x2A,  0x2B,  0xE1,  0x2D,  0xFE,  0x2E,
    0x30,  0x31,  0x32,  0x33,  0x34,  0x35,  0x36,  0x37,
    0x38,  0x39,  0xC6,  0xE6,  0xC1,  0x3D,  0xDE,  0x2C,
    0x22,  0xD4,  0xC8,  0xD1,  0xC2,  0xD3,  0xC0,  0xCF,
    0xD0,  0xD8,  0xCE,  0xCB,  0xC4,  0xDC,  0xD2,  0xD9,
    0xC7,  0xC9,  0xCA,  0xDB,  0xC5,  0xC3,  0xCC,  0xD6,
    0xD7,  0xCD,  0xDF,  0xF5,  0x5C,  0xFA,  0x3A,  0x5F,
    0xB8,  0xF4,  0xE8,  0xF1,  0xE2,  0xF3,  0xE0,  0xEF,
    0xF0,  0xF8,  0xEE,  0xEB,  0xE4,  0xFC,  0xF2,  0xF9,
    0xE7,  0xE9,  0xEA,  0xFB,  0xE5,  0xE3,  0xEC,  0xF6,
    0xF7,  0xED,  0xFF,  0xD5,  0x7C,  0xDA,  0xA8,  0x0,
  } ;





void RegisterNewFont(uint16_t Height, uint16_t Width,
		uint16_t Style, uint16_t Dir, char name[32], uint16_t *DATA )
{
	FONT_INFO *CurrentFont = &FontsBase[iFontsNumber] ;

	CurrentFont->Height = Height ;
	CurrentFont->Width = Width ;
	CurrentFont->Style = Style ;
	CurrentFont->Dir = Dir ;
	strcpy(CurrentFont->name, name) ;
	CurrentFont->DATA = DATA ;

	iFontsNumber++ ;
}


void lcd_RegisterFonts()
{
	int i, j ;

	CurrentFont = &FontsBase[0] ;
	iFontsNumber = 0 ;


	/*RegisterNewFont(27, 15, 0, 0, "DejaVu Sans" , (uint16_t*)(font_Deja_Book_15_27_hx));
	RegisterNewFont(27, 15, 0, 0, "DejaVu Sans" , (uint16_t*)(font_Deja_Bold_15_27_hx));
	RegisterNewFont(27, 16, 1,0,"DejaVu Sans", (uint16_t*)(font_Deja_Oblique_16_27_hx)) ;
	RegisterNewFont(27, 16, 1,0,"DejaVu Sans", (uint16_t*)(font_Deja_Bold_Oblique_16_27_hx)) ;*/

	RegisterNewFont(22, 14, 0, 0, "Nimbus Mono L",(uint16_t*)(font_Nimb_Regular_14_22_hx)) ;
	RegisterNewFont(21, 16, 1, 0, "Nimbus Mono L",(uint16_t*)(font_Nimb_Regular_Oblique_16_21_hx)) ;
	RegisterNewFont(21, 15, 2, 0, "Nimbus Mono L",(uint16_t*)(font_Nimb_Bold_15_21_hx)) ;
	RegisterNewFont(21, 16, 3, 0, "Nimbus Mono L",(uint16_t*)(font_Nimb_Bold_Oblique_16_21_hx)) ;


	/*RegisterNewFont(19,10,0,0,"DejaVu Sans",(uint16_t*)(font_Deja_Book_10_19_hx)) ;
	RegisterNewFont(18,10,1,0,"DejaVu Sans",(uint16_t*)(font_Deja_Oblique_10_18_hx)) ;
	RegisterNewFont(19,11,0,0,"DejaVu Sans",(uint16_t*)(font_Deja_Bold_11_19_hx)) ;
	RegisterNewFont(19,11,1,0,"DejaVu Sans",(uint16_t*)(font_Deja_Bold_Oblique_11_19_hx)) ;
*/
	//RegisterNewFont(16,9,0,0,"DejaVu Sans",(uint16_t*)(font_Deja_Book_9_16_hx)) ;
	//RegisterNewFont(16,9,1,0,"DejaVu Sans",(uint16_t*)(font_Deja_Oblique_9_16_hx)) ;
	//RegisterNewFont(15,9,2,0,"DejaVu Sans",(uint16_t*)(font_Deja_Bold_9_15_hx)) ;
	//RegisterNewFont(15,9,3,0,"DejaVu Sans",(uint16_t*)(font_Deja_Bold_Oblique_9_15_hx)) ;

	RegisterNewFont(12, 7, 0, 0, "Nimbus Mono L",(uint16_t*)(font_Nimb_Regular_7_12_hx)) ;
	RegisterNewFont(12, 8, 1, 0, "Nimbus Mono L",(uint16_t*)(font_Nimb_Regular_Oblique_8_12_hx)) ;
	RegisterNewFont(11, 8, 2, 0, "Nimbus Mono L",(uint16_t*)(font_Nimb_Bold_8_11_hx)) ;
	RegisterNewFont(11, 9, 3, 0, "Nimbus Mono L",(uint16_t*)(font_Nimb_Bold_Oblique_9_11_hx)) ;

	for(i=0;i<iFontsNumber;i++)
	{
		for(j=i+1;j<iFontsNumber;j++)
			if(FontsBase[i].Height > FontsBase[j].Height)
		{
			FontTmp = FontsBase[i] ;
			FontsBase[i] = FontsBase[j] ;
			FontsBase[j] = FontTmp ;
		}
	}

	CurrentFont = &FontsBase[0] ;

	CurrentColor = 65535 ;
	CurrentTextBkColor = 0 ;
}


int lcd_getFontsNumber()
{
	return iFontsNumber ;
}


void lcd_setFont(int i)
{

	if(i<0) i=0;
	if(i>= iFontsNumber) i = iFontsNumber-1 ;
	CurrentFont = &FontsBase[i] ;
}




void lcd_setFontName(char *family,int iHeight, int istyle)
{
	int i = 0 ;

	while((i<iFontsNumber) && ((iHeight>FontsBase[i].Height)||(istyle!=FontsBase[i].Style)))
	{

		i++ ;
	}

	if(i>= iFontsNumber) i = iFontsNumber-1 ;
	CurrentFont = &FontsBase[i] ;
}



void lcd_DisplaySymbol(int x,int y, int i)
{
    int ix, iy;
    int ih,iw ;
    int iSymbStep ;

    ih = CurrentFont->Height ;
    iw = CurrentFont->Width ;


    if(CurrentFont->Dir==0)
    {
        iSymbStep = ih ;

        LCD_Address_Set(x,y,x+iw-1,y+ih-1);

        LCD_Send_Start() ;

        for(iy=0;iy<ih;iy++)
    		for(ix=0;ix<iw;ix++)
    	{
    		//if( (CurrentFont->DATA[i*iSymbStep + iy] & (1<<(16-ix)))!=0)
    		//	LCDPutPixel(ix+x,iy+y, CurrentColor) ;
    		if( (CurrentFont->DATA[i*iSymbStep + iy] & (1<<(16-ix)))!=0)
    			LCD_SendDataForFast( CurrentColor ) ;
    			//LCD_WR_DATA(CurrentColor);
    		else
    			LCD_SendDataForFast( CurrentTextBkColor) ;
    			//LCD_WR_DATA(CurrentTextBkColor);
    	}

        LCD_Send_Stop() ;
     }
     else
     {
        iSymbStep = iw ;

        LCD_Address_Set(x,y,x+iw-1,y+ih-1);

        LCD_Send_Start() ;

        for(iy=0;iy<ih;iy++)
            for(ix=0;ix<iw;ix++)
        {
            //if( (CurrentFont->DATA[i*iSymbStep + iw-1-ix] & (1<<(16-iy)))!=0)
            //	LCDPutPixel(x+iw-1-ix,y+ih-1-iy, CurrentColor) ;
            if( (CurrentFont->DATA[i*iSymbStep + iw-1-ix] & (1<<(16-iy)))!=0)
    			LCD_SendDataForFast( CurrentColor ) ;
    			//LCD_WR_DATA(CurrentColor);
    		else
    			LCD_SendDataForFast( CurrentTextBkColor) ;
        }

        LCD_Send_Stop() ;
    }

    CurrentDrawX = x + iw ;
	CurrentDrawY = y ;
}



void lcd_DisplaySymbolRus(int x,int y, int i )
{
	if(i<128) i = iDecodeRusTable[i] & 0xff ;

	lcd_DisplaySymbol( x, y, i) ;
}





void lcd_PutString(int x, int y, char *str, int color)
{
	unsigned int i ;

	CurrentColor = color ;

    while(*str!=0)
    {
        i = *(unsigned char *)str ;

        lcd_DisplaySymbol(x,y, i&0xff ) ;

        x += CurrentFont->Width ;

        str++ ;
    }
}


void lcd_PutStringRus(int x, int y, char *str, int color)
{
	unsigned int i ;

	CurrentColor = color ;

    while(*str!=0)
    {
        i = *(unsigned char *)str ;

        lcd_DisplaySymbolRus(x,y, i&0xff ) ;

        x += CurrentFont->Width ;

        str++ ;
    }
}





void lcd_gotoxy(int x, int y)
{
    CurrentDrawX = x ;
	CurrentDrawY = y ;
}

int lcd_getx()
{
	return CurrentDrawX ;
}

int lcd_gety()
{
	return CurrentDrawY ;
}

int lcd_getFontHeight()
{
	return CurrentFont->Height ;
}

int lcd_getFontWidth()
{
	return CurrentFont->Width ;
}

int lcd_getTextWidth(char *str)
{
	return strlen(str) * lcd_getFontWidth() ;
}


void lcd_setcolor(int i)
{
	CurrentColor = i ;
}

void lcd_setBkcolor(int i)
{
	CurrentTextBkColor = i ;
}


