#ifndef __USB_H
#define __USB_H

#include "stm32f4xx_conf.h"

#define USB_BASE_ADDRESS 0x50000000
#define OTG_FS_GOTGINT (USB_BASE_ADDRESS + 0x004)
#define OTG_FS_GAHBCFG (USB_BASE_ADDRESS + 0x008)
#define OTG_FS_GUSBCFG (USB_BASE_ADDRESS + 0x00C)
#define OTG_FS_GRSTCTL (USB_BASE_ADDRESS + 0x010)
#define OTG_FS_GINTSTS (USB_BASE_ADDRESS + 0x014)
#define OTG_FS_GINTMSK (USB_BASE_ADDRESS + 0x018)

#define OTG_FS_GRXFSIZ (USB_BASE_ADDRESS + 0x024)

#define OTG_FS_HNPTXFSIZ (USB_BASE_ADDRESS + 0x028)
#define OTG_FS_GCCFG (USB_BASE_ADDRESS + 0x038)

#define OTG_FS_DOEPCTL0 (USB_BASE_ADDRESS + 0xB00)
#define OTG_FS_DOEPINT0 (USB_BASE_ADDRESS + 0xB08)



#define OTG_FS_DOEPTSIZ0 (USB_BASE_ADDRESS + 0xB10)
#define OTG_FS_DCFG (USB_BASE_ADDRESS + 0x800)
#define OTG_FS_DSTS (USB_BASE_ADDRESS + 0x808)
#define OTG_FS_DIEPMSK (USB_BASE_ADDRESS + 0x810)
#define OTG_FS_DOEPMSK (USB_BASE_ADDRESS + 0x814)
#define OTG_FS_DAINT (USB_BASE_ADDRESS + 0x818)
#define OTG_FS_DAINTMSK (USB_BASE_ADDRESS + 0x81C)
#define OTG_FS_DIEPCTL0 (USB_BASE_ADDRESS + 0x900)
#define OTG_FS_DIEPINT0 (USB_BASE_ADDRESS + 0x908)

#define OTG_FS_DIEPTSIZ0 (USB_BASE_ADDRESS + 0x910)
#define OTG_FS_DTXFSTS0 (USB_BASE_ADDRESS + 0x918)

#define OTG_FS_GRXSTSP (USB_BASE_ADDRESS + 0x020)

/*
#define USB_REG1 (USB_BASE_ADDRESS + 0x0000)
#define USB_REG2 (USB_BASE_ADDRESS + 0x0004)

volatile __no_init unsigned long REG@ADDRESS


*( (volatile unsigned long *) USB_REG1) = 0x00;
*/

#define OTGINT (1 << 2)
#define SOFM (1 << 3)
#define RXFLVL (1<<4)
#define ESUSPM (1 << 10)
#define USBSUSPM (1 << 11)
#define USBRST (1<<12)
#define ENUMDNE (1<<13)
#define ENUMDNE (1<<13)
#define OEPINT (1<<19)
#define IEPINT (1<<18)
#define NPTXFE (1<<5)

struct struct_usb
{
 volatile unsigned long reg1;
 volatile unsigned long reg2;
};


void usb_init(void);
void usb_reset_routine();
void usb_enum_done_routine();
#endif