#include "usb.h"
#include "lcd.h"


__IO struct struct_usb *usb = (struct struct_usb *) USB_BASE_ADDRESS;
//usb->reg1 = 0x00;

void usb_init(void){ 
    

  
  GPIO_InitTypeDef  GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_OTG_FS);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_OTG_FS);

  NVIC_InitStructure.NVIC_IRQChannel = OTG_FS_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  //*(OTG_FS_DCFG) = (1<<0) | (1<<1) | (1<<2); // DSPD[1:0] = 11, NZLSOHSK[2] = 1
  //*(OTG_FS_GINTMSK) = (1 << 3) | (1 << 10) | (1 << 11) | (1 << 12) | (1 << 13); 
  
  *( (volatile unsigned long *) OTG_FS_GAHBCFG) |= (1<<0); // GINTMSK = 1
  
  // Zeroing of RXFLVL

                
  //*( (volatile unsigned long *) OTG_FS_GAHBCFG) |= (1<<8); // PTXFELVL = 1
 

  
  // HNPCAP = 1, SRPCAP = 1, TRDT = 0xF TOCAL = 0x5
  *( (volatile unsigned long *) OTG_FS_GUSBCFG) |= (1<<9) | (1<<8) | (0xF << 10) | 0x5; 
  *( (volatile unsigned long *) OTG_FS_GINTSTS) = 0;
  *( (volatile unsigned long *) OTG_FS_GINTMSK) |= (1 << 1) | (1 << 2); //OTGINT = 1, mode mismatch = 1
  
  char buffer [50];
  int n;
  
 
  
  
  *( (volatile unsigned long *) OTG_FS_GUSBCFG) |= (1<<30);
  
  unsigned long tmp = 2500000;
  while(tmp--);
  //tmp = 2500000;
  //while(tmp--);
 // tmp = 2500000;
  //while(tmp--);
  //tmp = 2500000;
  //while(tmp--);
  
 unsigned char currentMode = *( (volatile unsigned long *) OTG_FS_GINTSTS) & 0x1; // should be 0
  
  *( (volatile unsigned long *) OTG_FS_DCFG) = (1<<0) | (1<<1) ; // DSPD[1:0] = 11, | (1<<2) NZLSOHSK[2] = 1
  *( (volatile unsigned long *) OTG_FS_GINTMSK) |= SOFM | RXFLVL | ESUSPM | USBSUSPM  | USBRST | ENUMDNE;//    // 
   /*    
    SOFM[3] = 1 , SOF
    RXFLVLM[4] = 1,
    ESUSPM[10] = 1, EARLY SUSPEND        
    USBSUSPM[11] =1, USB SUSPEND
    USBRST[12] = 1, USB RESET
    ENUMDNEM[13] = 1    ENUMERATION IS DONE
*/
  *( (volatile unsigned long *) OTG_FS_GCCFG) |= (1 << 16); // PWRDWN = 1
  *( (volatile unsigned long *) OTG_FS_GCCFG) |= (1 << 19) ; // VBUSBSEN = 1
  //*( (volatile unsigned long *) OTG_FS_GCCFG) |= (1 << 21);
  
   
  /*while (*( (volatile unsigned long *) OTG_FS_GINTSTS) & (1<<12)){
   
  };
  
  while (*( (volatile unsigned long *) OTG_FS_GINTSTS) & (1<<13)){
   
  };*/
   
  n=sprintf (buffer, "OTG_FS_GAHBCFG %#08x", *( (volatile unsigned long *) OTG_FS_GAHBCFG)); 
  LCD_printLine(8,0, buffer, n);
  n=sprintf (buffer, "OTG_FS_GINTSTS %#08x", *( (volatile unsigned long *) OTG_FS_GINTSTS)); 
  LCD_printLine(9,0, buffer, n);
  n=sprintf (buffer, "OTG_FS_GINTMSK %#08x", *( (volatile unsigned long *) OTG_FS_GINTMSK)); 
  LCD_printLine(10,0, buffer, n);
  
  n=sprintf (buffer, "OTG_FS_GUSBCFG %#08x", *( (volatile unsigned long *) OTG_FS_GUSBCFG)); 
  LCD_printLine(11,0, buffer, n);
  
  n=sprintf (buffer, "OTG_FS_DCFG %#04x", *( (volatile unsigned long *) OTG_FS_DCFG)); 
  LCD_printLine(12,0, buffer, n);
  
  n=sprintf (buffer, "OTG_FS_GCCFG %#08x", *( (volatile unsigned long *) OTG_FS_GCCFG)); 
  LCD_printLine(13,0, buffer, n);
  n=sprintf (buffer, "MODE %d", currentMode); 
  LCD_printLine(14,0, buffer, n);
   
 

  //OTG_FS_DCFG &= ~(1<<2);
}

void usb_reset_routine()
{
    *( (volatile unsigned long *) OTG_FS_DOEPCTL0) |= 1 << 27; //SNAK
    
    *( (volatile unsigned long *) OTG_FS_DAINTMSK) |= 1 << 0; //V
    *( (volatile unsigned long *) OTG_FS_DAINTMSK) |= 1 << 16; //V
    
    *( (volatile unsigned long *) OTG_FS_DOEPMSK) |= 1 << 3; //STUP
    *( (volatile unsigned long *) OTG_FS_DOEPMSK) |= 1 << 0; //XFRC    
    
    *( (volatile unsigned long *) OTG_FS_DIEPMSK) |= 1 << 0; //XFRCM
    *( (volatile unsigned long *) OTG_FS_DIEPMSK) |= 1 << 3; //TOC
    
   *( (volatile unsigned long *) OTG_FS_GRXFSIZ) |= 0x80; 
      
   *( (volatile unsigned long *) OTG_FS_HNPTXFSIZ) |= 0x40<<16; 
   
    *( (volatile unsigned long *) OTG_FS_DOEPTSIZ0) |= (1 << 29) | (1 << 30); 
}

void usb_enum_done_routine()
{
    unsigned enumSpeed = (*( (volatile unsigned long *) OTG_FS_DSTS) & 0x6) >> 1;
    *( (volatile unsigned long *) OTG_FS_DIEPCTL0) |= 0x3; 
        /*
    00: 64 bytes
    01: 32 bytes
    10: 16 bytes
    11: 8 bytes */
}

volatile unsigned long readBuffer[128];

void OTG_FS_IRQHandler(void){
  
  char buffer [50];
  static int tmp = 0;
  static int fifo_num = 0;
  int n, k, c;
  unsigned long stsStorage = *( (volatile unsigned long *) OTG_FS_GINTSTS);
  
  if (stsStorage & USBRST)
  {
    usb_reset_routine();    
    n=sprintf (buffer, "USBRST"); 
    LCD_printLine(0,0, buffer, n);
    *( (volatile unsigned long *) OTG_FS_GINTSTS) |= USBRST;
    tmp++;
  }

  if (stsStorage & ENUMDNE)
  {

    usb_enum_done_routine();
    n=sprintf (buffer, "END OF RESET"); 
    LCD_printLine(17,0, buffer, n);
    
    //n=sprintf (buffer, "ENUM SPEED: %d", enumSpeed); 
   // LCD_printLine(17,0, buffer, n);
    n=sprintf (buffer, "ENUMDNE"); 
    LCD_printLine(0,0, buffer, n);
    *( (volatile unsigned long *) OTG_FS_GINTSTS) |= ENUMDNE;    
  }
  
  if (stsStorage & RXFLVL)
  {
    unsigned long regMskStorage = *( (volatile unsigned long *) OTG_FS_GINTMSK);
    *( (volatile unsigned long *) OTG_FS_GINTMSK) = 0;
    unsigned long init_grxstsp = *((volatile unsigned long *) OTG_FS_GRXSTSP);
    unsigned char pcktStatus = (init_grxstsp >> 17) & (0xF) ;
    unsigned int bcnt = (init_grxstsp >> 4) & (0x7FF ) ;
    unsigned char dpid = (init_grxstsp >> 15) & (0x3) ;
    n=sprintf (buffer, "pcktSt %#08x bcnt %#08x", pcktStatus, bcnt); 
    LCD_printLine(1,0, buffer, n);
    
   
    if (bcnt != 0)
    {
      
      for (n = 0; n < (int)(bcnt/8); n++)
        {
          readBuffer[fifo_num] = *( (volatile unsigned long *) (USB_BASE_ADDRESS + 0x1000));
          fifo_num++;
          readBuffer[fifo_num] = *( (volatile unsigned long *) (USB_BASE_ADDRESS + 0x1000));
          c=sprintf (buffer, "%d %#2x %#2x %#4x %#4x %#4x", fifo_num, (readBuffer[fifo_num-1]>>24)&0xFF, (readBuffer[fifo_num-1]>>16)&0xFF, (readBuffer[fifo_num-1]>>0)&0xFFFF, (readBuffer[fifo_num]>>16)&0xFFFF, (readBuffer[fifo_num]>>0)&0xFFFF ); 
          
          fifo_num++;
          LCD_printLine(2+ fifo_num/2,0, buffer, c);
        }
      
      
      if (pcktStatus == 0x04)
      {
              
      }
      
      else if (pcktStatus == 0x06)
      {             
        //fifo_num -= 1;
        //c=sprintf (buffer, "%d %#2x %#2x %#4x %#4x %#4x", fifo_num, readBuffer[fifo_num]>>56, (readBuffer[fifo_num]>>48)&0xFF, (readBuffer[fifo_num]>>32)&0xFFFF, (readBuffer[fifo_num]>>16)&0xFFFF, readBuffer[fifo_num]&0xFFFF ); 
        //LCD_printLine(23,0, buffer, c);
        //fifo_num++;
      }
    }
    
    *( (volatile unsigned long *) OTG_FS_GINTMSK) = regMskStorage;
   
  }

   
   n=sprintf (buffer, "OTG_FS_GINTSTS %#08x", stsStorage); 
   LCD_printLine(20,0, buffer, n);
  
   n=sprintf (buffer, "tmp %d", tmp); 
   LCD_printLine(19,0, buffer, n);
   
   if (stsStorage & SOFM)
   {
     unsigned long dsts = *( (volatile unsigned long *) OTG_FS_DSTS);
     n=sprintf (buffer, "OTG_FS_DSTS %#08x", dsts); 
     LCD_printLine(21,0, buffer, n);
     *( (volatile unsigned long *) OTG_FS_GINTSTS) |= SOFM;
   }
   
   if (stsStorage & ESUSPM)
   {
     *( (volatile unsigned long *) OTG_FS_GINTSTS) |= ESUSPM;
   }
   
   if (stsStorage & USBSUSPM)
   {
     *( (volatile unsigned long *) OTG_FS_GINTSTS) |= USBSUSPM;
   }
   
   if (stsStorage & OTGINT)
   {
    
     unsigned long gotgint = *( (volatile unsigned long *) OTG_FS_GOTGINT);
     n=sprintf (buffer, "OTG_FS_GOTGINT %#08x", gotgint); 
     LCD_printLine(20,0, buffer, n);
     *( (volatile unsigned long *) OTG_FS_GOTGINT) |= (gotgint);
     //*( (volatile unsigned long *) OTG_FS_GINTSTS) |= OTGINT;
   }
   
   if (stsStorage & OEPINT)
   {
     unsigned long daint = *( (volatile unsigned long *) OTG_FS_DAINT);
     n=sprintf (buffer, "OTG_FS_DAINT %#08x", daint); 
     LCD_printLine(22,0, buffer, n);
      if ((*((volatile unsigned long *) OTG_FS_DOEPINT0)) & (0x8))
      {
        int stp_cnt = (*((volatile unsigned long *) OTG_FS_DOEPTSIZ0) >> 29) & 0x3;
        k=sprintf (buffer, "%d", stp_cnt); 
        LCD_printLine(18,0, buffer, k);        

        c=sprintf (buffer, "%#8x",readBuffer[4 - 2*stp_cnt]); 
        LCD_printLine(15,0, buffer, c);
        c=sprintf (buffer, "%#8x",readBuffer[5 - 2*stp_cnt]); 
        LCD_printLine(16,0, buffer, c);
        //fifo_num = 0;
      }     
   }
   
  
   return;
}
