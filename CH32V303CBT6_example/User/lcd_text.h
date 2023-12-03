/*
 * lcd_text.h
 *
 *  Created on: Oct 21, 2023
 *      Author: sap
 */

#ifndef USER_LCD_TEXT_H_
#define USER_LCD_TEXT_H_

#define FONT_STYLE_NORMAL		0
#define FONT_STYLE_ITALIC		1
#define FONT_STYLE_BOLD			2
#define FONT_STYLE_BOLD_ITALIC	3



void lcd_RegisterFonts() ;
void lcd_DisplaySymbol(int x,int y, int ch);
void lcd_DisplaySymbolRus(int x,int y, int ch);
int lcd_getx();
int lcd_gety();
void lcd_gotoxy(int x, int y);

void lcd_setcolor(int i) ;
void lcd_setBkcolor(int i) ;
void lcd_PutString(int x, int y, char *str, int color);
void lcd_PutStringRus(int x, int y, char *str, int color);
int lcd_getFontHeight();
int lcd_getFontWidth();
void lcd_setFont(int i );
int lcd_getFontsNumber() ;
void lcd_setFontName(char *family,int iHeight, int istyle);
int lcd_getTextWidth(char *str);


#endif /* USER_LCD_TEXT_H_ */
