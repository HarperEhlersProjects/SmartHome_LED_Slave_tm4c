#include <stdint.h>
#include <stdbool.h>

#include "interrupt.h"
#include "timer.h"

#include "data_receiver.h"
#include "system_set_up.h"
#include "delay.h"
#include "led_controller_interface.h"
#include "settings.h"

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
        while(uiLEDCITransmissionRun);
        vDataReceiverTransmissionInitiate();
    }
}

