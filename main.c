#include <stdint.h>
#include <stdbool.h>

#include "interrupt.h"
#include "timer.h"

#include "system_set_up.h"
#include "delay.h"
#include "led_controller_interface.h"
#include "quadrature_interface.h"
#include "i2c_interface.h"
#include "buttons.h"
#include "graphics.h"
#include "data_receiver.h"

#include "ui_statemachine.h"
#include "settings.h"

uint8_t uiCounter=0;

void main(void)
{

    IntMasterDisable();
    vSystemSetUp();
    vSettingsInit();
    vDelayInit();
    vDataReceiverInit();
    //vGraphicsInit();
    vLEDControllerInterfaceInit();
    //vQEIInit();
    //vI2CInterfaceInit();
    //vButtonsInit();
    //vUIInit();
    IntMasterEnable();

    while(1)
    {
        //vDelaySec(1);
        //vGraphicsModeCalc();
        //vGraphicsTransmissionInitiate();

        //vDelayMiliSec(10);

        vDataReceiverReceive();
        while(uiLEDCITransmissionRun==1);
        vDataReceiverTransmissionInitiate();
    }
}

