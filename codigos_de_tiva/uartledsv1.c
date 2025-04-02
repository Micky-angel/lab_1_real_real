#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "driverlib/pwm.h"

#define BUFFER_SIZE 50
#define PWM_FREQUENCY 1000

void enviar_string(const char* str)
{
    while (*str)
    {
        UARTCharPut(UART0_BASE, *str);
        str++;
    }
}

void configurar_pwm(void) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);
    
    GPIOPinConfigure(GPIO_PK4_M0PWM6);
    GPIOPinTypePWM(GPIO_PORTK_BASE, GPIO_PIN_4);
    GPIOPinConfigure(GPIO_PL5_M0PWM7);
    GPIOPinTypePWM(GPIO_PORTL_BASE, GPIO_PIN_5);
    
    uint32_t pwmClock = SysCtlClockGet() / 64;
    uint32_t pwmLoad = (pwmClock / PWM_FREQUENCY) - 1;
    
    PWMGenConfigure(PWM0_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_3, pwmLoad);
    
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_6, (pwmLoad * 90) / 100);
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_7, (pwmLoad * 90) / 100);
    
    PWMOutputState(PWM0_BASE, PWM_OUT_6_BIT | PWM_OUT_7_BIT, true);
    PWMGenEnable(PWM0_BASE, PWM_GEN_3);
}

void ajustar_duty_cycle(uint32_t motor, uint32_t duty) {
    if (duty > 100) duty = 100;
    uint32_t pwmClock = SysCtlClockGet() / 64;
    uint32_t pwmLoad = (pwmClock / PWM_FREQUENCY) - 1;
    
    if (motor == 1) {
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_6, (pwmLoad * duty) / 100);
    } else if (motor == 2) {
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_7, (pwmLoad * duty) / 100);
    }
}

int main(void)
{
    SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | 
                        SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTConfigSetExpClk(UART0_BASE, 120000000, 9600,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    GPIOPadConfigSet(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1, 
                     GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_4);
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4, 0);

    configurar_pwm();
    
    uint8_t prevPJ0 = 1;
    uint8_t prevPJ1 = 1;
    
    char buffer[BUFFER_SIZE];
    uint8_t buffer_index = 0;

    while (1)
    {
        uint8_t currentPJ0 = GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0) & GPIO_PIN_0;
        uint8_t currentPJ1 = GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_1) & GPIO_PIN_1;

        if (prevPJ0 && !currentPJ0) enviar_string("motor1 \n");
        if (prevPJ1 && !currentPJ1) enviar_string("motor2 \n");

        prevPJ0 = currentPJ0;
        prevPJ1 = currentPJ1;

        if(UARTCharsAvail(UART0_BASE))
        {
            char c = UARTCharGet(UART0_BASE);
            if(c == '\n' || c == '\r')
            {
                buffer[buffer_index] = '\0';
                enviar_string("Recibido: ");
                enviar_string(buffer);
                enviar_string("\n");
                
                if(strncmp(buffer, "duty1 ", 6) == 0)
                {
                    uint32_t duty = atoi(&buffer[6]);
                    ajustar_duty_cycle(1, duty);
                }
                else if(strncmp(buffer, "duty2 ", 6) == 0)
                {
                    uint32_t duty = atoi(&buffer[6]);
                    ajustar_duty_cycle(2, duty);
                }
                else if(strcmp(buffer, "buzzer") == 0)
                {
                    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4, GPIO_PIN_4);
                }
                else
                {
                    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4, 0);
                }
                buffer_index = 0;
            }
            else
            {
                if(buffer_index < BUFFER_SIZE - 1)
                {
                    buffer[buffer_index++] = c;
                }
            }
        }
    }
}
