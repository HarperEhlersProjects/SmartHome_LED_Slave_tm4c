#include "ui_statemachine.h"

#include "i2c_interface.h"
#include "system_set_up.h"

//combine these makros for setting up ui leds
#define UI_LED_BLANK 0x0
#define UI_MODELED_1 0x8
#define UI_MODELED_2 0x10
#define UI_MODELED_3 0x20
#define UI_RGBLED_RED 0x1
#define UI_RGBLED_GREEN 0x4
#define UI_RGBLED_BLUE 0x2
#define UI_RGBLED_ORANGE (UI_RGBLED_RED | UI_RGBLED_GREEN)
#define UI_RGBLED_PURPLE (UI_RGBLED_RED | UI_RGBLED_BLUE)
#define UI_RGBLED_TURKIS (UI_RGBLED_GREEN | UI_RGBLED_BLUE)
#define UI_RGBLED_WHITE (UI_RGBLED_RED | UI_RGBLED_GREEN | UI_RGBLED_BLUE)

uint8_t uiSevenSegmentLUT[UI_SYMBOLS_NUMBER] = {0x01,0x37,0x42,0x12,0x34,0x18,0x08,0x33,0x0,0x10,0x7F};

uint8_t uiUIDisplay00;
uint8_t uiUIDisplay01;

uint8_t uiUIDisplay10;
uint8_t uiUIDisplay11;
uint8_t uiUIDisplay12;

uint8_t uiUILED;

uint16_t uiUICounter;
uint8_t uiUICounterSLA;

void vUIInit()
{
    uiUIState=UI_STATE_STANDBY;
    uiUIStandbyPreviousState=UI_STATE_SLA_SELECTION;

    for(uiUICounter=0;uiUICounter<8;uiUICounter++)
    {
        uiUISLAActive[uiUICounter]=0x0;
    }

    uiUIMode=0;

    for(uiUICounter=1;uiUICounter<SETTINGS_MODIFIER_AMOUNT+1;uiUICounter++)
    {
        uiUIModeActive[uiUICounter]=0x2;
    }

    uiUIModeActive[0]=UI_MODE_NO_SELECTION;

    vUIUpdate();
}

void vUIUpdate(void)
{
    switch(uiUIState)
    {
        case UI_STATE_STANDBY:
                                    uiUIDisplay00=UI_SYMBOLS_BLANK;
                                    uiUIDisplay01=UI_SYMBOLS_BLANK;

                                    uiUIDisplay10=UI_SYMBOLS_BLANK;
                                    uiUIDisplay11=UI_SYMBOLS_BLANK;
                                    uiUIDisplay12=UI_SYMBOLS_BLANK;

                                    uiUILED=0x0;
        break;
        case UI_STATE_SLA_SELECTION:
                                    uiUIDisplay00=uiUISLA;
                                    uiUIDisplay01=UI_SYMBOLS_BLANK;

                                    uiUIDisplay10=UI_SYMBOLS_BLANK;
                                    uiUIDisplay11=UI_SYMBOLS_BLANK;
                                    uiUIDisplay12=UI_SYMBOLS_BLANK;

                                    if(uiUISLAActive[uiUISLA])
                                    {
                                        uiUILED=UI_RGBLED_GREEN | UI_MODELED_1;
                                    }
                                    else
                                    {
                                        uiUILED=UI_RGBLED_RED | UI_MODELED_1;
                                    }
        break;
        case UI_STATE_MODE_SELECTION:

                                    if(uiUIMode<10)
                                    {
                                        uiUIDisplay00=uiUIMode;
                                        uiUIDisplay01=UI_SYMBOLS_BLANK;
                                    }
                                    else
                                    {
                                        uiUIDisplay00=uiUIMode%10;
                                        uiUIDisplay01=(int) (uiUIMode/10);
                                    }

                                    uiUIDisplay10=UI_SYMBOLS_BLANK;
                                    uiUIDisplay11=UI_SYMBOLS_BLANK;
                                    uiUIDisplay12=UI_SYMBOLS_BLANK;

                                    if(uiUIModeTypeGet() == UI_STATE_TYPE_MODE)
                                    {
                                        if(uiUIMode==uiUIModeActive[0])
                                        {
                                            uiUILED=UI_RGBLED_PURPLE | UI_MODELED_2;

                                        }
                                        else
                                        {
                                            uiUILED=UI_MODELED_2;

                                        }
                                    }
                                    else if(uiUIModeTypeGet() == UI_STATE_TYPE_MODIFIER)
                                    {
                                        if(uiUIModeActive[UI_MODIFIER_INDEX]==0x2)
                                        {
                                            uiUILED=UI_RGBLED_BLUE | UI_MODELED_2;
                                        }
                                        else if(uiUIModeActive[UI_MODIFIER_INDEX]==0x0)
                                        {
                                            uiUILED=UI_RGBLED_RED | UI_MODELED_2;
                                        }
                                        else
                                        {
                                            uiUILED=UI_RGBLED_GREEN | UI_MODELED_2;
                                        }
                                    }
                                    else
                                    {
                                        uiUILED=UI_RGBLED_WHITE | UI_MODELED_2;
                                    }
        break;
        case UI_STATE_PARAMETER_SELECTION:
                                    uiUIDisplay00=uiUIParameter;
                                    uiUIDisplay01=UI_SYMBOLS_BLANK;

                                    if(uiUIParameterValue<10)
                                    {
                                        uiUIDisplay10=uiUIParameterValue;
                                        uiUIDisplay11=UI_SYMBOLS_BLANK;
                                        uiUIDisplay12=UI_SYMBOLS_BLANK;
                                    }
                                    else if(uiUIParameterValue<100)
                                    {
                                        uiUIDisplay10=uiUIParameterValue%10;
                                        uiUIDisplay11=uiUIParameterValue/10;
                                        uiUIDisplay12=UI_SYMBOLS_BLANK;
                                    }
                                    else
                                    {
                                        uiUIDisplay10=uiUIParameterValue%10;
                                        uiUIDisplay11=(uiUIParameterValue/10)%10;
                                        uiUIDisplay12=uiUIParameterValue/100;
                                    }

                                    uiUILED=UI_MODELED_3;
        break;
        case UI_STATE_PARAMETER_SELECTED:
                                    uiUIDisplay00=uiUIParameter;
                                    uiUIDisplay01=UI_SYMBOLS_BLANK;

                                    if(uiUIParameterValue<10)
                                    {
                                        uiUIDisplay10=uiUIParameterValue;
                                        uiUIDisplay11=UI_SYMBOLS_BLANK;
                                        uiUIDisplay12=UI_SYMBOLS_BLANK;
                                    }
                                    else if(uiUIParameterValue<100)
                                    {
                                        uiUIDisplay10=uiUIParameterValue%10;
                                        uiUIDisplay11=uiUIParameterValue/10;
                                        uiUIDisplay12=UI_SYMBOLS_BLANK;
                                    }
                                    else
                                    {
                                        uiUIDisplay10=uiUIParameterValue%10;
                                        uiUIDisplay11=(uiUIParameterValue/10)%10;
                                        uiUIDisplay12=uiUIParameterValue/100;
                                    }

                                    uiUILED=UI_RGBLED_ORANGE | UI_MODELED_3;
        break;
        case UI_STATE_SETTINGS_SELECTION:
                                    if(uiUISetting<10)
                                    {
                                        uiUIDisplay00=uiUISetting;
                                        uiUIDisplay01=UI_SYMBOLS_BLANK;
                                    }
                                    else
                                    {
                                        uiUIDisplay00=uiUISetting%10;
                                        uiUIDisplay01=(int) (uiUISetting/10);
                                    }

                                    if(uiUISettingValue<10)
                                    {
                                        uiUIDisplay10=uiUISettingValue;
                                        uiUIDisplay11=UI_SYMBOLS_BLANK;
                                        uiUIDisplay12=UI_SYMBOLS_BLANK;
                                    }
                                    else if(uiUISettingValue<100)
                                    {
                                        uiUIDisplay10=uiUISettingValue%10;
                                        uiUIDisplay11=uiUISettingValue/10;
                                        uiUIDisplay12=UI_SYMBOLS_BLANK;
                                    }
                                    else
                                    {
                                        uiUIDisplay10=uiUISettingValue%10;
                                        uiUIDisplay11=(uiUISettingValue/10)%10;
                                        uiUIDisplay12=uiUISettingValue/100;
                                    }

                                    uiUILED=UI_MODELED_1 | UI_MODELED_2 | UI_MODELED_3;
        break;
        case UI_STATE_SETTINGS_SELECTED:
                                    if(uiUISetting<10)
                                    {
                                        uiUIDisplay00=uiUISetting;
                                        uiUIDisplay01=UI_SYMBOLS_BLANK;
                                    }
                                    else
                                    {
                                        uiUIDisplay00=uiUISetting%10;
                                        uiUIDisplay01=(int) (uiUISetting/10);
                                    }

                                    if(uiUISettingValue<10)
                                    {
                                        uiUIDisplay10=uiUISettingValue;
                                        uiUIDisplay11=UI_SYMBOLS_BLANK;
                                        uiUIDisplay12=UI_SYMBOLS_BLANK;
                                    }
                                    else if(uiUISettingValue<100)
                                    {
                                        uiUIDisplay10=uiUISettingValue%10;
                                        uiUIDisplay11=uiUISettingValue/10;
                                        uiUIDisplay12=UI_SYMBOLS_BLANK;
                                    }
                                    else
                                    {
                                        uiUIDisplay10=uiUISettingValue%10;
                                        uiUIDisplay11=(uiUISettingValue/10)%10;
                                        uiUIDisplay12=uiUISettingValue/100;
                                    }

                                    uiUILED=UI_RGBLED_WHITE | UI_MODELED_1 | UI_MODELED_2 | UI_MODELED_3;
        break;
    }

    vI2CInterfaceOutputWrite(1,uiSevenSegmentLUT[uiUIDisplay01]);
    vI2CInterfaceOutputWrite(0,uiSevenSegmentLUT[uiUIDisplay00]);

    vI2CInterfaceOutputWrite(3,uiSevenSegmentLUT[uiUIDisplay12]);
    vI2CInterfaceOutputWrite(2,uiSevenSegmentLUT[uiUIDisplay11]);
    vI2CInterfaceOutputWrite(5,uiSevenSegmentLUT[uiUIDisplay10]);

    vI2CInterfaceOutputWrite(4,uiUILED);
}

void vUIClear()
{
    vI2CInterfaceOutputWrite(0,uiSevenSegmentLUT[UI_SYMBOLS_BLANK]);
    vI2CInterfaceOutputWrite(1,uiSevenSegmentLUT[UI_SYMBOLS_BLANK]);

    vI2CInterfaceOutputWrite(2,uiSevenSegmentLUT[UI_SYMBOLS_BLANK]);
    vI2CInterfaceOutputWrite(3,uiSevenSegmentLUT[UI_SYMBOLS_BLANK]);
    vI2CInterfaceOutputWrite(4,uiSevenSegmentLUT[UI_SYMBOLS_BLANK]);

    vI2CInterfaceOutputWrite(5,UI_LED_BLANK);
}

void vUIStandby(void)
{
    if(uiUIState!=UI_STATE_STANDBY)
    {
        uiUIStandbyPreviousState=uiUIState;
    }

    uiUIState=UI_STATE_STANDBY;

    vUIUpdate();
}


void vUIReset(void)
{
    uint8_t uiCounter;

    uiUIStandbyPreviousState=UI_STATE_SLA_SELECTION;

    if(uiUIState != UI_STATE_STANDBY)
    {
        uiUIState=UI_STATE_SLA_SELECTION;
    }

    uiUISLA=0;
    uiUIMode=0;
    uiUIModeActive[0]=UI_MODE_NO_SELECTION;
    uiUIParameter=0;

    for(uiCounter=0;uiCounter<SETTINGS_SLA_AMOUNT;uiCounter++)
    {
        uiUISLAActive[uiCounter]=0x0;
    }

    for(uiCounter=1;uiCounter<SETTINGS_MODIFIER_AMOUNT;uiCounter++)
    {
        uiUIModeActive[uiCounter]=0x2;
    }

    vUIUpdate();
}

void vUIWakeUp(void)
{
    if(uiUIState==UI_STATE_STANDBY) //WAKE UP
    {
        uiUIState=uiUIStandbyPreviousState;
    }

    uiSystemStandbyCounterSeconds=uiSystemStandbyTimeSeconds;
    uiSystemResetCounterSeconds=uiSystemResetTimeSeconds;
}

uint8_t uiUIModeTypeGet(void)
{

    if(uiUIMode <= UI_MODE_MAX_VALUE)
    {
        return UI_STATE_TYPE_MODE;
    }
    else if(uiUIMode <= UI_MODIFIER_MAX_VALUE)
    {
        return UI_STATE_TYPE_MODIFIER;
    }
    else
    {
        return UI_STATE_TYPE_PRESET;
    }
}

/*
void vUIParameterSet(void)
{
    uint8_t uiCounterSLA;

    if(uiUIModeTypeGet() == UI_STATE_TYPE_MODE)
    {
        for(uiCounterSLA=0;uiCounterSLA<SETTINGS_SLA_AMOUNT;uiCounterSLA++)
        {
            if(uiUISLAActive[uiCounterSLA])
            {
                tsSettings[uiCounterSLA].uiModeParameter[0][uiUIParameter]=uiUIParameterValue;
            }
        }
    }
    else
    {
        for(uiCounterSLA=0;uiCounterSLA<SETTINGS_SLA_AMOUNT;uiCounterSLA++)
        {
            if(uiUISLAActive[uiCounterSLA])
            {
                tsSettings[uiCounterSLA].uiModeParameter[UI_MODIFIER_INDEX][uiUIParameter]=uiUIParameterValue;
            }
        }
    }
}
*/

void vUISLASelectionSwitch(void)
{
    uint8_t uiCounterModifier;

    //switch selection
    uiUISLAActive[uiUISLA]=!uiUISLAActive[uiUISLA];


    //Reset rest of UI
    for(uiCounterModifier=1;uiCounterModifier<UI_MODIFIER_AMOUNT+1;uiCounterModifier++)
    {
        uiUIModeActive[uiCounterModifier]=UI_MODIFIER_UNCHANGED;
    }

    uiUIModeActive[0]=UI_MODE_NO_SELECTION;
}

/*
 * Sets the Parameters of Mode uiMode to standard values for all active SLAs
 */
void vUIModeParameterStandardSet(void)
{
    uint8_t uiCounterSLA,uiCounterParameter;

    for(uiCounterSLA=0;uiCounterSLA<SETTINGS_SLA_AMOUNT;uiCounterSLA++)
    {
        if(uiUISLAActive[uiCounterSLA])
        {
            tsSettings[uiCounterSLA].uiModeActive[0]=uiUIModeActive[0];

            for(uiCounterParameter=0;uiCounterParameter<SETTINGS_PARAMETER_AMOUNT;uiCounterParameter++)
            {
                tsSettings[uiCounterSLA].uiModeParameter[0][uiCounterParameter]=uiSettingsParameterStandard[uiUIModeActive[0]][uiCounterParameter];
            }

            //test for actors
            tsSettings[uiCounterSLA].uiModeActors[0]=0;
            tsSettings[uiCounterSLA].uiModeActors[1]=0;
        }
    }
}

void vUIModifierParameterStandardSet(void)
{


}

void vUIModifierSelectionSwitch(void)
{
    if(uiUIModeActive[UI_MODIFIER_INDEX]==UI_MODIFIER_ACTIVATED)
    {
        //Unselect Modifier in UI
        uiUIModeActive[UI_MODIFIER_INDEX]=UI_MODIFIER_DEACTIVATED;
        //deactivate selected modifier in settings
        vUIModifierDeactivate();
    }
    else
    {
        //Select Modifier in UI
        uiUIModeActive[UI_MODIFIER_INDEX]=UI_MODIFIER_ACTIVATED;
        //Activate Modifier in settings and load standard parameters
        vUIModifierActivate();
    }
}

void vUIModifierActivate(void)
{
    uint8_t uiCounterSLA;
    uint8_t uiCounterParameter;

    for(uiCounterSLA=0;uiCounterSLA<SETTINGS_SLA_AMOUNT;uiCounterSLA++)
    {
        if(uiUISLAActive[uiCounterSLA])
        {
            tsSettings[uiCounterSLA].uiModeActive[UI_MODIFIER_INDEX]=0x1;

            for(uiCounterParameter=0;uiCounterParameter<SETTINGS_PARAMETER_AMOUNT;uiCounterParameter++)
            {
                tsSettings[uiCounterSLA].uiModeParameter[UI_MODIFIER_INDEX][uiCounterParameter]=uiSettingsParameterStandard[UI_MODIFIER_INDEX][uiCounterParameter];
            }
        }
    }
}

void vUIModifierDeactivate(void)
{
    uint8_t uiCounterSLA;

    for(uiCounterSLA=0;uiCounterSLA<SETTINGS_SLA_AMOUNT;uiCounterSLA++)
    {
        if(uiUISLAActive[uiCounterSLA])
        {
            tsSettings[uiCounterSLA].uiModeActive[UI_MODIFIER_INDEX]=0x0;
        }
    }
}


void vUIPresetLoad(void)
{
    uint8_t uiCounterSLA;

    for(uiCounterSLA=0;uiCounterSLA<SETTINGS_SLA_AMOUNT;uiCounterSLA++)
    {
            tsSettings[uiCounterSLA]=tsPresets[UI_PRESET_INDEX][uiCounterSLA];
    }

}

uint16_t uiUIParameterValueGet(void)
{
    uint8_t uiCounterSLA=0;

        if(uiUIModeTypeGet() == UI_STATE_TYPE_MODE)
        {
            while(uiCounterSLA<SETTINGS_SLA_AMOUNT)
            {
                if(uiUISLAActive[uiCounterSLA])
                {
                    return tsSettings[uiCounterSLA].uiModeParameter[0][uiUIParameter];
                }
                uiCounterSLA++;
            }
        }
        else if(uiUIModeTypeGet() == UI_STATE_TYPE_MODIFIER)
        {
            while(uiCounterSLA<SETTINGS_SLA_AMOUNT)
            {
                if(uiUISLAActive[uiCounterSLA])
                {
                    return tsSettings[uiCounterSLA].uiModeParameter[UI_MODIFIER_INDEX][uiUIParameter];
                }
                uiCounterSLA++;
            }
        }

    return 0;
}

void vUIParameterValueSet(void)
{
    uint8_t uiCounterSLA=0;

        if(uiUIModeTypeGet() == UI_STATE_TYPE_MODE)
        {
            for(uiCounterSLA=0;uiCounterSLA<SETTINGS_SLA_AMOUNT;uiCounterSLA++)
            {
                if(uiUISLAActive[uiCounterSLA])
                {
                    tsSettings[uiCounterSLA].uiModeParameter[0][uiUIParameter]=uiUIParameterValue;
                }
            }
        }
        else if(uiUIModeTypeGet() == UI_STATE_TYPE_MODIFIER)
        {
            for(uiCounterSLA=0;uiCounterSLA<SETTINGS_SLA_AMOUNT;uiCounterSLA++)
            {
                if(uiUISLAActive[uiCounterSLA])
                {
                    tsSettings[uiCounterSLA].uiModeParameter[UI_MODIFIER_INDEX][uiUIParameter]=uiUIParameterValue;
                }
            }
        }


}

uint16_t uiUISettingValueGet(void)
{
    if(uiUISetting < 8)
    {
        return tsSettings[uiUISetting].uiSLALength;
    }
    else if(uiUISetting == 8)
    {
        return tsSystemTime.uiHour;
    }
    else if(uiUISetting == 9)
    {
        return tsSystemTime.uiMinute;
    }
    else if(uiUISetting == 10)
    {
        return tsSystemTime.uiSecond;

    }
    else if(uiUISetting == 11)
    {
        return tsSystemTime.ui100MilliSecond;
    }

    return 0;
}

void vUISettingValueSet(void)
{
    if(uiUISetting < 8)
    {
        tsSettings[uiUISetting].uiSLALength=uiUISettingValue;
    }
    else if(uiUISetting == 8)
    {
        tsSystemTime.uiHour=uiUISettingValue;
    }
    else if(uiUISetting == 9)
    {
        tsSystemTime.uiMinute=uiUISettingValue;
    }
    else if(uiUISetting == 10)
    {
        tsSystemTime.uiSecond=uiUISettingValue;

    }
    else if(uiUISetting == 11)
    {
        tsSystemTime.ui100MilliSecond=uiUISettingValue;
    }
}
