#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "inc/hw_ints.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"



uint32_t timer0Load;
char uartChar = 'a';
uint8_t estadoR = 0;
uint8_t estadoG = 0;
uint8_t estadoB = 0;
uint8_t colorON = 0;
uint8_t ledState = 0;


int main(void)
{
    //-------------------------------------- OSCILADOR ---------------------------------------
    // Configurar oscilador en 40MHz
    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
    // Configurar el reloj al puerto F
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    // Configurar el reloj al puerto A
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    // Configurar el reloj al Timer0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    // Configurar el reloj al UART0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //---------------------------------------- GPIOS -----------------------------------------
    // Configurar como salidas los pines 1, 2 y 3 del puerto F (LEDs)
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
    // Configurar como UART los pines 0 y 1 del puerto A
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);


    //---------------------------------------- TIMER0 ----------------------------------------
    // Carga de tiempo en 0.5Hz
    timer0Load = (SysCtlClockGet() / 0.5);
    // Configurar timer0 como un temporizador periodico
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    // Asignar carga de tiempo al temporizador
    TimerLoadSet(TIMER0_BASE, TIMER_BOTH, timer0Load - 1 );

    // Activar comunicación UART a 115200
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200, UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE);
    // Activar banderas de recepcion
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
    // Activar interrupcion de uart0
    IntEnable(INT_UART0);
    // Activar UART0
    UARTEnable(UART0_BASE);

    //-------------------------------------- INTERRUPT ---------------------------------------
    // Interrupcion de timer0
    IntEnable(INT_TIMER0A);
    // Se habilita interrupción por Timeout
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    // Se habilitan las interrupciones Globales
    IntMasterEnable();
    // Se habilita el Timer
    TimerEnable(TIMER0_BASE, TIMER_BOTH);



    while (1)
    {
    }
}


void Timer0IntHandler(void)
{
    // Limpiar la bandera de interrupcion
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    //
    if (ledState == 0)
    {
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, colorON);
        ledState = 1;
    }
    else
    {
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0x0);
        ledState = 0;
    }
}

void UARTIntHandler(void)
{
    // Limpiar la bandera de interrupcion
    TimerIntClear(UART0_BASE, UART_INT_RX | UART_INT_RT);
    // Realizar lectura del char entrante
    uartChar = UARTCharGetNonBlocking(UART0_BASE);

    if (uartChar == 'r' && estadoR == 0)
    {
        colorON = 0x2;
        estadoR = 1;
        estadoG = 0;
        estadoB = 0;
    }
    else if (uartChar == 'r' && estadoR == 1)
    {
        colorON = 0x0;
        estadoR = 0;
        estadoG = 0;
        estadoB = 0;
    }

    else if (uartChar == 'g' && estadoG == 0)
    {
        colorON = 0x8;
        estadoR = 0;
        estadoG = 1;
        estadoB = 0;
    }
    else if (uartChar == 'g' && estadoG == 1)
    {
        colorON = 0x0;
        estadoR = 0;
        estadoG = 0;
        estadoB = 0;
    }

    else if (uartChar == 'b' && estadoB == 0)
    {
        colorON = 0x4;
        estadoR = 0;
        estadoG = 0;
        estadoB = 1;
    }
    else if (uartChar == 'b' && estadoB == 1)
    {
        colorON = 0x0;
        estadoR = 0;
        estadoG = 0;
        estadoB = 0;
    }
}
