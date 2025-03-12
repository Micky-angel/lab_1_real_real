//*****************************************************************************
//
// blinky.c - Simple example to blink the on-board LED.
//
// Copyright (c) 2013-2020 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 2.2.0.295 of the EK-TM4C1294XL Firmware Package.
//
//*****************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>Blinky (blinky)</h1>
//!
//! A very simple example that blinks the on-board LED using direct register
//! access.
//
//*****************************************************************************

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
    while(1);
}
#endif

//*****************************************************************************
//
// Blink the on-board LED.
//
//*****************************************************************************
int
main(void)
{
    volatile uint32_t ui32Loop;

    // Configuramos el clock
    SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480),120000000); 
	  
    //
    // Enable the GPIO port that is used for the on-board LED.
    //

    // Activar los perifericos que utilizaremos 
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);// enables periphel N    
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);// enables periphel F
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);// enables periphel F
    //
    // Check if the peripheral access is enabled.
    //
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION))
    {
    }
    
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF))
    {
    }
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ))
    {
    }

    //
    // Enable the GPIO pin for the LED (PN0).  Set the direction as output, and
    // enable the GPIO pin for digital function.
    //
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1);

    
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_4);

    
    //INPUT
    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_0);
    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_1);
    
    GPIOPadConfigSet(GPIO_PORTJ_BASE,GPIO_PIN_0|GPIO_PIN_1,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
    
    //
    // Loop forever.
    //
    volatile int cnt=0;
    volatile int last0=1;
    volatile int last1=1;
    while(1)
    {
        //
        // Turn on the LED.
        //
      //GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, GPIO_PIN_0);

      //raro
      /* GPIOPinWrite(GPIO_PORTN_BASE, 0x03, 0x02); */
      /* GPIOPinWrite(GPIO_PORTF_BASE, 0x11, 0x01); */

      // SHORT EXAMPLE 1.

      /* GPIOPinWrite(GPIO_PORTN_BASE, 0x03, 0x02); */
      /* for(ui32Loop = 0; ui32Loop < 2000000; ui32Loop++) */
      /* 	{ */
      /* 	} */
      /* GPIOPinWrite(GPIO_PORTN_BASE, 0x03, 0x01); */
      /* for(ui32Loop = 0; ui32Loop < 2000000; ui32Loop++) */
      /* 	{ */
      /* 	} */
      /* GPIOPinWrite(GPIO_PORTN_BASE, 0x03, 0x00); */
      /* for(ui32Loop = 0; ui32Loop < 2000000; ui32Loop++) */
      /* 	{ */
      /* 	} */
      
      // SHORT EXAMPLE 2.

      // 00000011 -> 00000001
      /* GPIOPinWrite(GPIO_PORTN_BASE, 0x03, 0x01); */
      /* // 00010001 -> 00000001 */
      /* GPIOPinWrite(GPIO_PORTF_BASE, 0x11, 0x01); */
      /* for(ui32Loop = 0; ui32Loop < 2000000; ui32Loop++) */
      /* 	{ */
      /* 	} */
      
      /* // 00000011 -> 00000010 */
      /* GPIOPinWrite(GPIO_PORTN_BASE, 0x03, 0x02); */
      /* // 00010001 -> 00010000 */
      /* GPIOPinWrite(GPIO_PORTF_BASE, 0x11, 0x10); */
      /* for(ui32Loop = 0; ui32Loop < 2000000; ui32Loop++) */
      /* 	{ */
      /* 	}             */

      // SHORT EXAMPLE 3.

      /* int val = GPIOPinRead(GPIO_PORTJ_BASE,GPIO_PIN_1); */
      /* if(val==0){ */
      /* 	// 00000010 -> 00000010 */
      /* 	GPIOPinWrite(GPIO_PORTN_BASE, 0x03, 0x02);	 */
      /* } */
      /* else{ */
      /* 	GPIOPinWrite(GPIO_PORTN_BASE, 0x03, 0x00);	 */
      /* } */
      
      // EXERCISE 1
      
      /* int val0 = GPIOPinRead(GPIO_PORTJ_BASE,GPIO_PIN_0); */
      /* int val1 = GPIOPinRead(GPIO_PORTJ_BASE,GPIO_PIN_1); */
      /* if(val0==0 && last0!=val0){ */
      /* 	cnt=cnt+1; */
      /* 	if(cnt>4)cnt=4; */
      /* } */
      /* if(val1==0 && last1!=val1){ */
      /* 	cnt=cnt-1; */
      /* 	if(cnt<0)cnt=0; */
      /* } */
      /* last0 = val0; */
      /* last1 = val1; */
      /* if(cnt==0){ */
      /* 	GPIOPinWrite(GPIO_PORTN_BASE, 0x03, 0x00); */
      /* 	GPIOPinWrite(GPIO_PORTF_BASE, 0x11, 0x00); */
      /* } */
      /* if(cnt==1){ */
      /* 	GPIOPinWrite(GPIO_PORTN_BASE, 0x03, 0x02); */
      /* 	GPIOPinWrite(GPIO_PORTF_BASE, 0x11, 0x00); */
      /* } */
      /* if(cnt==2){ */
      /* 	GPIOPinWrite(GPIO_PORTN_BASE, 0x03, 0x03); */
      /* 	GPIOPinWrite(GPIO_PORTF_BASE, 0x11, 0x00); */
      /* } */
      /* if(cnt==3){ */
      /* 	GPIOPinWrite(GPIO_PORTN_BASE, 0x03, 0x03); */
      /* 	GPIOPinWrite(GPIO_PORTF_BASE, 0x11, 0x10); */
      /* } */
      /* if(cnt==4){ */
      /* 	GPIOPinWrite(GPIO_PORTN_BASE, 0x03, 0x03); */
      /* 	GPIOPinWrite(GPIO_PORTF_BASE, 0x11, 0x11); */
      /* } */

      // EXERCISE 2
      
      /* GPIOPinWrite(GPIO_PORTN_BASE, 0x03, 0x02); */
      /* GPIOPinWrite(GPIO_PORTF_BASE, 0x11, 0x10); */
      
      /* SysCtlDelay(SysCtlClockGet()/0.0675); */

      /* GPIOPinWrite(GPIO_PORTN_BASE, 0x03, 0x01); */
      /* GPIOPinWrite(GPIO_PORTF_BASE, 0x11, 0x01); */
      
      /* SysCtlDelay(SysCtlClockGet()/0.0675); */

      /* GPIOPinWrite(GPIO_PORTN_BASE, 0x03, 0x02); */
      /* GPIOPinWrite(GPIO_PORTF_BASE, 0x11, 0x01); */

      /* SysCtlDelay(SysCtlClockGet()/0.0675); */

      /* GPIOPinWrite(GPIO_PORTN_BASE, 0x03, 0x01); */
      /* GPIOPinWrite(GPIO_PORTF_BASE, 0x11, 0x10); */

      /* SysCtlDelay(SysCtlClockGet()/0.0675); */
      
      // EXERCISE 3
      
      int val0 = GPIOPinRead(GPIO_PORTJ_BASE,GPIO_PIN_0);
      int val1 = GPIOPinRead(GPIO_PORTJ_BASE,GPIO_PIN_1);
      if(val0==0 && last0!=val0){
	cnt=cnt+1;
	if(cnt>15)cnt=15;
      }
      if(val1==0 && last1!=val1){
	cnt=cnt-1;
	if(cnt<0)cnt=0;
      }
      last0 = val0;
      last1 = val1;
      int aux=cnt;
      int sw1=aux%2;
      aux=aux/2;
      int sw2=aux%2;
      aux=aux/2;
      int sw3=aux%2;
      aux=aux/2;
      int sw4=aux;
      if(sw1==1){
	GPIOPinWrite(GPIO_PORTN_BASE, 0x03, 0x02);
      }
      else{
	GPIOPinWrite(GPIO_PORTN_BASE, 0x03, 0x00);
      }
      if(sw2==1){
	GPIOPinWrite(GPIO_PORTN_BASE, 0x03, 0x01);
      }
      else{
	GPIOPinWrite(GPIO_PORTN_BASE, 0x03, 0x00);
      }
      if(sw3==1){
	GPIOPinWrite(GPIO_PORTF_BASE, 0x11, 0x10);
      }
      else{
	GPIOPinWrite(GPIO_PORTF_BASE, 0x11, 0x00);
      }
      if(sw4==1){
	GPIOPinWrite(GPIO_PORTF_BASE, 0x11, 0x01);
      }
      else{
	GPIOPinWrite(GPIO_PORTF_BASE, 0x11, 0x00);
      }
      
      //GPIO INPUT
      
      /* if(GPIOPinRead(GPIO_PORTJ_BASE,GPIO_PIN_0)==0){ */
      /* 	button++; */
      /* 	GPIOPinWrite(GPIO_PORTN_BASE, 0x03, 0x02); */
      /* 	GPIOPinWrite(GPIO_PORTF_BASE, 0x11, 0x01); */
      /* } */
      /* if(button%2 == 0){ */
      /* 	GPIOPinWrite(GPIO_PORTN_BASE, 0x03, 0x02); */
      /* 	GPIOPinWrite(GPIO_PORTF_BASE, 0x11, 0x01); */
      /* 	for(ui32Loop = 0; ui32Loop < 200000; ui32Loop++) */
      /* 	  { */
      /* 	  } */
      /* } */
      /* else{ */
      /* 	GPIOPinWrite(GPIO_PORTN_BASE, 0x03, 0x01); */
      /* 	GPIOPinWrite(GPIO_PORTF_BASE, 0x11, 0x10); */
      /* 	for(ui32Loop = 0; ui32Loop < 200000; ui32Loop++) */
      /* 	  { */
      /* 	  } */
      /* } */
      /* // */
      /* // Delay for a bit. */
      /* // */
      /* for(ui32Loop = 0; ui32Loop < 200000; ui32Loop++) */
      /*   { */
      /*   } */
      
        //
        // Turn off the LED.
        //
        //GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, 0x0);
	/* GPIOPinWrite(GPIO_PORTN_BASE, 0x03, 0x01); */
	/* GPIOPinWrite(GPIO_PORTF_BASE, 0x11, 0x10); */

        //
        // Delay for a bit.
        //
        /* for(ui32Loop = 0; ui32Loop < 200000; ui32Loop++) */
        /* { */
        /* } */
    }
}
