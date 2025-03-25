#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "utils/uartstdio.c"

#define BUTTON_PJ0 GPIO_PIN_0  // Internal button for motor 1
#define BUTTON_PE0 GPIO_PIN_0  // External button for motor 2
#define BUZZER_PE4 GPIO_PIN_4  // Buzzer connected to PE4

void InitializeSystem() {
    uint32_t reloj = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);
    
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ)) {}
    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, BUTTON_PJ0);
    GPIOPadConfigSet(GPIO_PORTJ_BASE, BUTTON_PJ0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE)) {}
    GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, BUTTON_PE0);
    GPIOPadConfigSet(GPIO_PORTE_BASE, BUTTON_PE0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);  // No pull
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, BUZZER_PE4);
    GPIOPinWrite(GPIO_PORTE_BASE, BUZZER_PE4, 0);
    
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0)) {}
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTStdioConfig(0, 9600, reloj);
}

void ActivateBuzzer() {
    GPIOPinWrite(GPIO_PORTE_BASE, BUZZER_PE4, BUZZER_PE4);  // Turn on buzzer
}

int main(void) {
    InitializeSystem();
    uint8_t prevButtonStateJ0 = 1;  // Previous state for PJ0 (motor 1)
    uint8_t prevButtonStateE0 = 1;  // Previous state for PE0 (motor 2)
    char inputStr[20];

    while (1) {
        uint8_t currentButtonStateJ0 = GPIOPinRead(GPIO_PORTJ_BASE, BUTTON_PJ0);
        uint8_t currentButtonStateE0 = GPIOPinRead(GPIO_PORTE_BASE, BUTTON_PE0);
        
        if (prevButtonStateJ0 == 1 && currentButtonStateJ0 == 0) {
            UARTprintf("motor 1\n");  // Send "motor 1" when PJ0 pressed
        }
        
        if (prevButtonStateE0 == 1 && currentButtonStateE0 == 0) {
            UARTprintf("motor 2\n");  // Send "motor 2" when PE0 pressed
        }
        
        prevButtonStateJ0 = currentButtonStateJ0;
        prevButtonStateE0 = currentButtonStateE0;
        
        if (UARTCharsAvail(UART0_BASE)) {
            UARTgets(inputStr, sizeof(inputStr));
            if (strcmp(inputStr, "buzzer") == 0) {
                ActivateBuzzer();  // Activate buzzer on "buzzer" message
            }
        }
    }
    return 0;
}