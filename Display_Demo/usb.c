#include "usb.h"
#include "lcd.h"

#define REPORT_LENGHT 52

__IO struct struct_usb *usb = (struct struct_usb *) USB_BASE_ADDRESS;
//usb->reg1 = 0x00;

const  uint8_t  deviceDescriptor  [ 18 ]  =  
{ 
	18 , 			// bLength 
	1 , 			// bDescriptorType 
	0x00 ,  0x02 , 	        // bcdUSB 
	0x00 , 		        // bDeviceClass 
	0x00 , 		        // bDeviceSubClass 
	0x00 , 		        // bDeviceProtocol 
	64 , 			// bMaxPacketSize0 
	0xAD ,  0xDE ,          // idVendor 
	0xEF ,  0xBE ,          // idProduct 
	0x00 ,  0x02 ,          // bcdDevice
	0 , 			// iManufacturer 
	0 , 			// iProduct 
	0 , 			// iSerialNumber 
	1 			// bNumConfigurations 
};

const  uint8_t  configDescriptor  []  =  
{ 
    // Configuration descriptor
    0x09, // bLength: Configuration Descriptor size
    0x02, // bDescriptorType: Configuration
    0x22, // wTotalLength: 34 Bytes returned
    0x00,
    0x01, // bNumInterfaces: 1 interface
    0x01, // bConfigurationValue: Configuration value
    0x00, // iConfiguration: Index of string descriptor describing the configuration
    0xC0, // bmAttributes: Bus powered and Remote wake up
    0x0A, // MaxPower 20 mA
    // Interface descriptor
    0x09, // bLength: Interface Descriptor size
    0x04, // bDescriptorType: Interface descriptor type
    0x01, // bInterfaceNumber: Number of Interface )
    0x00, // bAlternateSetting: Alternate setting
    0x01, // bNumEndpoints: one endpoint used
    0x03, // bInterfaceClass: HID
    0x01, // bInterfaceSubClass: No subclass
    0x02, // bInterfaceProtocol: Mouse
    0x00, // interface: Index of string descriptor
    // HID descriptor
    0x09, // bLength: HID Descriptor size
    0x21, // bDescriptorType: HID
    0x00, // bcdHID: HID Class Spec release number
    0x01,
    0x21, // bCountryCode: Hardware target country US
    0x01, // bNumDescriptors: Number of HID class descriptors to follow
    0x22, // bDescriptorType     
    REPORT_LENGHT, // wItemLength: Report descriptor length low bite
    0x00,
    // Endpoint descriptor
    0x07, // bLength: Endpoint Descriptor size
    0x05, // bDescriptorType: Endpoint descriptor type
    0x81, // bEndpointAddress: Endpoint Address (IN)
    0x03, // bmAttributes: Interrupt endpoint
    0x08, // wMaxPacketSize: 8 Byte max
    0x00,
    0x0A // bInterval: Polling Interval (8 ms) 
     
};

volatile int tmp = 0;

struct setup_packet {
  uint8_t bmRequestType;
  uint8_t bRequest;
  uint16_t wValue;
  uint16_t wIndex;
  uint16_t wLength;
};

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
  
  *( (volatile unsigned long *) OTG_FS_DCFG) = (1<<0) | (1<<1)  ; // DSPD[1:0] = 11
  *( (volatile unsigned long *) OTG_FS_DCFG) &= ~(1<<2)  ; // NZLSOHSK[2] = 0
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
   
    *( (volatile unsigned long *) OTG_FS_GRSTCTL) |= 1 << 4; //FLUSH FIFO
    *( (volatile unsigned long *) OTG_FS_DOEPCTL0) |= 1 << 27; //SNAK
    *( (volatile unsigned long *) OTG_FS_DIEPCTL0) &= ~(1 << 21); //NOT STALL
    *( (volatile unsigned long *) OTG_FS_DOEPCTL0) &= ~(1 << 21); //NOT STALL
    
    *( (volatile unsigned long *) OTG_FS_DAINTMSK) = 0x10001; //Enable EPIN0 & EPOUT0
    
    
    *( (volatile unsigned long *) OTG_FS_DOEPMSK) |= 0xB; //STUP XFRC EPDM
        
    *( (volatile unsigned long *) OTG_FS_DCFG) &= ~(0x7F0); //zero address
    //*( (volatile unsigned long *) OTG_FS_DCFG) |= (0x7F0);
    
    *( (volatile unsigned long *) OTG_FS_DIEPMSK) |= 0xB; //XFRCM TOC EPDM
  
   *( (volatile unsigned long *) OTG_FS_GRXFSIZ) |= 0x80; 
      
   *( (volatile unsigned long *) OTG_FS_HNPTXFSIZ) |= 0x40<<16; 
   
    *( (volatile unsigned long *) OTG_FS_DOEPTSIZ0) |= (1 << 29) | (1 << 30); 
    //*( (volatile unsigned long *) OTG_FS_DOEPCTL0) |= 1 << 31;
}

void usb_enum_done_routine()
{
    unsigned enumSpeed = (*( (volatile unsigned long *) OTG_FS_DSTS) & 0x6) >> 1;
    *( (volatile unsigned long *) OTG_FS_DIEPCTL0) |= 0x1; 
        /*
    00: 64 bytes
    01: 32 bytes
    10: 16 bytes
    11: 8 bytes */
}
uint16_t newAddr = 0;
uint8_t needToChangeAddr = 0;

uint8_t send_desc(struct setup_packet * stp_pck)
{
  int n;
  char buffer[50];

    
  uint8_t  type  =  (( stp_pck->wValue  >>  8 )  &  0xFF ); 
  uint8_t  index  = ( stp_pck->wValue  &  0xFF ); 
  
  volatile static uint32_t TxBuffer[64];
  if  (stp_pck->bRequest == 6 && type  ==  1  &&  index  ==  0  &&  stp_pck->wIndex  ==  0)  
  { 
    
    uint16_t  length  =  stp_pck->wLength > sizeof(deviceDescriptor) ? sizeof(deviceDescriptor) : stp_pck->wLength;  
    
    for (n = 0; n < (int)(length/4); n++)
    {
      //*( (volatile unsigned long *) (USB_BASE_ADDRESS + 0x1000)) = 
      TxBuffer[n] = deviceDescriptor[n*4] | (deviceDescriptor[n*4 + 1] << 8) |
        (deviceDescriptor[n*4 + 2] << 16) | (deviceDescriptor[n*4 + 3] << 24);
            
    }
    if (length%4 != 0)
    {
      TxBuffer[(int)(length/4)] = 0;
      for (n = 0; n < length%4; n++)
      {
        TxBuffer[(int)(length/4)] |= deviceDescriptor[((int)(length/4))*4+n]<<(8*n);
      }
    }
    
    int free_tx = (*( (volatile unsigned long *) OTG_FS_DTXFSTS0) & 0xFFFF)*4;
    if (free_tx > length)
    {
      
      *( (volatile unsigned long *) OTG_FS_DIEPTSIZ0) = (1<<19) | length;
      
      volatile unsigned long tmp_reg = *( (volatile unsigned long *) OTG_FS_DIEPCTL0);
      tmp_reg &= ~(0x3 | (0xF << 22));
      tmp_reg |= (1<<26);
      *( (volatile unsigned long *) OTG_FS_DIEPCTL0) = tmp_reg;
      *( (volatile unsigned long *) OTG_FS_DIEPCTL0) |= (1<<31);
      
      for (n = 0; n <= (int)(length/4); n++)
      {
        *( (volatile unsigned long *) (USB_BASE_ADDRESS + 0x1000)) = TxBuffer[n];
      }
      //tmp++;
      //n=sprintf (buffer, "Sent: %d, avail: %d", length, free_tx ); 
      //LCD_printLine(3,0, buffer, n);
      return 1;
    }
    //n=sprintf (buffer, "%#08x %#08x %d", TxBuffer[3], TxBuffer[4], (int)(length/4)); 
    //LCD_printLine(4,0, buffer, n);
  }
  
  else if  (stp_pck->bRequest == 6 && type  ==  2  &&  index  ==  0  &&  stp_pck->wIndex  ==  0)  
  { 
    if (stp_pck->wLength == 34)
    {
      uint8_t bp = 0;
    }
    uint16_t  length  =  stp_pck->wLength > sizeof(configDescriptor) ? sizeof(configDescriptor) : stp_pck->wLength;
    
    
    
    for (n = 0; n < (int)(length/4); n++)
    {
      TxBuffer[n] = configDescriptor[n*4] | (configDescriptor[n*4 + 1] << 8) |
        (configDescriptor[n*4 + 2] << 16) | (configDescriptor[n*4 + 3] << 24);
            
    }
    if (length%4 != 0)
    {
      TxBuffer[(int)(length/4)] = 0;
      for (n = 0; n < length%4; n++)
      {
        TxBuffer[(int)(length/4)] |= configDescriptor[((int)(length/4))*4+n]<<(8*n);
      }
    }
    
    int free_tx = (*( (volatile unsigned long *) OTG_FS_DTXFSTS0) & 0xFFFF)*4;
    if (free_tx > length)
    {
      
      *( (volatile unsigned long *) OTG_FS_DIEPTSIZ0) = (1<<19) | length;
      
      volatile unsigned long tmp_reg = *( (volatile unsigned long *) OTG_FS_DIEPCTL0);
      tmp_reg &= ~(0x3 | (0xF << 22));
      tmp_reg |= (1<<26);
      *( (volatile unsigned long *) OTG_FS_DIEPCTL0) = tmp_reg;
      *( (volatile unsigned long *) OTG_FS_DIEPCTL0) |= (1<<31);
      
      for (n = 0; n <= (int)(length/4); n++)
      {
        *( (volatile unsigned long *) (USB_BASE_ADDRESS + 0x1000)) = TxBuffer[n];
      }
      //tmp++;
      //n=sprintf (buffer, "Sent: %d, avail: %d", length, free_tx ); 
      //LCD_printLine(3,0, buffer, n);
      return 1;
    }
    //n=sprintf (buffer, "%#08x %#08x %d", TxBuffer[3], TxBuffer[4], (int)(length/4)); 
    //LCD_printLine(4,0, buffer, n);
  }
  
  else if (stp_pck->bmRequestType == 0 && stp_pck->bRequest == 5)
  {
    //*( (volatile unsigned long *) OTG_FS_DCFG) &= ~(0x7F0);
    //*( (volatile unsigned long *) OTG_FS_DCFG) |= stp_pck->wValue << 4;
    int free_tx = (*( (volatile unsigned long *) OTG_FS_DTXFSTS0) & 0xFFFF)*4;
    if (free_tx > 0)
    {
      *( (volatile unsigned long *) OTG_FS_DIEPTSIZ0) = (1<<19);
      volatile unsigned long tmp_reg = *( (volatile unsigned long *) OTG_FS_DIEPCTL0);
      tmp_reg &= ~(0x3 | (0xF << 22));
      tmp_reg |= (1<<26);
      *( (volatile unsigned long *) OTG_FS_DIEPCTL0) = tmp_reg;
      *( (volatile unsigned long *) OTG_FS_DIEPCTL0) |= (1<<31);
      
      needToChangeAddr = 1;
      newAddr = stp_pck->wValue;
      
      //*( (volatile unsigned long *) (USB_BASE_ADDRESS + 0x1000));
      
      *( (volatile unsigned long *) OTG_FS_DCFG) |= newAddr << 4;
      //n=sprintf (buffer, "%d", newAddr); 
      //LCD_printLine(4,0, buffer, n);
      
      return 1;
    }
  }
    
    else
    {
    //*( (volatile unsigned long *) OTG_FS_DCFG) &= ~(0x7F0);
    //*( (volatile unsigned long *) OTG_FS_DCFG) |= stp_pck->wValue << 4;
    int free_tx = (*( (volatile unsigned long *) OTG_FS_DTXFSTS0) & 0xFFFF)*4;
    if (free_tx > 0)
    {
      *( (volatile unsigned long *) OTG_FS_DIEPTSIZ0) = (1<<19);
      volatile unsigned long tmp_reg = *( (volatile unsigned long *) OTG_FS_DIEPCTL0);
      tmp_reg &= ~(0x3 | (0xF << 22));
      tmp_reg |= (1<<26);
      *( (volatile unsigned long *) OTG_FS_DIEPCTL0) = tmp_reg;
      *( (volatile unsigned long *) OTG_FS_DIEPCTL0) |= (1<<31);
      
      
      return 1;
    }
  }
  return 0;
}

void sendOUT()
{
  
  *( (volatile unsigned long *) OTG_FS_DOEPTSIZ0) = (1<<19);
  volatile unsigned long tmp_reg = *( (volatile unsigned long *) OTG_FS_DOEPCTL0);
  tmp_reg &= ~(0x3 | 1<<27);
  tmp_reg |= (1<<26);
  *( (volatile unsigned long *) OTG_FS_DOEPCTL0) = tmp_reg;
  *( (volatile unsigned long *) OTG_FS_DOEPCTL0) |= (1<<31);
     /* 
    *( (volatile unsigned long *) OTG_FS_DIEPTSIZ0) = (1<<19) | 0;
    volatile unsigned long tmp_reg = *( (volatile unsigned long *) OTG_FS_DIEPCTL0);
    tmp_reg &= ~(0x3 | (0xF << 22));
    tmp_reg |= (1<<26);
    *( (volatile unsigned long *) OTG_FS_DIEPCTL0) = tmp_reg;
    *( (volatile unsigned long *) OTG_FS_DIEPCTL0) |= (1<<31);*/
}

volatile unsigned long readBuffer[128];
volatile uint8_t add_set = 0;
void OTG_FS_IRQHandler(void){
  
  char buffer [50];

  static int fifo_num = 0;
  static uint8_t allowedReplies = 10;
  static uint8_t packetProcessed = 0;
  if (packetProcessed > 20) packetProcessed = 0;
  static int pos = 0;
  if (pos > 32) pos = 0;
  int n, k, c;
  unsigned long stsStorage = *( (volatile unsigned long *) OTG_FS_GINTSTS);
  
  n=sprintf (buffer, "%#08x", (*( (volatile unsigned long *) OTG_FS_DCFG) >> 4) & 0x7F); 
  LCD_printLine(2,0, buffer, n);
  
  

  
  if (stsStorage & USBRST)
  {
    usb_reset_routine();    
    n=sprintf (buffer, "U"); 
    LCD_printLine(0,pos, buffer, n);
    *( (volatile unsigned long *) OTG_FS_GINTSTS) |= USBRST;
    //tmp++;
    pos ++;
  }

  if (stsStorage & ENUMDNE)
  {

    usb_enum_done_routine();
    n=sprintf (buffer, "END OF RESET"); 
    LCD_printLine(17,0, buffer, n);
    
    //n=sprintf (buffer, "ENUM SPEED: %d", enumSpeed); 
   // LCD_printLine(17,0, buffer, n);
    n=sprintf (buffer, "E"); 
    LCD_printLine(0,pos, buffer, n);
    pos ++;
    *( (volatile unsigned long *) OTG_FS_GINTSTS) |= ENUMDNE;    
  }
  
  if (stsStorage & RXFLVL)
  {
    unsigned long regMskStorage = *( (volatile unsigned long *) OTG_FS_GINTMSK);
    *( (volatile unsigned long *) OTG_FS_GINTMSK) = 0;
    uint32_t init_grxstsp = *((volatile unsigned long *) OTG_FS_GRXSTSP);
    uint8_t pcktStatus = (init_grxstsp >> 17) & (0xF) ;
    uint16_t bcnt = (init_grxstsp >> 4) & (0x7FF ) ;
    uint8_t dpid = (init_grxstsp >> 15) & (0x3) ;
    uint8_t ep_num = (init_grxstsp) & (0xF) ;
    if ((pcktStatus == 2 || pcktStatus == 3)) // && needToChangeAddr
    //if(1)
    {
      //n=sprintf (buffer, "pcktSt %#08x bcnt %#08x", pcktStatus, bcnt);
      n=sprintf (buffer, "%#08x", (*( (volatile unsigned long *) OTG_FS_DOEPCTL0) & (1<<27 | 1 << 26))); 
      LCD_printLine(1,0, buffer, n);
    }
    
    //if (add_set) while(1);
    
    *( (volatile unsigned long *) OTG_FS_DOEPCTL0) |= (1 << 26)  | (1 << 31);
    n=sprintf (buffer, "R"); 
    LCD_printLine(0,pos, buffer, n);
    pos ++; 
    
    if (pcktStatus == 0x02 || pcktStatus == 0x03)
    {
      //sendOUT();
      //*( (volatile unsigned long *) OTG_FS_DOEPCTL0) |= (1<<26) | (1 << 31);

    }
    
    if (pcktStatus == 0x04)
    {
      //*( (volatile unsigned long *) OTG_FS_DOEPCTL0) |= (1<<26) | (1 << 31);
  
    }
    
    else if (pcktStatus == 0x06 && bcnt != 0)
    {             
      for (n = 0; n < (int)(bcnt/4); n++)
      {
        readBuffer[fifo_num] = *( (volatile unsigned long *) (USB_BASE_ADDRESS + 0x1000));
        
        //readBuffer[fifo_num] = *( (volatile unsigned long *) (USB_BASE_ADDRESS + 0x1000));
        if (fifo_num > 1)
        {          
          c=sprintf (buffer, "%d %d %#08x %#08x", fifo_num, bcnt, readBuffer[fifo_num-1], readBuffer[fifo_num]);               
          LCD_printLine(21,0, buffer, c);
        }
        fifo_num++;
      }
      
      struct setup_packet stp_pck;
      stp_pck.bmRequestType = readBuffer[fifo_num-2] & 0xFF;
      stp_pck.bRequest = (readBuffer[fifo_num-2] >> 8) & 0xFF;
      stp_pck.wValue = (readBuffer[fifo_num-2] >> 16) & 0xFFFF;
      stp_pck.wIndex = readBuffer[fifo_num-1] & 0xFFFF;
      stp_pck.wLength = (readBuffer[fifo_num-1] >> 16) & 0xFFFF;
      c=sprintf (buffer, "%d %#02x %#02x %#02x %#04x %#04x %#04x", ep_num, ((*( (volatile unsigned long *) OTG_FS_DCFG) >> 4) & 0x7F), stp_pck.bmRequestType, stp_pck.bRequest, stp_pck.wValue, stp_pck.wIndex, stp_pck.wLength); 
      LCD_printLine(3+packetProcessed,0, buffer, c);
      packetProcessed++;
      send_desc(&stp_pck);
      n=sprintf (buffer, "S"); 
      LCD_printLine(0,pos, buffer, n);
      pos ++;
      
    }
    
    *( (volatile unsigned long *) OTG_FS_GINTMSK) = regMskStorage;

  }
        
   n=sprintf (buffer, "OTG_FS_GINTSTS %#08x", stsStorage); 
   LCD_printLine(20,0, buffer, n);
  
   n=sprintf (buffer, "tmp %d", tmp); 
   LCD_printLine(19,0, buffer, n);
   
   
   
   if (stsStorage & IEPINT)
   {
    unsigned long daint = *( (volatile unsigned long *) OTG_FS_DAINT);
    n=sprintf (buffer, "OTG_FS_DAINT %#08x", daint); 
    if (*( (volatile unsigned long *) OTG_FS_DIEPINT0) & (1<<0))
    {   
        if (needToChangeAddr)
        {
          needToChangeAddr = 0;
          volatile unsigned long init_dcfg = *( (volatile unsigned long *) OTG_FS_DCFG);
          //*( (volatile unsigned long *) OTG_FS_DCFG) &= ~(0x7F0);
          //*( (volatile unsigned long *) OTG_FS_DCFG) |= newAddr << 4;
          //*( (volatile unsigned long *) OTG_FS_DCFG) |= 0x7F0;
          n=sprintf (buffer, "%#02x %#08x %#08x", newAddr, init_dcfg, *( (volatile unsigned long *) OTG_FS_DCFG)); 
          LCD_printLine(1,0, buffer, n);
          //while(1);
          add_set = 1;
          tmp++;
        }
        
      //tmp++;
      *( (volatile unsigned long *) OTG_FS_DIEPINT0) |= (1<<7);
    }
   }
   
   /*
   if (stsStorage & SOFM)
   {
     unsigned long dsts = *( (volatile unsigned long *) OTG_FS_DSTS);
     //n=sprintf (buffer, "OTG_FS_DSTS %#08x", dsts); 
     //LCD_printLine(21,0, buffer, n);
     *( (volatile unsigned long *) OTG_FS_GINTSTS) |= SOFM;
    //n=sprintf (buffer, "S"); 
    //LCD_printLine(0,pos, buffer, n);
    //pos ++;
   }
   
   if (stsStorage & ESUSPM)
   {
     *( (volatile unsigned long *) OTG_FS_GINTSTS) |= ESUSPM;
    //n=sprintf (buffer, "1"); 
    //LCD_printLine(0,pos, buffer, n);
    //pos ++;
   }
   
   if (stsStorage & USBSUSPM)
   {
     *( (volatile unsigned long *) OTG_FS_GINTSTS) |= USBSUSPM;
    n=sprintf (buffer, "2"); 
    LCD_printLine(0,pos, buffer, n);
    pos ++;
   }
   
   if (stsStorage & OTGINT)
   {

     unsigned long gotgint = *( (volatile unsigned long *) OTG_FS_GOTGINT);
     n=sprintf (buffer, "OTG_FS_GOTGINT %#08x", gotgint); 
     LCD_printLine(20,0, buffer, n);
     *( (volatile unsigned long *) OTG_FS_GOTGINT) |= (gotgint);
     *( (volatile unsigned long *) OTG_FS_GINTSTS) |= OTGINT;
    //n=sprintf (buffer, "3"); 
    //LCD_printLine(0,pos, buffer, n);
    //pos ++;
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
      /*if(allowedReplies--)
      {
        struct setup_packet stp_pck;
        stp_pck.bmRequestType = readBuffer[4 - 2*stp_cnt] & 0xFF;
        stp_pck.bRequest = (readBuffer[4 - 2*stp_cnt] >> 8) & 0xFF;
        stp_pck.wValue = (readBuffer[4 - 2*stp_cnt] >> 16) & 0xFFFF;
        stp_pck.wIndex = readBuffer[5 - 2*stp_cnt] & 0xFFFF;
        stp_pck.wLength = (readBuffer[5 - 2*stp_cnt] >> 16) & 0xFFFF;
        uint8_t addr =  ((*( (volatile unsigned long *) OTG_FS_DCFG) >> 4) & 0x7F);
        uint8_t res = send_desc(&stp_pck);
        
        //if (stp_pck.bRequest != 6 && stp_pck.bRequest != 5)
        if(1)
        {
          c=sprintf (buffer, "%d %d %#02x %#02x %#04x %#04x %#04x", res, addr, stp_pck.bmRequestType, stp_pck.bRequest, stp_pck.wValue, stp_pck.wIndex, stp_pck.wLength); 
          LCD_printLine(3+packetProcessed,0, buffer, c);
          packetProcessed++;
        }
        
        n=sprintf (buffer, "S"); 
        LCD_printLine(0,pos, buffer, n);
        pos ++;
        
      }*/
              
   //   *((volatile unsigned long *) OTG_FS_DOEPINT0) |= (0x8);
   //   *( (volatile unsigned long *) OTG_FS_DOEPCTL0) |= 1 << 26; //CNAK
      //tmp++;
    //}

    //n=sprintf (buffer, "4"); 
    //LCD_printLine(0,pos, buffer, n);
    //pos ++;
    //}
   
  
   return;
}
