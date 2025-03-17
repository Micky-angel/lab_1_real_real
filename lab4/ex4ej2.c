// EJERCICIO 2
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"

uint32_t g_ui32SysClock;
uint8_t counter = 0; // este es el int cont de los olds

uint8_t ledSequence[5] = {
    0b0001,
    0b0011,
    0b0111,
    0b1111,

    0b0000};

// iniciar por que ya fue declarado

void Timer1IntHandler(void) {}
void Timer0IntHandler(void)
{
    //--------------------------------------------------------------------------------------------clean the interrupcion
    MAP_TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, (ledSequence[counter] & 0x01) ? GPIO_PIN_1 : 0);
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, (ledSequence[counter] & 0x02) ? GPIO_PIN_0 : 0);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, (ledSequence[counter] & 0x04) ? GPIO_PIN_4 : 0);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, (ledSequence[counter] & 0x08) ? GPIO_PIN_0 : 0);

    counter = (counter + 1) % 5;
}

int main(void)
{

    g_ui32SysClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                             SYSCTL_OSC_MAIN |
                                             SYSCTL_USE_PLL |
                                             SYSCTL_CFG_VCO_240),
                                            120000000);

    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION); // PN0 PN1
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); // PF0 PF4

    MAP_GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1 | GPIO_PIN_0);
    MAP_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0);

    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    MAP_TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

    // ----------------------------------------------------------------------change this bro the *
    MAP_TimerLoadSet(TIMER0_BASE, TIMER_A, g_ui32SysClock * 2);

    MAP_IntEnable(INT_TIMER0A);
    MAP_TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    MAP_TimerEnable(TIMER0_BASE, TIMER_A);
    while (1)
    {
    }
}