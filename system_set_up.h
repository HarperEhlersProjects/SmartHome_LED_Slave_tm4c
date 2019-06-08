#ifndef SYSTEM_SET_UP_H_
#define SYSTEM_SET_UP_H_

#include <stdint.h>
#include <stdbool.h>

#include "sysctl.h"
#include "systick.h"

#define SYS_CLK 120000000

#define SYSTEM_STANDBY_TIME_INIT 30
#define SYSTEM_RESET_TIME_INIT 600

typedef struct{
    uint8_t ui100MilliSecond;
    uint8_t uiSecond;
    uint8_t uiMinute;
    uint8_t uiHour;

    uint8_t uiDay;
    uint8_t uiMonth;
    uint16_t uiYear;
}tsSystemTimeStruct;

tsSystemTimeStruct tsSystemTime;

uint16_t uiSystemStandbyCounterSeconds;
uint16_t uiSystemResetCounterSeconds;

uint16_t uiSystemStandbyTimeSeconds;
uint16_t uiSystemResetTimeSeconds;

uint32_t clock;

void vSystemSetUp(void);
void vSystemSysTickHandler(void);

#endif /* SYSTEM_SET_UP_H_ */
