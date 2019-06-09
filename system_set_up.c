#include "system_set_up.h"
#include "interrupt.h"

#include "ui_statemachine.h"

uint16_t uiSystemCounter;

void vSystemSetUp(void)
{
    clock=SysCtlClockFreqSet(SYSCTL_OSC_INT | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480, SYS_CLK);

    tsSystemTime.uiSecond=0;
    tsSystemTime.uiMinute=0;
    tsSystemTime.uiHour=0;
    tsSystemTime.uiDay=0;
    tsSystemTime.uiMonth=0;
    tsSystemTime.uiYear=0;

    uiSystemStandbyTimeSeconds=SYSTEM_STANDBY_TIME_INIT;
    uiSystemResetTimeSeconds=SYSTEM_RESET_TIME_INIT;

    //Disable systemtime for more performance
    //SysTickPeriodSet(SYS_CLK/10); //interrupt every 100ms

    //SysTickIntRegister(vSystemSysTickHandler);

    //IntPrioritySet(15,0x20); //15 is the place of the handler in Interrupt vector table

    //SysTickIntEnable();

    //SysTickEnable();
}

void vSystemSysTickHandler(void)
{
    if(tsSystemTime.ui100MilliSecond<9)
    {
        tsSystemTime.ui100MilliSecond++;
    }
    else if(tsSystemTime.uiSecond<59)
    {
        tsSystemTime.ui100MilliSecond=0;
        tsSystemTime.uiSecond++;

        //----------------GENERATING STANDBY TIMER---------------------------------------
        if(uiSystemStandbyCounterSeconds>1)
        {
            uiSystemStandbyCounterSeconds--;
        }
        else if(uiSystemStandbyCounterSeconds==1)
        {
            vUIStandby();
            uiSystemStandbyCounterSeconds--;
        }

        if(uiSystemResetCounterSeconds>1)
        {
            uiSystemResetCounterSeconds--;
        }
        else if(uiSystemResetCounterSeconds==1)
        {
            vUIReset();
            uiSystemResetCounterSeconds--;
        }
        //----------------GENERATING STANDBY TIMER---------------------------------------

    }
    else if(tsSystemTime.uiMinute<59)
    {
        tsSystemTime.ui100MilliSecond=0;
        tsSystemTime.uiSecond=0;
        tsSystemTime.uiMinute++;
    }
    else if(tsSystemTime.uiHour<23)
    {
        tsSystemTime.ui100MilliSecond=0;
        tsSystemTime.uiSecond=0;
        tsSystemTime.uiMinute=0;
        tsSystemTime.uiHour++;
    }
    else
    {
        tsSystemTime.ui100MilliSecond=0;
        tsSystemTime.uiSecond=0;
        tsSystemTime.uiMinute=0;
        tsSystemTime.uiHour=0;
        tsSystemTime.uiDay++;
    }
}
