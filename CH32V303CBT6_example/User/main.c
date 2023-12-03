/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Mi croelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 *@Note
 USART Print debugging routine:
 USART1_Tx(PA9).
 This example demonstrates using USART1(PA9) as a print debug port output.

*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>


//#include <core_riscv.h>


#include "lcd.h"
#include "lcd_text.h"

#include "debug.h"

#include "ch32v30x_adc.h"

//#define M_PI (3.1415927f)
//#define M_PI_2 (M_PI/2.0f)
#define M_PI_M_2 (M_PI*2.0f)


/* Global typedef */

/* Global define */

/* Global Variable */


int16_t calibration_value;
int myCount=0; //increments every 1ms
u8 i = 0;
uint16_t vdata[238];


#define KEY_ENTER	GPIO_Pin_3
#define KEY_ESCAPE	GPIO_Pin_7
#define KEY_UP		GPIO_Pin_6
#define KEY_DOWN	GPIO_Pin_12
#define KEY_LEFT	GPIO_Pin_11
#define KEY_RIGHT	GPIO_Pin_2

#define DEF_KEYBOARD_MASK	0x018cc



void PIO_Keyboard_Init()
{
	GPIO_InitTypeDef  GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3
    		| GPIO_Pin_6 | GPIO_Pin_7
			| GPIO_Pin_11 | GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    //GPIO_AFIODeInit();
}



uint16_t getKey()
{
	return ~(GPIO_ReadInputData(GPIOA)) & DEF_KEYBOARD_MASK ;
}




void ADC_initialization()
{


    ADC_InitTypeDef InitStruct = { 0 } ;

    // First let's give the ADC some clock
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_ADC1, ENABLE ); // Clock to ADC1 - both ADCs are hanging off of the APB2 bus
    RCC_ADCCLKConfig(RCC_PCLK2_Div4); // What is PCLK2 - 144 MHz / 4 = 36 MHz?

	ADC_DeInit(ADC1) ;

    InitStruct.ADC_Mode = ADC_Pga_1 | ADC_Mode_Independent;  // No fancy injection stuff for now
    InitStruct.ADC_ScanConvMode = DISABLE;       // Just sample one
    InitStruct.ADC_ContinuousConvMode = DISABLE; // And just do it once
    InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // We will ASK when needed
    InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
    InitStruct.ADC_NbrOfChannel = 1;


	ADC_Init(ADC1, &InitStruct) ;
    ADC_Cmd(ADC1, ENABLE);

    // Let's run a self calibration on the ADC
    ADC_BufferCmd(ADC1, DISABLE);   // disable buffer
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1));
    calibration_value = Get_CalibrationValue(ADC1);
    ADC_BufferCmd(ADC1, ENABLE);   // reenable buffer

    ADC_TempSensorVrefintCmd(ENABLE);

	/*ADC_DiscModeCmd(ADC1, ENABLE) ;
	ADC_SoftwareStartConvCmd(ADC1, ENABLE) ;*/
}


unsigned int getADC(int ch)
{
    ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_28Cycles5);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);

    uint32_t cnt = 0;
	//ADC_SoftwareStartConvCmd(ADC1, ENABLE) ;
	//ADC_Cmd(ADC1, ENABLE) ;

	while( ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)==0) ++cnt;

	return ADC_GetConversionValue(ADC1)&0x0fff;
}


u16 Get_ADC_Average(u16 ch, u16 times) {
    u32 temp_val = 0;
    u16 t;
    u16 val;

    for (t = 0; t < times; t++) {
        temp_val += getADC(ch);
    }

    val = temp_val / times;

    return val;
}




u16 Get_ADC_Dispersion(u16 ch, u32 times) {

    u16 t;
    u16 val, val1;
    float dv1 = 0, dv2 = 0 ;

    for (t = 0; t < times; t++) {
    	val1 = getADC(ch);
        dv1 += val1 ;
        dv2 += val1 * val1 ;
    }

    dv1 = dv1 / times;
    dv2 = dv2 / times;

    dv1 = dv1*dv1 ;
    dv1 = sqrt(dv2-dv1) ;

    val = dv1 ;
    return val;
}


u16 Get_ADC_Value(u16 ch, u32 times) {

    u16 t;
    u16 val, val1;
    float dv1 = 0 ;

    for (t = 0; t < times; t++) {
    	val1 = Dispersion(ch,100) ;
        dv1 += val1 ;
        //dv2 += val1 * val1 ;
    }

    dv1 = dv1 / times;
    /*dv2 = dv2 / times;

    dv1 = dv1*dv1 ;
    dv1 = sqrt(dv2-dv1) ;*/

    val = dv1 ;
    return val;
}




void LedInit()
{
	//GPIO_DeInit(GPIOB) ;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE) ;
	GPIO_InitTypeDef  GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15 ;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA, GPIO_Pin_15) ;
}




void TIM3_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
extern void TIM3_Init( uint16_t arr, uint16_t psc );
void toggleLED();

//initializes timer 3
void TIM3_Init( uint16_t arr, uint16_t psc )
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = { 0 };
    NVIC_InitTypeDef NVIC_InitStructure = { 0 };

    /* Enable timer3 clock */
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM3, ENABLE );

    /* Initialize timer3 */
    TIM_TimeBaseStructure.TIM_Period = arr;
    TIM_TimeBaseStructure.TIM_Prescaler = psc;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit( TIM3, &TIM_TimeBaseStructure );

    /* Enable updating timer3 interrupt */
    TIM_ITConfig( TIM3, TIM_IT_Update, ENABLE );

    /* Configure timer3 interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init( &NVIC_InitStructure );

    /* Enable timer3 */
    TIM_Cmd( TIM3, ENABLE );

    /* Enable timer3 interrupt */
    NVIC_EnableIRQ( TIM3_IRQn );
}


// this function will be executed every 1ms
void TIM3_IRQHandler( void )
{
    if( TIM_GetITStatus( TIM3, TIM_IT_Update ) != RESET )
    {
        /* Clear interrupt flag */
        TIM_ClearITPendingBit( TIM3, TIM_IT_Update );

        if (myCount>500) { // if count>500ms, blink led
            myCount=0;
            toggleLED();
        }
        myCount++;
    }
}

// sets IO pins
void GPIO_Toggle_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}


//toggle LED IO pin
void toggleLED() {
    GPIO_WriteBit(GPIOA, GPIO_Pin_15, (i == 0) ? (i = Bit_SET) : (i = Bit_RESET));
}

//3d functions prototypes
float myround(float p);

int compare_float(float f1, float f2);
/*float cos(float x);
float sin(float x);*/


#define ANGLE_RANGE  (M_PI/6)

void DrawScale(float dValue, int color, int redraw) ;



uint16_t Rgb(uint16_t r, uint16_t g, uint16_t b)
{

	r >>= 3 ;
	g >>= 2 ;
	b >>= 3 ;
	return (r<<11) | (g<<5) | b ;
}



/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{

    RCC_ClocksTypeDef RCC_ClocksStatus={0};


    /*NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    Delay_Init();
    USART_Printf_Init(115200);
    printf("SystemClk:%d\r\n", SystemCoreClock);

    printf("This is printf example\r\n");

    while(1)
    {
    }*/


    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    Delay_Init();
    Delay_Ms(100);
    USART_Printf_Init(115200);
    printf("SystemClk:%ld\r\n", SystemCoreClock);

    SystemCoreClockUpdate();
    printf("SystemClk:%d\r\n",SystemCoreClock);

    RCC_GetClocksFreq(&RCC_ClocksStatus);
    printf("SYSCLK_Frequency-%d\r\n", RCC_ClocksStatus.SYSCLK_Frequency);
    printf("HCLK_Frequency-%d\r\n", RCC_ClocksStatus.HCLK_Frequency);
    printf("PCLK1_Frequency-%d\r\n", RCC_ClocksStatus.PCLK1_Frequency);
    printf("PCLK2_Frequency-%d\r\n", RCC_ClocksStatus.PCLK2_Frequency);

    /* NOT NEEDED printf("GPIO Toggle TEST\r\n");
    GPIO_Toggle_init();*/

    lcd_init();

    GPIO_Toggle_init();

    TIM3_Init( 9, SystemCoreClock / 10000 - 1 ); // timer will counts MSs
    // since real frequency will be = SystemCoreClock/ (10000/(9+1))


    LCD_Fill_Fast(0,0,240,240,BLACK); // clear screen black

    lcd_RegisterFonts() ;

    lcd_gotoxy(0,8);


    lcd_setFontName("Nimbus Mono L",20,FONT_STYLE_ITALIC);

    char* textMeasure2 = "Delightfulness" ;
    lcd_PutString((240-lcd_getTextWidth(textMeasure2))/2, 160,
    		textMeasure2, RED);

    int j ;
	for(i=0;i<50;i++)
		for(j=0;j<239;j++)
			LCDPutPixel(j,239-i,Rgb(255-i*5,i*5,(j*2)/3+60));


    DrawScale(0,GREEN,1) ;

    float dValue = 0.0, dValue_prev = 0.0 ;


    ADC_initialization() ;

    PIO_Keyboard_Init() ;

    uint16_t key ;

    uint16_t uiDispMode = 0 ;
    for(i=0;i<240;i++) vdata[i] = 0 ;

    while(1)
    {


    	key = getKey() ;

    	if(key !=0)
    	{
    		while(getKey() !=0 ) Delay_Ms(100);

    		switch(key)
    		{
    		case KEY_ENTER : uiDispMode++ ;
    			if(uiDispMode>=2) uiDispMode=0 ;
    			break ;
    		default : ;
    		}

    		LCD_Fill_Fast(0,0,239,239,BLACK); // clear screen black

    		lcd_setFontName("Nimbus Mono L",20,FONT_STYLE_ITALIC);
    	    lcd_PutString((240-lcd_getTextWidth(textMeasure2))/2, 210,
    	    		textMeasure2, RED);


    		if(uiDispMode == 0)
    		{
    			LCD_Fill_Fast(0,0,239,200,BLACK);
    			int i,j ;

    			for(i=0;i<50;i++)
     				for(j=0;j<239;j++)
    					LCDPutPixel(j,239-i,Rgb(255-i*5,i*5,(j*2)/3+60));

    			DrawScale(dValue_prev, BLACK,1) ;

    			lcd_setFontName("Nimbus Mono L",20,FONT_STYLE_ITALIC);
        	    lcd_PutString((240-lcd_getTextWidth(textMeasure2))/2, 160,
        	    		textMeasure2, RED);


    		}
    		else if(uiDispMode == 1)
    		{
    			LCD_Fill_Fast(0,0,239,200,WHITE);
    			lcd_setFontName("Nimbus Mono L",20,FONT_STYLE_ITALIC);
    			lcd_PutString((240-lcd_getTextWidth(textMeasure2))/2, 210,
    			    			    		    		textMeasure2, RED);
    		}
    	}





    	if(uiDispMode == 0)
    	{
    		DrawScale(dValue, GREEN,0) ;

    		Delay_Ms(10);

    		dValue_prev = dValue ;
    		//dValue += (M_PI/6)/20 ;

    		//dValue = getADC(ADC_Channel_2) ;

    		dValue = Get_ADC_Average(ADC_Channel_0,2000) ;

     		dValue = (dValue *(M_PI/3)) /0x0fff;

    		if(dValue>(M_PI/3) )
    		{
    			dValue = M_PI/3 ;
    		}
     		DrawScale(dValue_prev, BLACK,0) ;
    	}
    	else if(uiDispMode == 1)
    	{
    		uint32_t Value, Value_prev;
    		uint16_t  vmin,vmax, amp, v ;
    		uint16_t i ;

    		Delay_Ms(10);

    		vmin = 4095 ;
    		vmax = 0 ;


    		for(i=0;i<238;i++)
    		{
    			Delay_Us(200);
    			v = (sin(M_PI*i/20)*0.8+1.0)*0x8ff;//getADC(ADC_Channel_0) ;
    			vdata[i] = v ;
    			if(vmin>v) vmin=v ;
    			if(vmax<v) vmax=v ;
    		}

    		amp = vmax-vmin ;
    		if(amp<10) amp = 10 ;

    		LCDDrawLine(0, 199, 239, 199, WHITE) ;

    		Value_prev = (vdata[0]*180)/4096 ;

    		for(i=2;i<239;i++)
    		{
    			Value = (vdata[i-1]*180)/4096 ;

    			LCDDrawLine(i-1,198-Value_prev,i,198-Value, BLUE) ;

    			Value_prev = Value ;
    		}

    	}


    }

}





//
// 3D functions
//

float myround(float p)
{ // some weird mounriver studio bug doesn't let us use math.h round()
    return (int)(p);
}





int compare_float(float f1, float f2)
{
 float precision = 0.00000000000000000001;
 if ((f1 - precision) < f2)
  {
 return -1;
  }
 else if ((f1 + precision) > f2)
 {
  return 1;
 }
 else
  {
 return 0;
  }
}


// we have to have our own sin/cos functions because mounRiver studio doesn't let us use the ones in math.h
/*float cos(float x){
 if( x < 0.0f )
  x = -x;

  if (0 <= compare_float(x,M_PI_M_2))
 {
 do {
  x -= M_PI_M_2;
  }while(0 <= compare_float(x,M_PI_M_2));

  }

  if ((0 <= compare_float(x, M_PI)) && (-1 == compare_float(x, M_PI_M_2)))
  {
   x -= M_PI;
   return ((-1)*(1.0f - (x*x/2.0f)*( 1.0f - (x*x/12.0f) * ( 1.0f - (x*x/30.0f) * (1.0f - (x*x/56.0f )*(1.0f - (x*x/90.0f)*(1.0f - (x*x/132.0f)*(1.0f - (x*x/182.0f)))))))));
  }
 return 1.0f - (x*x/2.0f)*( 1.0f - (x*x/12.0f) * ( 1.0f - (x*x/30.0f) * (1.0f - (x*x/56.0f )*(1.0f - (x*x/90.0f)*(1.0f - (x*x/132.0f)*(1.0f - (x*x/182.0f)))))));
}



float sin(float x){return cos(x-M_PI_2);}
*/




void DrawScale(float dValue, int color, int iredraw)
{
	int ColorLines1 = WHITE, textcolor = BLUE ;
	int iCenterX  = 240 /2 ;
	int iCenterY = 230 ;
	int iybgn = 0, iyend = 20, iyend5 = 25, iyend10 = 35, iyendTmp ;
	int a1,a2, c1,c2, a3 ;
	float  sincos ;
	float dAngle = -ANGLE_RANGE, dAngleStep = ANGLE_RANGE/20, dAngleEnd = ANGLE_RANGE;
	int icount5, icount10 ;
	int iValue = 0 ;
	char cBuffer[10] ;

	/*int i,j ;

	for(i=0;i<50;i++)
		//LCDDrawLine (0, 180+i, 239, 180+i, Rgb(255-i*12,i*12,255));
		for(j=0;j<239;j++)
			LCDPutPixel(j,155+i,Rgb(255-i*5,i*5,(j*2)/3+60));*/

	lcd_setFont(2) ;

	a1 = iCenterY - iybgn ;

	icount5 = 5 ;
	icount10 = 10 ;

	/*if(color!=BLACK)
	{
		dAngleEnd = dAngle ;

		while(dAngleEnd <= (dValue - ANGLE_RANGE) ) dAngle += dAngleStep ;

	}*/

	if(iredraw)
	while(dAngle <= dAngleEnd )
	{
		sincos = sin(-dAngle)/cos(dAngle) ;

		iyendTmp = iyend ;

		if(icount5 == 5)
		{
			iyendTmp = iyend5 ;
			icount5 = 0 ;
		}
		if(icount10 == 10)
		{
			iyendTmp = iyend10 ;
			icount10 = 0 ;

			a2 = iCenterY - iyendTmp ;
			c2 = sincos * a2 ;

			sprintf(cBuffer,"%d",iValue) ;
			lcd_PutStringRus(iCenterX-c2-(lcd_getTextWidth(cBuffer)/2)-5,
					iyendTmp, cBuffer, textcolor);
			iValue += 100 ;
		}
		icount5++ ;
		icount10++ ;

		a2 = iCenterY - iyendTmp ;

		c1 = sincos * a1 ;
		c2 = sincos * a2 ;

		LCDDrawLine (iCenterX-c1, iybgn, iCenterX-c2, iyendTmp, ColorLines1);

		dAngle += dAngleStep ;
	}


	dAngle = dValue - ANGLE_RANGE ;

	sincos = sin(-dAngle)/cos(dAngle) ;

	iyendTmp = 150 ;

	iybgn += 50 ;

	a3 = iCenterY - iybgn ;
	//a3 = cos(dAngle)*a3 ;

	a2 = iCenterY - iyendTmp ;

	c1 = sincos * a3 ;
	c2 = sincos * a2 ;

	LCDDrawLine (iCenterX-c1, iCenterY-a3, iCenterX-c2, iyendTmp, color);
	LCDDrawLine (iCenterX-c1+1, iCenterY-a3, iCenterX-c2+1, iyendTmp, color);

}


