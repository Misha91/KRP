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





void OTG_FS_IRQHandler(void){
  LCD_printNum(6,0,1);
  char buffer [50];
  static int tmp = 0;
  int n, k;
  unsigned long regStorage = *( (volatile unsigned long *) OTG_FS_GINTSTS);
  
  if (regStorage & (1<<12))
  {
    
    *( (volatile unsigned long *) OTG_FS_DOEPCTL0) |= 1 << 27; //SNAK
    
    *( (volatile unsigned long *) OTG_FS_DAINTMSK) |= 1 << 0; //V
    *( (volatile unsigned long *) OTG_FS_DAINTMSK) |= 1 << 16; //V
    
    *( (volatile unsigned long *) OTG_FS_DOEPMSK) |= 1 << 3; //STUP
    *( (volatile unsigned long *) OTG_FS_DOEPMSK) |= 1 << 0; //XFRC    
    
    *( (volatile unsigned long *) OTG_FS_DIEPMSK) |= 1 << 0; //XFRCM
    *( (volatile unsigned long *) OTG_FS_DIEPMSK) |= 1 << 3; //TOC
    
   *( (volatile unsigned long *) OTG_FS_GRXFSIZ) |= 0x40; 
      
   *( (volatile unsigned long *) OTG_FS_HNPTXFSIZ) |= 0x40<<16; 
   
    *( (volatile unsigned long *) OTG_FS_DOEPTSIZ0) |= (1 << 29) | (1 << 30); 
    
    n=sprintf (buffer, "START OF RESET"); 
    LCD_printLine(15,0, buffer, n);
    *( (volatile unsigned long *) OTG_FS_GINTSTS) |= (1<<12);
    tmp++;
    //OTG_FS_GINTSTS = ???;
    //usb_reset();
  }

  if (regStorage & (1<<13))
  {
    unsigned enumSpeed = (*( (volatile unsigned long *) OTG_FS_DSTS) & 0x6) >> 1;
    *( (volatile unsigned long *) OTG_FS_DIEPCTL0) |= 0x3; 
    /*
    00: 64 bytes
    01: 32 bytes
    10: 16 bytes
    11: 8 bytes */
    n=sprintf (buffer, "END OF RESET"); 
    LCD_printLine(16,0, buffer, n);
    
    n=sprintf (buffer, "ENUM SPEED: %d", enumSpeed); 
    LCD_printLine(17,0, buffer, n);
    *( (volatile unsigned long *) OTG_FS_GINTSTS) |= (1<<13);
    
       //tmp++;
    
  }
  
  if (regStorage & (1<<4))
  {
   unsigned long pcktStatus = (*( (volatile unsigned long *) OTG_FS_GRXSTSP) & (0xF << 16)) >> 16;
   unsigned long bcnt = (*( (volatile unsigned long *) OTG_FS_GRXSTSP) & (0xFFF << 4)) >> 4;
   
   n=sprintf (buffer, "pcktSt %#08x bcnt %#08x", pcktStatus, bcnt); 
   LCD_printLine(1,0, buffer, n);
      
   unsigned long readBuffer[128];
   if (pcktStatus != 0 && bcnt != 0)
   {
     k = 0;
     while (*( (volatile unsigned long *) OTG_FS_GINTSTS) & (1<<4))
     {
       unsigned long regMskStorage = *( (volatile unsigned long *) OTG_FS_GINTMSK);
       *( (volatile unsigned long *) OTG_FS_GINTMSK) = 0;
       if (k < 128) readBuffer[k] = *( (volatile unsigned long *) (USB_BASE_ADDRESS + 0x1000));
       else *( (volatile unsigned long *) (USB_BASE_ADDRESS + 0x1000));
       //*( (volatile unsigned long *) (USB_BASE_ADDRESS + 0x1000));
       *( (volatile unsigned long *) OTG_FS_GINTMSK) = regMskStorage;
       k++;
       
     };
     
     n=sprintf (buffer, "READING IS FINISHED %d", k); 
     LCD_printLine(3,0, buffer, n);
     
     if (k != 0 && k < 128)
     {
       int c;
       for (c = 0; c < k; c++)
       {
         n=sprintf (buffer, "%d %#08x",c, readBuffer[c]); 
         LCD_printLine(c,0, buffer, n);
       }
     }
   }
   


    *( (volatile unsigned long *) OTG_FS_GINTSTS) |= (1<<4);
  }

   
   n=sprintf (buffer, "OTG_FS_GINTSTS %#08x", regStorage); 
   LCD_printLine(19,0, buffer, n);
  
   n=sprintf (buffer, "tmp %d", tmp); 
   LCD_printLine(18,0, buffer, n);
  
   return;
}






int main()
{
  clk_init();
  
  LCD_Init();
  //LCD_SetFont(&Font16x24);  
  LCD_SetFont(&Font8x8);
  //timer_init();
  usb_init();
      
  //char buffer [50];  
  //int n;
  //n=sprintf (buffer, "%d", sizeof(chars)-1); 
  //LCD_printLine(0,0, buffer, n);
  
  char chars[] = "Hello world abc!";
  LCD_printLine(0,0, chars, sizeof(chars)-1);
  
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

