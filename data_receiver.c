#include "data_receiver.h"

#include "tm4c1294ncpdt.h"

#include "stdint.h"
#include "stdbool.h"

#include "sysctl.h"
#include "gpio.h"

#include "led_controller_interface.h"
#include "settings.h"
#include "system_set_up.h"
#include "delay.h"
#include "uart.h"
#include "pin_map.h"


//Data receiver configuration
#define DATARECEIVER_BAUDRATE 2000000

#define DATARECEIVER_UART_PINS (GPIO_PIN_4 | GPIO_PIN_5)
#define DATARECEIVER_UART_PORT GPIO_PORTC_BASE
#define DATARECEIVER_UART_SYSCTL_PORT SYSCTL_PERIPH_GPIOC


uint32_t uiDataReceiverCounter, uiDataReceiverCounter1,uiDataReceiverCounterFail;

uint8_t uiDataReceiverTransmissionOK;

uint8_t* ReceiverBufferPtr = 0;

void vDataReceiverInit(void)
{
    SysCtlPeripheralEnable(DATARECEIVER_UART_SYSCTL_PORT);

    asm("\tnop;\r\n\tnop;\r\n\tnop;\r\n");

    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART7);

    asm("\tnop;\r\n\tnop;\r\n\tnop;\r\n");

    GPIOPinTypeUART(DATARECEIVER_UART_PORT, DATARECEIVER_UART_PINS);

    GPIOPinConfigure(GPIO_PC4_U7RX);
    GPIOPinConfigure(GPIO_PC5_U7TX);

    UARTClockSourceSet(UART7_BASE, UART_CLOCK_SYSTEM);

    UARTConfigSetExpClk(UART7_BASE,SysCtlClockGet(), SysCtlClockGet()/30, UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE);

    UARTFIFOEnable(UART7_BASE);

    UARTEnable(UART7_BASE);

    uiDataReceiverCounterFail=0;

    ReceiverBufferPtr = uiLEDCIDoubleBuffer[1];
}


void vDataReceiverReceive(void)
{
    //vDelayMicroSec(1);

    uiDataReceiverTransmissionOK = 0;
    while(!uiDataReceiverTransmissionOK)
    {
        while(UARTCharGetNonBlocking(UART7_BASE) >= 0);    //Clear FIFO
        UARTCharPut(UART7_BASE, 'r');   //request ('r') data from transmitter

        while(!UARTCharsAvail(UART7_BASE))       //wait for received character
        {
            //repeat request every timeout interval
            uiDataReceiverCounter1++;
            if(uiDataReceiverCounter1 > 2000000)
            {
                UARTCharPut(UART7_BASE, 'r');
                uiDataReceiverCounter1 = 0;
            }
        }

        //If Transmitter sends back a ready signal acknowledge it by sending a start ('s') request
        if(UARTCharGetNonBlocking(UART7_BASE) == 'r')
        {
            UARTCharPut(UART7_BASE, 's');
            uiDataReceiverTransmissionOK = 1;   //give permission to receive data
        }

        uiDataReceiverCounter = 0;
        uiDataReceiverCounter1 = 0;

        //Recieve data when permission granted
        if(uiDataReceiverTransmissionOK)
        {
            while(uiDataReceiverCounter < SETTINGS_SLA_LENGTH_MAX*24)
            {
                //Write received data into receiver buffer
                if(UARTCharsAvail(UART7_BASE))
                {
                    //uiDataReceiverBuffer[uiDataReceiverCounter] = UART7_DR_R;
                    //Double buffer implementation
                    ReceiverBufferPtr[uiDataReceiverCounter] = UART7_DR_R;
                    uiDataReceiverCounter++;
                }
                else
                {
                    //Abort receiving process and signal transmission failure ('f') when transmission takes to long
                    uiDataReceiverCounter1++;
                    if(uiDataReceiverCounter1 > 100000)
                    {
                        UARTCharPut(UART7_BASE, 'f');
                        uiDataReceiverTransmissionOK = 0;
                        uiDataReceiverCounter = SETTINGS_SLA_LENGTH_MAX*24;
                        uiDataReceiverCounterFail++;
                    }
                }
            }
        }
    }
}


//Move received data to the output buffer and set the permission for transmission to the led arrays
void vDataReceiverTransmissionInitiate(void)
{

    while(uiLEDCITransmissionRun);

    //Rotate double buffer
    TransmitterBufferIndex = !TransmitterBufferIndex;
    ReceiverBufferPtr = uiLEDCIDoubleBuffer[!TransmitterBufferIndex];
    TransmitterBufferPtr = uiLEDCIDoubleBuffer[TransmitterBufferIndex];

    //give permission to transmit
    uiLEDCITransmissionRun=1;
}
