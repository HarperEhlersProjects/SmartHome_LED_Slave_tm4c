#include "buttons.h"

#include "gpio.h"
#include "interrupt.h"

#include "hw_ints.h"

#include "ui_statemachine.h"
#include "quadrature_interface.h"
#include "delay.h"

uint8_t uiButtonsPushed;
uint8_t uiButtonsCounter;
uint8_t uiButtonsCounter1;

uint8_t uiButtonsDataFound;

uint8_t uiButtonsSLAAllActive;

void vButtonsInit()
{
    //Enable clock to the QEI input GPIO Port
    SysCtlPeripheralEnable(BUTTONS_SYSCTL_GPIO_PORT);
    //wait 3 clocksycles for SysCtlPeripheralEnable
    asm("\tnop;\r\n\tnop;\r\n\tnop;\r\n");

    //configure as standard pins
    GPIOPadConfigSet(BUTTONS_GPIO_PORT,BUTTONS_PINS,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD);

    //set as input
    GPIOPinTypeGPIOInput(BUTTONS_GPIO_PORT,BUTTONS_PINS);

    //Set interrupt on falling edge
    GPIOIntTypeSet(BUTTONS_GPIO_PORT,BUTTONS_PINS,GPIO_RISING_EDGE);

    //Set interrupt Priority to 2
    IntPrioritySet(INT_GPIOE,0x40);

    //register interrupthandler
    GPIOIntRegister(BUTTONS_GPIO_PORT,vButtonsHandler);

    //enable interrupt
    GPIOIntEnable(BUTTONS_GPIO_PORT,BUTTONS_PINS);

}

void vButtonsHandler(void)
{

vDelayMiliSec(30);

uiButtonsPushed=GPIOPinRead(BUTTONS_GPIO_PORT,BUTTONS_PINS);


switch(uiButtonsPushed)
{
case BUTTONS_PUSHED_INCREMENTER://INCREMENTER IS PUSHED
    switch(uiUIState)
    {
        case UI_STATE_SLA_SELECTION: //constant state

                                     vUISLASelectionSwitch();

        break;
        case UI_STATE_MODE_SELECTION: //constant state
                                      if(uiUIModeTypeGet() == UI_STATE_TYPE_MODE)//if Mode selected
                                      {
                                          //Set activated mode as the selected Mode
                                          uiUIModeActive[0]=uiUIMode;
                                          //Load standard Parameters for selected SLAs and activated mode
                                          vUIModeParameterStandardSet();
                                      }
                                      else if(uiUIModeTypeGet() == UI_STATE_TYPE_MODIFIER)//if Modifier selected
                                      {
                                          //Switch state of selected Modifier and load standard values into settings if activated
                                          vUIModifierSelectionSwitch();
                                      }
                                      else if(uiUIModeTypeGet() == UI_STATE_TYPE_PRESET)//Preset selected
                                      {
                                          //load preset
                                          vUIPresetLoad();
                                      }

        break;
        case UI_STATE_PARAMETER_SELECTION: uiUIState=UI_STATE_PARAMETER_SELECTED;
                                           //Set QEI position for new state
                                           vQEIPositionSet(uiUIParameterValue);

        break;
        case UI_STATE_PARAMETER_SELECTED: uiUIState=UI_STATE_PARAMETER_SELECTION;
                                          //Set Parameter depending on which mode is selected
                                          vUIParameterValueSet();
                                          //Set QEIposition for new state
                                          vQEIPositionSet(uiUIParameter);
        break;
        case UI_STATE_SETTINGS_SELECTION: uiUIState=UI_STATE_SETTINGS_SELECTED;
                                          //Set QEI position for new state
                                          vQEIPositionSet(uiUISettingValue);
        break;
        case UI_STATE_SETTINGS_SELECTED:  uiUIState=UI_STATE_SETTINGS_SELECTION;
                                          //Set Parameter depending on which systemsetting is selected
                                          vUISettingValueSet();
                                          //Set QEIposition for new state
                                          vQEIPositionSet(uiUISetting);
        break;
    }
break;
case BUTTONS_PUSHED_RIGHT://RIGHT BUTTON PUSHED
    switch(uiUIState)
    {
        case UI_STATE_SLA_SELECTION: uiUIState=UI_STATE_MODE_SELECTION;
                                     vQEIPositionSet(uiUIMode);
        break;
        case UI_STATE_MODE_SELECTION:
                                      if(uiUIModeActive[0] != UI_MODE_NO_SELECTION || ((uiUIModeTypeGet() == UI_STATE_TYPE_MODIFIER) && uiUIModeActive[UI_MODIFIER_INDEX]==1))
                                      {
                                          uiUIState=UI_STATE_PARAMETER_SELECTION;
                                          vQEIPositionSet(uiUIParameter);
                                      }
        break;
        case UI_STATE_PARAMETER_SELECTION: uiUIState=UI_STATE_SLA_SELECTION;
                                           vQEIPositionSet(uiUISLA);
        break;
    }
break;
case BUTTONS_PUSHED_LEFT://LEFT BUTTON PUSHED
    switch(uiUIState)
    {
        case UI_STATE_SLA_SELECTION:
                                      if(uiUIModeActive[0] != UI_MODE_NO_SELECTION || ((uiUIModeTypeGet() == UI_STATE_TYPE_MODIFIER) && uiUIModeActive[UI_MODIFIER_INDEX]==1))
                                      {
                                          uiUIState=UI_STATE_PARAMETER_SELECTION;
                                          vQEIPositionSet(uiUIParameter);
                                      }
        break;
        case UI_STATE_MODE_SELECTION:
                                          uiUIState=UI_STATE_SLA_SELECTION;
                                          vQEIPositionSet(uiUISLA);
        break;
        case UI_STATE_PARAMETER_SELECTION:
                                            uiUIState=UI_STATE_MODE_SELECTION;
                                            vQEIPositionSet(uiUIMode);
        break;
    }
break;
case BUTTONS_PUSHED_LEFTANDRIGHT:
    switch(uiUIState)
    {
        case UI_STATE_SLA_SELECTION:
                                    uiUIState = UI_STATE_SETTINGS_SELECTION;
                                    uiUISetting=0;
                                    vQEIPositionSet(uiUISetting);
        break;
        case UI_STATE_SETTINGS_SELECTION:
                                    uiUIState=UI_STATE_SLA_SELECTION;
                                    vQEIPositionSet(uiUISLA);
        break;
    }
break;
}

GPIOIntClear(BUTTONS_GPIO_PORT,BUTTONS_PINS);

vUIWakeUp();

vUIUpdate();    //update UI with new state

//vDelayMiliSec(100);

GPIOIntClear(BUTTONS_GPIO_PORT,BUTTONS_PINS);

}
