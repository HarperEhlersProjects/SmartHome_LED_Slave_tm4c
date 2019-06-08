#ifndef DELAY_H_
#define DELAY_H_

#include <stdint.h>
#include <stdbool.h>

#include <hw_memmap.h>
#include "timer.h"
#include "sysctl.h"

//Should not be used for SYS_CLOCK below 20 MHz

/*
 * Delay module use Timer 0.
 * change here for the use of an other timer.
 */
#define DELAY_SYSCTL_TIMER SYSCTL_PERIPH_TIMER0
#define DELAY_TIMER TIMER0_BASE

void vDelayInit(void);

void vDelaySec(uint32_t);
void vDelayMiliSec(uint32_t);
void vDelayMicroSec(uint32_t);


#endif /* DELAY_H_ */
