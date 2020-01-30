#include "stm32f4xx_conf.h"
#include "lcd.h"
#include "usb.h"
#include <stdio.h>
#include "stm324xg_eval_ioe.c"
#include "stm324xg_eval.c"

uint32_t global_time_cnt = 0;
void timer_init(void);

extern uint8_t IOE_Config(void);
extern volatile uint8_t usb_configured;
extern struct report_struct report;

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
  LCD_SetFont(&Font8x8); 
  usb_init();     
  timer_init();  
  IOE_Config();
  
  STM_EVAL_PBInit(BUTTON_TAMPER, BUTTON_MODE_GPIO);
  STM_EVAL_PBInit(BUTTON_KEY, BUTTON_MODE_GPIO);
  STM_EVAL_LEDInit(LED1);
  STM_EVAL_LEDInit(LED2);
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);
  STM_EVAL_LEDOn(LED4);
  char buffer[50];
  int n=sprintf (buffer, "The device has started!"); 
  LCD_printLine(1,0, buffer, n); 
  
  n=sprintf (buffer, "BE4M38KRP Semestral Work"); 
  LCD_printLine(22,0, buffer, n); 

  while(1)
  {

  }
  
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

  TIM_TimeBaseStructure.TIM_Period = 256; 
  TIM_TimeBaseStructure.TIM_Prescaler = 2048;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
  
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
   
  TIM_Cmd(TIM2, ENABLE);
  
}

void TIM2_IRQHandler(void)
{
  static unsigned char buffer[50];
  static int n;
  static uint8_t sent_num = 0;
  static uint8_t last_was_zero = 1;
  static uint8_t last_wheel = 0;
  if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) 
  {
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
   
    global_time_cnt++;

    if (usb_configured)
    { 
      if (last_was_zero)
      {
        last_was_zero = 0;
        int n=sprintf (buffer, "USB IS CONNECTED!"); 
        LCD_printLine(4,0, buffer, n); 
        STM_EVAL_LEDOn(LED3);
      }

      if (!STM_EVAL_PBGetState(BUTTON_TAMPER)) report.but0 = 1;
      if (!STM_EVAL_PBGetState(BUTTON_KEY)) report.but1 = 1;
      
      switch(IOE_JoyStickGetState())
      {
        case JOY_SEL:   
          report.but2 = 1;        
          break;
          
        case JOY_DOWN:
          if (!report.but1)
            report.y += 1; 
          else
            report.wheel -= (sent_num % 3 == 0) ? 1 : 0;
          break;
          
        case JOY_LEFT:
          report.x -= 1;
          break;
          
        case JOY_RIGHT:
          report.x += 1;
          break;
          
        case JOY_UP:  
          if (!report.but1)
            report.y -= 1;
          else
            report.wheel += (sent_num % 3 == 0) ? 1 : 0;
          break;
          
        default: break;
      }  
      
      if (sent_num % 5 == 0)
      {
        if (report.but1 && (report.wheel != 0))
        {
          report.but1 = 0;
          last_wheel = 50;
        }
        
        if (last_wheel)
        {
          report.but1 = 0;
          last_wheel--;
        }
        buffer[0] = report.but0 | (report.but1 << 1) | (report.but2 << 2) | (report.but3 << 3);
        buffer[1] = report.x;
        buffer[2] = report.y;
        buffer[3] = report.wheel;
      
        send_data(buffer, 4, 1);
        memset(&report, 0, sizeof(struct report_struct));
        STM_EVAL_LEDToggle(LED1);
      }
      
      sent_num++;
 #ifdef DEBUG   
      n=sprintf (buffer, "%d", sent_num); 
      LCD_printLine(22,0, buffer, n);
#endif
    }
    else
    {
      if (last_was_zero == 0)
      {
        last_was_zero = 1;
        int n=sprintf (buffer, "USB IS NOT CONNECTED!"); 
        LCD_printLine(4,0, buffer, n); 
        STM_EVAL_LEDOff(LED3);
      }
    }
    if (global_time_cnt %5 == 0) STM_EVAL_LEDToggle(LED2);
  }

  return;
}

