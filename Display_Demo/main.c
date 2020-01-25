#include "stm32f4xx_conf.h"
#include "lcd.h"
#include "usb.h"
#include <stdio.h>


//extern sFONT *LCD_Currentfonts;

uint32_t global_time_cnt = 0;
void timer_init(void);


extern volatile uint8_t usb_configured;
extern struct report_struct report;
//extern uint8_t send_data_ep1(const uint8_t * data_buf, uint8_t length, uint8_t ep);

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
  
  usb_init();     
  timer_init();
  
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
  static char buffer[50];
  static int n;
  //static uint32_t toWrite;
  static uint8_t sent_num = 0;
  if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) 
  {
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
   
    global_time_cnt++;
    
    if (usb_configured)
    {
      //if (usb_configured < 2)
      //{
      //    usb_configured++;
      //    return;
      //}
      if (global_time_cnt)
      {
        
        //report.but0 = 1;
        //report.but1 = 1;
      //memset(&report, 255, sizeof(struct report_struct));
       /*
      toWrite = report.but0 | (report.but1 << 1) |
      (report.but2 << 2) | (report.but3 << 3) | (report.but4 << 4) |
      (report.notUsed << 5) | (report.x << 8) | (report.y << 16) |
      (report.wheel << 24);*/
        //if (sent_num == 0 || sent_num == 2)
        //{        
          report.x = 100 * (sent_num + 1);
          report.y = 100 * (sent_num + 1);
        if (sent_num == 1){
          int abc;
        }
          send_data(&report, sizeof(report), 1);
          sent_num++;
        //}
        //else if (sent_num %2 == 1)
        //{
          //send_data(0,0,1);
        //  sent_num++;
        //}
        //n=sprintf (buffer, "%d", (global_time_cnt%2 == 0) ? 0 : 1);   
        n=sprintf (buffer, "%d", sent_num); 
        LCD_printLine(22,0, buffer, n);
        //usb_configured = 0;
        memset(&report, 0, sizeof(struct report_struct));
      }
      
      
    }
  }

  return;
}

