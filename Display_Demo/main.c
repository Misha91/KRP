#include "stm32f4xx_conf.h"
#include "lcd.h"
#include "usb.h"
#include <stdio.h>


//extern sFONT *LCD_Currentfonts;

void clk_init()
{
  /* Clock Setup - see Clock tree in reference manual - Figure 9.   */
  RCC_HSEConfig(RCC_HSE_ON);
  RCC_HCLKConfig(RCC_SYSCLK_Div1);
  RCC_PCLK1Config(RCC_HCLK_Div4);
  RCC_PCLK2Config(RCC_HCLK_Div2);
  RCC_PLLConfig(RCC_PLLSource_HSE, 25, 336, 2, 7);
  RCC_PLLCmd(ENABLE);
  while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == 0) ;
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
  RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_OTG_FS, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

}

int main()
{
  clk_init();
  
  LCD_Init();
  //LCD_SetFont(&Font16x24);  
  LCD_SetFont(&Font8x8);
  //timer_init();
  usb_init();     

  
  while(1);  
  
  return 0;
}


void timer_init(void)
{
 
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  
  /* Timer Init */
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  TIM_TimeBaseStructure.TIM_Period = 46875;
  TIM_TimeBaseStructure.TIM_Prescaler = 102;//1024
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
  
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
   
  TIM_Cmd(TIM2, ENABLE);
  
}

void TIM2_IRQHandler(void)
{
  static uint8_t tmp = 0;

  if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    
    int c;
    char buffer [50];
    c=sprintf (buffer, "String: %d, %#04x", tmp, tmp);     
    LCD_printHex(2,0,tmp);
    LCD_printNum(3,0,tmp);
    LCD_printLine(5,0, buffer, c);
    
    tmp++;
  }

  return;
}

