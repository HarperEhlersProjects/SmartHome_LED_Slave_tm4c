#include "data_receiver.h"

#include "tm4c1294ncpdt.h"

#include "led_controller_interface.h"
#include "settings.h"
#include "system_set_up.h"
#include "delay.h"
#include "udma.h"
#include "uart.h"
#include "pin_map.h"


#define DMA_TRANSFER_COUNT (24*255)
#define DMA_ITEM_SIZE UDMA_SIZE_8
#define DMA_SOURCE_INCREMENT UDMA_SRC_INC_NONE

#define DMA_DESTINATION_INCREMENT UDMA_DST_INC_8
#define DMA_ARB_SIZE UDMA_ARB_1024
#define DMA_MODE UDMA_MODE_BASIC

uint32_t uiDataReceiverCounter, uiDataReceiverCounter1,uiDataReceiverCounterFail;

uint8_t uiDataReceiverTransmissionOK;

//uint8_t uiDataReceiverBitField[SETTINGS_SLA_LENGTH_MAX][BIT_SEQUENCE_LENGTH];
uint8_t uiDataReceiverBuffer[SETTINGS_SLA_LENGTH_MAX*BIT_SEQUENCE_LENGTH] = {0};
uint16_t uiDataReceiverBufferIndex;

/*
const tDMAControlTable DMATaskList[] =
{
uDMATaskStructEntry(DMA_TRANSFER_COUNT,
                    DMA_ITEM_SIZE,
                    DMA_SOURCE_INCREMENT,
                    UART7_DR_R,
                    DMA_DESTINATION_INCREMENT,
                    uiDataReceiverBuffer,
                    DMA_ARB_SIZE,
                    DMA_MODE),

//uDMATaskStructEntry(Task2Count, ...),
};

*/

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
}


void vDataReceiverReceive(void)
{
    uiDataReceiverTransmissionOK = 0;
    while(!uiDataReceiverTransmissionOK)
    {
        while(UARTCharGetNonBlocking(UART7_BASE) >= 0);    //Clear FIFO
        UARTCharPut(UART7_BASE, 'r');

        while(!UARTCharsAvail(UART7_BASE))       //wait for ready Transmitter
        {
            //vDelayMiliSec(1);
            uiDataReceiverCounter1++;

            if(uiDataReceiverCounter1 > 2000000) //repeat request if timeout is reached
            {
                UARTCharPut(UART7_BASE, 'r');
                uiDataReceiverCounter1 = 0;
            }
        }

        if(UARTCharGetNonBlocking(UART7_BASE) == 'r')
        {
            UARTCharPut(UART7_BASE, 's');
            uiDataReceiverTransmissionOK = 1;
        }

        uiDataReceiverCounter = 0;
        uiDataReceiverCounter1 = 0;

        if(uiDataReceiverTransmissionOK)
        {
            while(uiDataReceiverCounter < SETTINGS_SLA_LENGTH_MAX*24)
            {

                if(UARTCharsAvail(UART7_BASE))
                {
                    //uiDataReceiverBuffer[uiDataReceiverCounter] = UARTCharGetNonBlocking(UART7_BASE);
                    uiDataReceiverBuffer[uiDataReceiverCounter] = UART7_DR_R;
                    uiDataReceiverCounter++;
                    //uiDataReceiverCounter1 = 0;
                }
                else
                {
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

void vDataReceiverTransmissionInitiate(void)
{

    while(uiLEDCITransmissionRun);

    for(uiDataReceiverCounter=0;uiDataReceiverCounter<SETTINGS_SLA_LENGTH_MAX;uiDataReceiverCounter++)
    {
        for(uiDataReceiverCounter1=0;uiDataReceiverCounter1<24;uiDataReceiverCounter1++)
        {
            uiLEDCIBitField[uiDataReceiverCounter][uiDataReceiverCounter1]=uiDataReceiverBuffer[uiDataReceiverCounter*24+uiDataReceiverCounter1];
        }
    }

    uiLEDCITransmissionRun=1;

}
