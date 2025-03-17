// EJERCICIO 4
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

uint8_t secuenciajunta[10][4] = {
    {0, 0, 0, 0},
    {0, 0, 0, 1},
    {0, 0, 1, 0},
    {0, 0, 1, 1},
    {0, 1, 0, 0},
    {0, 1, 0, 1},
    {0, 1, 1, 0},
    {0, 1, 1, 1},
    {1, 0, 0, 0},
    {1, 0, 0, 1}};

// iniciar por que ya fue declarado
void Timer1IntHandler(void) {}
// iniciar por que ya fue declarado
void Timer0IntHandler(void)
{

    MAP_TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, secuenciajunta[counter][0] ? GPIO_PIN_1 : 0);
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, secuenciajunta[counter][1] ? GPIO_PIN_0 : 0);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, secuenciajunta[counter][2] ? GPIO_PIN_4 : 0);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, secuenciajunta[counter][3] ? GPIO_PIN_0 : 0);

    int val = GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0);
    if (val == 0)
    {
        // 00000010 -> 00000010
        MAP_TimerLoadSet(TIMER0_BASE, TIMER_A, g_ui32SysClock * 3);
    }
    else
    {
        MAP_TimerLoadSet(TIMER0_BASE, TIMER_A, g_ui32SysClock * 1.5);
    }

    counter = (counter + 1) % 10;
}

int main(void)
{

    g_ui32SysClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                             SYSCTL_OSC_MAIN |
                                             SYSCTL_USE_PLL |
                                             SYSCTL_CFG_VCO_240),
                                            120000000);

    // Habilitar puertos
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION); // PN0 PN1
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); // PF0   PF4
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);

    MAP_GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    MAP_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4);

    MAP_GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_0);
    MAP_GPIOPadConfigSet(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    // start Timer 0
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    MAP_TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

    // ----------------------------------------------------------------------------
    MAP_TimerLoadSet(TIMER0_BASE, TIMER_A, g_ui32SysClock * 0.5);

    MAP_IntEnable(INT_TIMER0A);
    MAP_TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    MAP_TimerEnable(TIMER0_BASE, TIMER_A);

    while (1)
    {
    }
}