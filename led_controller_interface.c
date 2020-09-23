#include "led_controller_interface.h"

#include "tm4c1294ncpdt.h"
#include "system_set_up.h"
#include "gpio.h"
#include "timer.h"
#include "sysctl.h"
#include "interrupt.h"


//State if Transmitter
uint8_t uiLEDCIState=LED_CONTROLLER_INTERFACE_STATE_RESET;

/*
 * Output Mask is masking out disabled output pins
 * or holds several output pins on idle state (low Voltage) if all data for some connected led arrays
 * already have been sent but not for others.
 */
uint8_t uiLEDCIOutputMask;
uint8_t uiLEDCIOutputMaskInit;

uint16_t uiLEDCIBitSequenceCounter=0;
uint16_t uiLEDCIDataPackageCounter=0;


void vLEDControllerInterfaceInit(void)
{
    //Enable Clock for led controller interface output GPIO port
    SysCtlPeripheralEnable(LED_CONTROLLER_INTERFACE_SYSCTL_OUTPUT_PORT);
    //wait 3 clocksycles for SysCtlPeripheralEnable
    asm("\tnop;\r\n\tnop;\r\n\tnop;\r\n");

    //Use led controller interface output GPIO port as Digital IO
    GPIOPadConfigSet(LED_CONTROLLER_INTERFACE_OUTPUT_PORT,LED_CONTROLLER_INTERFACE_OUTPUT_PINS,GPIO_STRENGTH_8MA,GPIO_PIN_TYPE_STD);  //select pins defined in LED_CONTROLLER_INTERFACE_OUTPUT_PINS

    //Use led controller interface output GPIO port as digital outputs
    GPIODirModeSet(LED_CONTROLLER_INTERFACE_OUTPUT_PORT,LED_CONTROLLER_INTERFACE_OUTPUT_PINS,GPIO_DIR_MODE_OUT);   //select pins defined in LED_CONTROLLER_INTERFACE_OUTPUT_PINS

    //Enable Clock for NZR-Bit-Timer
    SysCtlPeripheralEnable(LED_CONTROLLER_INTERFACE_SYSCTL_TIMER_BIT);
    //wait 3 clocksycles for SysCtlPeripheralEnable
    asm("\tnop;\r\n\tnop;\r\n\tnop;\r\n");

    //Disable timer for configuration
    TimerDisable(LED_CONTROLLER_INTERFACE_TIMER_BIT, TIMER_A);

    //Configure Bit-Timer as periodic with full width
    TimerConfigure(LED_CONTROLLER_INTERFACE_TIMER_BIT, TIMER_CFG_PERIODIC);

    //Load timers with an interval for 2.5 micro seconds
    TimerLoadSet(LED_CONTROLLER_INTERFACE_TIMER_BIT,TIMER_A,TIMER_INTERVAL_2us5);

    //Set interrupt priority
    //LEDControllerInterface need to get the highest priority for fluid transmission
    IntPrioritySet(INT_TIMER1A,0x0);

    //Clear timer interrupt flags
    TimerIntClear(LED_CONTROLLER_INTERFACE_TIMER_BIT, TIMER_TIMA_TIMEOUT);
    //TimerIntClear(LED_CONTROLLER_INTERFACE_TIMER_HL, TIMER_TIMA_TIMEOUT | TIMER_TIMB_TIMEOUT);

    //Register Interrupts and write IVT in RAM
    TimerIntRegister(LED_CONTROLLER_INTERFACE_TIMER_BIT,TIMER_A,LEDControllerInterfaceBitHandler);

    //Enable Timer Interrupts
    TimerIntEnable(LED_CONTROLLER_INTERFACE_TIMER_BIT, TIMER_TIMA_TIMEOUT);

    TimerDisable(LED_CONTROLLER_INTERFACE_TIMER_BIT, TIMER_A);
    TIMER1_CTL_R|=0x001;

    //calculate initial Outputmask for every transmit
    uiLEDCIOutputMaskInit = SETTINGS_SLA0_ENABLE | (SETTINGS_SLA1_ENABLE<<1) | (SETTINGS_SLA2_ENABLE<<2) | (SETTINGS_SLA3_ENABLE<<3) | (SETTINGS_SLA4_ENABLE<<4) | (SETTINGS_SLA5_ENABLE<<5) | (SETTINGS_SLA6_ENABLE<<6) | (SETTINGS_SLA7_ENABLE<<7);

    uiLEDCITransmissionRun=0;
}

void LEDControllerInterfaceBitHandler(void)
{
    //Clear interrupt flag
    TimerIntClear(LED_CONTROLLER_INTERFACE_TIMER_BIT, TIMER_TIMA_TIMEOUT);

    //Enter statemachine of transmission process
    switch(uiLEDCIState)
    {
        //Transmitter in state of reset impulse or in idle
        case LED_CONTROLLER_INTERFACE_STATE_RESET:

            //Reset impulse
            if(uiLEDCIBitSequenceCounter<21)
            {
                vLEDControllerInterfaceOutputSet(0x0 & uiLEDCIOutputMask);
                uiLEDCIBitSequenceCounter++;
            }
            else
            {
                //Change state to transmission if permission granted. Otherwise leave connection in idle.
                if(uiLEDCITransmissionRun)
                {
                    //vLEDControllerInterfaceOutputSet(0xFF & uiLEDCIOutputMask);
                    uiLEDCIState=LED_CONTROLLER_INTERFACE_STATE_TRANSMISSION;
                    uiLEDCIBitSequenceCounter=0;
                    //Reset Outputmask
                    uiLEDCIOutputMask=uiLEDCIOutputMaskInit;
                }
            }

           break;

        //Transmitter in state of transmission
        case LED_CONTROLLER_INTERFACE_STATE_TRANSMISSION:

            if(uiLEDCIBitSequenceCounter<BIT_SEQUENCE_LENGTH)
            {
                vLEDControllerInterfaceOutputSet(0xFF & uiLEDCIOutputMask);
                while(TIMER1_TAV_R > TIMER_INTERVAL_2us5-TIMER_INTERVAL_0_OFFSET);
                vLEDControllerInterfaceOutputSet(uiLEDCIBitField[uiLEDCIDataPackageCounter][uiLEDCIBitSequenceCounter] & uiLEDCIOutputMask);
                while(TIMER1_TAV_R > TIMER_INTERVAL_2us5-TIMER_INTERVAL_1_OFFSET);
                vLEDControllerInterfaceOutputSet(0x0 & uiLEDCIOutputMask);
                uiLEDCIBitSequenceCounter++;
            }
            else
            {
                uiLEDCIDataPackageCounter++;
                if(uiLEDCIDataPackageCounter < SETTINGS_SLA_LENGTH_MAX)
                {
                    vLEDControllerInterfaceOutputMaskSet();
                }
                else
                {
                    uiLEDCIState=LED_CONTROLLER_INTERFACE_STATE_RESET;
                    uiLEDCIDataPackageCounter=0;
                    uiLEDCITransmissionRun=0;
                }
                vLEDControllerInterfaceOutputSet(0x0 & uiLEDCIOutputMask);
                uiLEDCIBitSequenceCounter=0;
            }

        break;
    }
}

void inline vLEDControllerInterfaceOutputSet(uint8_t uiOutputBitfield)
{
    GPIOPinWrite(LED_CONTROLLER_INTERFACE_OUTPUT_PORT,LED_CONTROLLER_INTERFACE_OUTPUT_PINS,uiOutputBitfield);      //write on pins defined in LED_CONTROLLER_INTERFACE_OUTPUT_PINS
}

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
