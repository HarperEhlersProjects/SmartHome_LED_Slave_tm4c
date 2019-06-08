#include "delay.h"

#include "system_set_up.h"



/*
 * Initialise DELAY_TIMER as Timer for delay functions
 *
 */
void vDelayInit(void)
{
    //Enable Clock for Timer 0
    SysCtlPeripheralEnable(DELAY_SYSCTL_TIMER);

    //wait 3 clocksycles for SysCtlPeripheralEnable
    asm("\tnop;\r\n\tnop;\r\n\tnop;\r\n");

    //Set Timer 0 clock source as system clock
    TimerClockSourceSet(DELAY_TIMER,TIMER_CLOCK_SYSTEM);

    //DIsable TImer 0 for configuring
    TimerDisable(DELAY_TIMER,TIMER_A);

    //Configure Timer 0 as half width periodic timer
    TimerConfigure(DELAY_TIMER, TIMER_CFG_A_PERIODIC | TIMER_CFG_A_ACT_NONE);

    //Configure Value for Match-Interrupt
    TimerMatchSet(DELAY_TIMER, TIMER_A, 0);

    //Load Value for 1 MicroSecond counting time
    TimerLoadSet(DELAY_TIMER, TIMER_A, SYS_CLK/1000000);

    //Clear Timer 0 Match Interrupt Flag
    TimerIntClear(DELAY_TIMER, TIMER_TIMA_MATCH);

    //Enable TImer0_A
    TimerEnable(DELAY_TIMER, TIMER_A);
}


void vDelaySec(uint32_t uiTimeSec)
{
    vDelayMicroSec(uiTimeSec*1000000);
}

void vDelayMiliSec(uint32_t uiTimeMiliSec)
{
    vDelayMicroSec(uiTimeMiliSec*1000);
}

void vDelayMicroSec(uint32_t uiTimeMicroSec)
{
    uint32_t uiMicroSecCounter = 0;

    //Load Value for 1 MicroSecond counting time
    TimerLoadSet(DELAY_TIMER, TIMER_A, SYS_CLK/1000000);

    while(uiMicroSecCounter <= uiTimeMicroSec)
    {
        //clear Timer 0 Match Interrupt Flag
        TimerIntClear(DELAY_TIMER, TIMER_TIMA_MATCH);
        //Wait for Timer0 Match Interrupt
        while((~TimerIntStatus(DELAY_TIMER,false)) & TIMER_TIMA_MATCH);

        uiMicroSecCounter++;
    }
}
