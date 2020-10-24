#include "led_controller_interface.h"

#include "tm4c1294ncpdt.h"
#include "system_set_up.h"
#include "gpio.h"
#include "timer.h"
#include "sysctl.h"
#include "interrupt.h"
#include "udma.h"

//uDMA control table
uint8_t uDMAControlTable[1024];

/*
 * Output Mask is masking out disabled output pins
 * or holds several output pins on idle state (low Voltage) if all data for some connected led arrays
 * already have been sent but not for others.
 */
uint8_t uiLEDCIOutputMask;
uint8_t uiLEDCIOutputMaskInit;

//Current index of buffer;
uint16_t uiLEDCIDataCounter = SETTINGS_SLA_LENGTH_MAX*BIT_SEQUENCE_LENGTH;
uint16_t uiResetCounter = 0;

uint8_t transmissionStopped = 0;

void vLEDControllerInterfaceInit(void)
{
    //Setup uDMA channel 20 for timer 1 triggered data transfer to GPIO Port K
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UDMA);
    asm("\tnop;\r\n\tnop;\r\n\tnop;\r\n");

    uDMAEnable();

    uDMAControlBaseSet(uDMAControlTable);
    uDMAChannelControlSet(20 | UDMA_PRI_SELECT, UDMA_SIZE_8 | UDMA_SRC_INC_8 | UDMA_DST_INC_NONE | UDMA_ARB_1);
    uDMAChannelControlSet(20 | UDMA_ALT_SELECT, UDMA_SIZE_8 | UDMA_SRC_INC_8 | UDMA_DST_INC_NONE | UDMA_ARB_1);

    uDMAChannelAssign(UDMA_CHANNEL_TMR1A);
    uDMAIntRegister(20, uDMAPingPongHandler);
    uDMAChannelDisable(20);


    //Enable Clock for led controller interface output GPIO port
    SysCtlPeripheralEnable(LED_CONTROLLER_INTERFACE_SYSCTL_OUTPUT_PORT);
    //wait 3 clocksycles for SysCtlPeripheralEnable
    asm("\tnop;\r\n\tnop;\r\n\tnop;\r\n");

    //Use led controller interface output GPIO port as Digital IO
    GPIOPadConfigSet(LED_CONTROLLER_INTERFACE_OUTPUT_PORT,LED_CONTROLLER_INTERFACE_OUTPUT_PINS,GPIO_STRENGTH_8MA,GPIO_PIN_TYPE_STD);  //select pins defined in LED_CONTROLLER_INTERFACE_OUTPUT_PINS

    //Use led controller interface output GPIO port as digital outputs
    GPIODirModeSet(LED_CONTROLLER_INTERFACE_OUTPUT_PORT,LED_CONTROLLER_INTERFACE_OUTPUT_PINS,GPIO_DIR_MODE_OUT);   //select pins defined in LED_CONTROLLER_INTERFACE_OUTPUT_PINS

    //Enable Clock for NZR-Bit-Timer
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER3);
    //wait 3 clocksycles for SysCtlPeripheralEnable
    asm("\tnop;\r\n\tnop;\r\n\tnop;\r\n");

    //Disable timer for configuration
    TimerDisable(TIMER2_BASE, TIMER_A);
    TimerDisable(TIMER1_BASE, TIMER_A);
    TimerDisable(TIMER3_BASE, TIMER_A);

    //Configure Bit-Timer as periodic with full width
    TimerConfigure(TIMER2_BASE, TIMER_CFG_PERIODIC);
    TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);
    TimerConfigure(TIMER3_BASE, TIMER_CFG_PERIODIC);

    TIMER2_TAMR_R |= 0x20;
    TIMER3_TAMR_R |= 0x20;

    //Load timer with an interval for 2.5 micro seconds
    TimerLoadSet(TIMER2_BASE,TIMER_A,TIMER_INTERVAL_BIT_INTERRUPT);
    TimerLoadSet(TIMER1_BASE,TIMER_A,TIMER_INTERVAL_BIT_INTERRUPT);
    TimerLoadSet(TIMER3_BASE,TIMER_A,TIMER_INTERVAL_BIT_INTERRUPT);

    TimerMatchSet(TIMER1_BASE,TIMER_A,TIMER_INTERVAL_BIT_INTERRUPT-TIMER_INTERVAL_0us35);
    TimerMatchSet(TIMER3_BASE,TIMER_A,TIMER_INTERVAL_BIT_INTERRUPT-TIMER_INTERVAL_0us9);

    //Set interrupt priority
    //LEDControllerInterface need to get the highest priority for fluid transmission
    IntPrioritySet(INT_TIMER2A,0x2);
    //IntPrioritySet(INT_TIMER1A,0x2);
    IntPrioritySet(INT_TIMER3A,0x2);

    //Clear timer interrupt flags
    TimerIntClear(TIMER2_BASE, TIMER_TIMA_TIMEOUT);
    TIMER1_ICR_R |= 0x10;
    TIMER3_ICR_R |= 0x10;

    //Register Interrupts and write IVT in RAM
    TimerIntRegister(TIMER2_BASE,TIMER_A,highHandler);
    //TimerIntRegister(TIMER1_BASE,TIMER_A,dataHandler);
    TimerIntRegister(TIMER3_BASE,TIMER_A,lowHandler);

    TimerDMAEventSet(TIMER1_BASE, TIMER_DMA_MODEMATCH_A);

    //Enable timer interrupt of low handler
    //TIMER1_IMR_R |= 0x01;
    //TIMER2_IMR_R |= 0x10;
    TIMER3_IMR_R |= 0x10;

    //Start timer
    TIMER2_CTL_R|=0x001;
    TIMER1_CTL_R|=0x001;
    TIMER3_CTL_R|=0x001;

    //Synchronize timer
    TimerSynchronize(TIMER0_BASE,TIMER_1A_SYNC | TIMER_2A_SYNC |  TIMER_3A_SYNC);

    //Calculate initial Outputmask for every transmit
    uiLEDCIOutputMaskInit = SETTINGS_SLA0_ENABLE | (SETTINGS_SLA1_ENABLE<<1) | (SETTINGS_SLA2_ENABLE<<2) | (SETTINGS_SLA3_ENABLE<<3) | (SETTINGS_SLA4_ENABLE<<4) | (SETTINGS_SLA5_ENABLE<<5) | (SETTINGS_SLA6_ENABLE<<6) | (SETTINGS_SLA7_ENABLE<<7);

    uiLEDCITransmissionRun=0;

    TransmitterBufferIndex = 0;
    TransmitterBufferPtr = uiLEDCIDoubleBuffer[TransmitterBufferIndex];
}

void uDMAPingPongHandler(void)
{
    int16_t uiTransferSize;

    //Update size of transfered data
    uiLEDCIDataCounter += 1024;
    uiTransferSize = SETTINGS_SLA_LENGTH_MAX*BIT_SEQUENCE_LENGTH - uiLEDCIDataCounter;

    //Determine next transfer size
    if(uiTransferSize > 1024)
    {
        uiTransferSize = 1024;
    }
    else if(uiTransferSize < 0)
    {
        uiTransferSize = 0;
    }

    if(uiTransferSize)
    {
        //Configure inactive control struct
        if(uDMAChannelModeGet(20 | UDMA_ALT_SELECT) == UDMA_MODE_STOP)
        {
            uDMAChannelTransferSet(20 | UDMA_ALT_SELECT,UDMA_MODE_PINGPONG,TransmitterBufferPtr += uiLEDCIDataCounter,LED_CONTROLLER_INTERFACE_OUTPUT_DATA,(uint16_t) uiTransferSize);
        }
        else if(uDMAChannelModeGet(20 | UDMA_PRI_SELECT) == UDMA_MODE_STOP)
        {
            uDMAChannelTransferSet(20 | UDMA_PRI_SELECT,UDMA_MODE_PINGPONG,TransmitterBufferPtr += uiLEDCIDataCounter,LED_CONTROLLER_INTERFACE_OUTPUT_DATA,(uint16_t) uiTransferSize);
        }

        uDMAChannelEnable(20);
    }
}


void highHandler(void)
{
    //Clear interrupt flag
    TIMER2_ICR_R |= 0x01;

    //Set output high for the beginning of any data bit
    LED_CONTROLLER_INTERFACE_OUTPUT_DATA = 0xFF;
}

void dataHandler(void)
{
    TIMER1_ICR_R |= 0x10;

    //Set output high or low according to data
    LED_CONTROLLER_INTERFACE_OUTPUT_DATA = TransmitterBufferPtr[uiLEDCIDataCounter];
    uiLEDCIDataCounter++;
}

void lowHandler(void)
{
    TIMER3_ICR_R |= 0x10;

    //Set output low for the end of any data bit or the reset phase.
    LED_CONTROLLER_INTERFACE_OUTPUT_DATA = 0x0;

    //Check if one transmission is complete
    if(uiLEDCIDataCounter >= SETTINGS_SLA_LENGTH_MAX*BIT_SEQUENCE_LENGTH)
    {
        //Stop transmission once
        if(!transmissionStopped)
        {
            uiLEDCITransmissionRun = 0;
            transmissionStopped = 1;
        }

        //Disable interrupts of highHandler and dataHandler if one transmission completed.
        TIMER2_IMR_R &= 0xFE;
        TIMER1_IMR_R &= 0xEF;
        TIMER2_ICR_R |= 0x01;
        TIMER1_ICR_R |= 0x10;
        uDMAChannelDisable(20);

        //If reset time of above 50us is reached and permission of transmission is granted
        //enable interrupts of highHandler and lowHandler and reset transmission and reset counter.
        uiResetCounter++;
        if(uiResetCounter > 150 && uiLEDCITransmissionRun)
        {
            transmissionStopped = 0;
            uiResetCounter = 0;
            uiLEDCIDataCounter = 0;

            uDMAChannelTransferSet(UDMA_PRI_SELECT,UDMA_MODE_PINGPONG,TransmitterBufferPtr,LED_CONTROLLER_INTERFACE_OUTPUT_DATA,1024);
            uiLEDCIDataCounter += 1024;
            uDMAChannelTransferSet(UDMA_ALT_SELECT,UDMA_MODE_PINGPONG,TransmitterBufferPtr+1024,LED_CONTROLLER_INTERFACE_OUTPUT_DATA,1024);

            TIMER2_ICR_R |= 0x01;
            TIMER1_ICR_R |= 0x10;
            TIMER2_IMR_R |= 0x01;
            TIMER1_IMR_R |= 0x10;
            uDMAChannelEnable(20);
        }
    }
}

void inline vLEDControllerInterfaceOutputSet(uint8_t uiOutputBitfield)
{
    //write on pins defined in LED_CONTROLLER_INTERFACE_OUTPUT_PINS
    GPIOPinWrite(LED_CONTROLLER_INTERFACE_OUTPUT_PORT,LED_CONTROLLER_INTERFACE_OUTPUT_PINS,uiOutputBitfield);
}

/*
void inline vLEDControllerInterfaceOutputMaskSet(void)
{
    uint16_t uiCounter;

    for(uiCounter=0;uiCounter<SETTINGS_SLA_AMOUNT;uiCounter++)
    {
        if(uiLEDCIDataPackageCounter >= tsSettings[uiCounter].uiSLALength)
        {
                uiLEDCIOutputMask &= ~(0x1<<uiCounter);
        }
    }
}
*/
