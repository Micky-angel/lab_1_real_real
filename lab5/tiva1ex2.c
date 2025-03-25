#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "utils/uartstdio.h"
#include "utils/uartstdio.c"

uint32_t g_ui32SysClock;
uint8_t sequenceIndex = 0; 
uint8_t sequenceMode = 0;   
uint8_t sequence1[4][4] = {
    {1, 0, 0, 0},  
    {0, 1, 0, 0},  
    {0, 0, 1, 0},  
    {0, 0, 0, 1}   
};


uint8_t sequence2[4][4] = {
    {1, 0, 1, 0},  
    {0, 1, 0, 1},  
    {1, 0, 1, 0},  
    {0, 1, 0, 1}   
};


uint8_t sequence3[4][4] = {
    {1, 1, 0, 0},  
    {0, 0, 1, 1},  
    {1, 1, 0, 0},  
    {0, 0, 1, 1}  
};

void InitializeSystem() {
    g_ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);
    
    
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION)) {}
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1, 0);
    
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)) {}
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4, 0);
    
    
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0)) {}
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTStdioConfig(0, 9600, g_ui32SysClock);
    
    
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, g_ui32SysClock / 2);  
    
    IntEnable(INT_TIMER0A);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    TimerEnable(TIMER0_BASE, TIMER_A);
}

void Timer0IntHandler(void) {
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    
    uint8_t (*currentSequence)[4];
    switch (sequenceMode) {
        case 0: currentSequence = sequence1; break;
        case 1: currentSequence = sequence2; break;
        case 2: currentSequence = sequence3; break;
        default: currentSequence = sequence1; break;
    }
    
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, currentSequence[sequenceIndex][0] ? GPIO_PIN_1 : 0);
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, currentSequence[sequenceIndex][1] ? GPIO_PIN_0 : 0);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, currentSequence[sequenceIndex][2] ? GPIO_PIN_4 : 0);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, currentSequence[sequenceIndex][3] ? GPIO_PIN_0 : 0);
    
    sequenceIndex = (sequenceIndex + 1) % 4;  
}

int main(void) {
    InitializeSystem();
    char inputStr[20];
    
    IntMasterEnable();

    while (1) {
        if (UARTCharsAvail(UART0_BASE)) {
            UARTgets(inputStr, sizeof(inputStr));
            if (strcmp(inputStr, "seq1") == 0) {
                sequenceMode = 0;  
            }
            else if (strcmp(inputStr, "seq2") == 0) {
                sequenceMode = 1;  
            }
            else if (strcmp(inputStr, "seq3") == 0) {
                sequenceMode = 2;  
            }
        }
    }
    return 0;
}