#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"

#define BUFFER_SIZE 50

void enviar_string(const char* str)
{
    while (*str)
    {
        UARTCharPut(UART0_BASE, *str);  // Envía cada carácter
        str++;
    }
}

int main(void)
{
    // Configurar el reloj del sistema a 120 MHz
    SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | 
                        SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);

    // Habilitar UART0 y configurar los pines PA0 (RX) y PA1 (TX)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTConfigSetExpClk(UART0_BASE, 120000000, 9600,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    // Habilitar GPIO para botones PJ0 y PJ1 y configurarlos como entrada con resistencia pull-up
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    GPIOPadConfigSet(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1, 
                     GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    // Habilitar GPIO para el buzzer en PB4 y configurarlo como salida
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_4);
    // Inicialmente se apaga el buzzer
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4, 0);

    uint8_t prevPJ0 = 1;
    uint8_t prevPJ1 = 1;
    
    char buffer[BUFFER_SIZE];
    uint8_t buffer_index = 0;

    while (1)
    {
        uint8_t currentPJ0 = GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0) & GPIO_PIN_0;
        uint8_t currentPJ1 = GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_1) & GPIO_PIN_1;

        // Detectar flanco descendente en PJ0 (de 1 a 0)
        if (prevPJ0 && !currentPJ0)
        {
            enviar_string("motor1 \n");
        }

        // Detectar flanco descendente en PJ1 (de 1 a 0)
        if (prevPJ1 && !currentPJ1)
        {
            enviar_string("motor2 \n");
        }

        prevPJ0 = currentPJ0;
        prevPJ1 = currentPJ1;

        // Recepción no bloqueante de UART
        if(UARTCharsAvail(UART0_BASE))
        {
            char c = UARTCharGet(UART0_BASE);
            // Si se recibe fin de línea, se procesa el mensaje
            if(c == '\n' || c == '\r')
            {
                buffer[buffer_index] = '\0'; // Terminar cadena
                enviar_string("Recibido: ");
                enviar_string(buffer);
                enviar_string("\n");

                // Si el mensaje recibido es "buzzer", se enciende el buzzer; de lo contrario, se apaga.
                if(strcmp(buffer, "buzzer") == 0)
                {
                    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4, GPIO_PIN_4);
                }
                else
                {
                    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4, 0);
                }
                // Reiniciar el índice del buffer para la próxima recepción
                buffer_index = 0;
            }
            else
            {
                // Acumular el carácter en el buffer si hay espacio
                if(buffer_index < BUFFER_SIZE - 1)
                {
                    buffer[buffer_index++] = c;
                }
            }
        }
    }
}

