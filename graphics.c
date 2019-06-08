#include "graphics.h"

#include "timer.h"
#include "sysctl.h"
#include "interrupt.h"
#include "hw_ints.h"

#include "led_controller_interface.h"
#include "settings.h"


uint16_t uiGraphicsMode2Counter;


uint16_t uiGraphicsCounter;
uint16_t uiGraphicsCounter1;

uint8_t uiGraphicsBitField[SETTINGS_SLA_LENGTH_MAX][BIT_SEQUENCE_LENGTH];

void vGraphicsInit(void)
{
    //Enable Clock for graphics timer
    SysCtlPeripheralEnable(GRAPHICS_SYSCTL_TIMER);

    //wait 3 clocksycles for SysCtlPeripheralEnable
    asm("\tnop;\r\n\tnop;\r\n\tnop;\r\n");

    //Set graphics timer clock source as system clock
    TimerClockSourceSet(GRAPHICS_TIMER,TIMER_CLOCK_SYSTEM);

    //Disable graphics timer for configuring
    TimerDisable(GRAPHICS_TIMER,TIMER_A);

    //Configure graphics timer as full width periodic timer
    TimerConfigure(GRAPHICS_TIMER, TIMER_CFG_PERIODIC);

    //Load Value for 30 ms counting time
    TimerLoadSet(GRAPHICS_TIMER, TIMER_A, GRAPHICS_FRAME_PERIOD);

    //Clear graphics timer timeout interrupt flag
    TimerIntClear(GRAPHICS_TIMER, TIMER_TIMA_TIMEOUT);

    //Set interrupt priority
    IntPrioritySet(INT_TIMER2A,0x20);

    //Register interrupt in IVT
    TimerIntRegister(GRAPHICS_TIMER,TIMER_A,vGraphicsHandler);

    //Enable graphics timer interrupt
    TimerIntEnable(GRAPHICS_TIMER,TIMER_TIMA_TIMEOUT);

    //Enable graphics timer A
    TimerEnable(GRAPHICS_TIMER, TIMER_A);

}

void vGraphicsHandler(void)
{
    vGraphicsModeCalc();
    vGraphicsTransmissionInitiate();

    TimerIntClear(GRAPHICS_TIMER,TIMER_TIMA_TIMEOUT);
}

void vGraphicsModeCalc()
{
    uint8_t uiCounterSLA;

    for(uiCounterSLA=0;uiCounterSLA<SETTINGS_SLA_AMOUNT;uiCounterSLA++)
    {
        switch(tsSettings[uiCounterSLA].uiModeActive[0])
        {
            case 0:vGraphicsMode0(uiCounterSLA);//everything clear
            break;
            case 1:vGraphicsMode1(uiCounterSLA);//constant RGB
            break;
            case 2:vGraphicsMode2(uiCounterSLA);//moving snake RGB
            break;
        }
    }

}

void vGraphicsTransmissionInitiate(void)
{
    while(uiLEDCITransmissionRun);

    for(uiGraphicsCounter=0;uiGraphicsCounter<SETTINGS_SLA_LENGTH_MAX;uiGraphicsCounter++)
    {
        for(uiGraphicsCounter1=0;uiGraphicsCounter1<24;uiGraphicsCounter1++)
        {
            uiLEDCIBitField[uiGraphicsCounter][uiGraphicsCounter1]=uiGraphicsBitField[uiGraphicsCounter][uiGraphicsCounter1];
        }
    }

    uiLEDCITransmissionRun=1;

    for(uiGraphicsCounter=0;uiGraphicsCounter<SETTINGS_SLA_LENGTH_MAX;uiGraphicsCounter++)
    {
        for(uiGraphicsCounter1=0;uiGraphicsCounter1<24;uiGraphicsCounter1++)
        {
            uiGraphicsBitField[uiGraphicsCounter][uiGraphicsCounter1]=0;
        }
    }
}

void vGraphicsLedSet(uint8_t uiSLAMask,uint16_t uiLED,tsColorStruct tsColor)
{
uint16_t i;
for(i=0;i<8;i++)
{
    if(tsColor.uiRed & (0x80 >> i))
    {
        uiGraphicsBitField[uiLED][i+8]|=uiSLAMask;
    }
    else
    {
        uiGraphicsBitField[uiLED][i+8]&=~uiSLAMask;
    }

    if(tsColor.uiGreen & (0x80 >> i))
    {
        uiGraphicsBitField[uiLED][i]|=uiSLAMask;
    }
    else
    {
        uiGraphicsBitField[uiLED][i]&=~uiSLAMask;
    }

    if(tsColor.uiBlue & (0x80 >> i))
    {
        uiGraphicsBitField[uiLED][i+16]|=uiSLAMask;
    }
    else
    {
        uiGraphicsBitField[uiLED][i+16]&=~uiSLAMask;
    }
}

}

void vGraphicsMode0(uint8_t uiSLA)
{


}

void vGraphicsMode1(uint8_t uiSLA)
{
    tsColorStruct tsColor={tsSettings[uiSLA].uiModeParameter[0][0]*tsSettings[uiSLA].uiModeParameter[0][3],tsSettings[uiSLA].uiModeParameter[0][1]*tsSettings[uiSLA].uiModeParameter[0][3],tsSettings[uiSLA].uiModeParameter[0][2]*tsSettings[uiSLA].uiModeParameter[0][3]};
    uint16_t uiCounter;

    for(uiCounter=0;uiCounter<tsSettings[uiSLA].uiSLALength;uiCounter++)
    {
        vGraphicsLedSet(1<<uiSLA,uiCounter,tsColor);
    }
}

void vGraphicsMode2(uint8_t uiSLA)
{
    tsColorStruct tsColor={tsSettings[uiSLA].uiModeParameter[0][0]*tsSettings[uiSLA].uiModeParameter[0][3],tsSettings[uiSLA].uiModeParameter[0][1]*tsSettings[uiSLA].uiModeParameter[0][3],tsSettings[uiSLA].uiModeParameter[0][2]*tsSettings[uiSLA].uiModeParameter[0][3]};
    uint16_t uiCounter;
    uint16_t uiTopBoundary,uiBottomBoundary;

    tsSettings[uiSLA].uiModeActors[1]++;

    if(tsSettings[uiSLA].uiModeActors[1]>255-tsSettings[uiSLA].uiModeParameter[0][5])
    {
        tsSettings[uiSLA].uiModeActors[0]++;

        if(tsSettings[uiSLA].uiModeActors[0]>=tsSettings[uiSLA].uiSLALength+tsSettings[uiSLA].uiModeParameter[0][4])
        {
            tsSettings[uiSLA].uiModeActors[0]=0;
        }

        tsSettings[uiSLA].uiModeActors[1]=0;
    }

    if(tsSettings[uiSLA].uiModeActors[0] - tsSettings[uiSLA].uiModeParameter[0][4] < 0)  //cut if its too long for SLA
    {
        uiBottomBoundary=0;
    }
    else
    {
        uiBottomBoundary=tsSettings[uiSLA].uiModeActors[0] - tsSettings[uiSLA].uiModeParameter[0][4];
    }

    uiTopBoundary=tsSettings[uiSLA].uiModeActors[0];

    if(uiTopBoundary >= tsSettings[uiSLA].uiSLALength)
    {
        uiTopBoundary=tsSettings[uiSLA].uiSLALength;
    }

    for(uiCounter=uiBottomBoundary;uiCounter<uiTopBoundary;uiCounter++)
    {
        vGraphicsLedSet(1<<uiSLA,uiCounter,tsColor);
    }
}
