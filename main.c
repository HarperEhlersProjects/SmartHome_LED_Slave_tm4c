#include <stdint.h>
#include <stdbool.h>

#include "interrupt.h"
#include "timer.h"

#include "system_set_up.h"
#include "delay.h"
#include "led_controller_interface.h"
#include "data_receiver.h"
#include "settings.h"

uint8_t uiCounter=0;

void main(void)
{

    IntMasterDisable();
    vSystemSetUp();
    vSettingsInit();
    vDelayInit();
    vDataReceiverInit();
    vLEDControllerInterfaceInit();

    IntMasterEnable();

    while(1)
    {
        vDataReceiverReceive();
        while(uiLEDCITransmissionRun==1);
        vDataReceiverTransmissionInitiate();
    }
}

