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
  *( (volatile unsigned long *) OTG_FS_GINTMSK) |= (1<< 4) | (1 << 12) | (1 << 13);//(1 << 3) | (1 << 10) | (1 << 11)  |   // 
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

