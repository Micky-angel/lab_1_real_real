// EJERCICIO 1
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
#include "driverlib/uart.h"

// Variables parwa la freq del sistema
uint32_t g_ui32SysClock;

// interrupcion del timer0
void Timer0IntHandler(void)
{
    // clean interrupt 0
    MAP_TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    // read and tooooogle it
    uint8_t ledState = GPIOPinRead(GPIO_PORTN_BASE, GPIO_PIN_1);
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, ledState ^ GPIO_PIN_1);
}

void Timer1IntHandler(void) {}

int main(void)
{
    //
    g_ui32SysClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                             SYSCTL_OSC_MAIN |
                                             SYSCTL_USE_PLL |
                                             SYSCTL_CFG_VCO_240),
                                            120000000);

    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    MAP_GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1);

    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    MAP_TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

    MAP_TimerLoadSet(TIMER0_BASE, TIMER_A, g_ui32SysClock * 5); // es esto bro pa cmabiar

    MAP_IntEnable(INT_TIMER0A);
    MAP_TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    MAP_TimerEnable(TIMER0_BASE, TIMER_A);

    while (1)
    {
    }
}