#ifndef QUADRATURE_INTERFACE_H_
#define QUADRATURE_INTERFACE_H_

#include "system_set_up.h"

#include <hw_memmap.h>

/*
 * Use GPIO Port L as QEI input port.
 * change makros for using an other port.
 */
#define QEI_SYSCTL_INPUT_PORT SYSCTL_PERIPH_GPIOL
#define QEI_INPUT_PORT GPIO_PORTL_BASE

/*
 * Use Pin 1 and Pin 2 for PhA and PhB
 */
#define QEI_PINS 0x6

/*
 * Use QEI module 0
 */
#define QEI_SYSCTL_MODULE SYSCTL_PERIPH_QEI0
#define QEI_MODULE QEI0_BASE

/*
 * configure QEI as clock on both edges, without reset index signal, clock and direction and no pin swap
 */
#define QEI_CONFIGURATION (QEI_CONFIG_CAPTURE_A | QEI_CONFIG_NO_RESET | QEI_CONFIG_QUADRATURE | QEI_CONFIG_NO_SWAP)

/*
 * QEI Counter offset for boundarys
 */
#define QEI_COUNTER_OFFSET 24

/*
 * QEI counter maximum position
 */
#define QEI_COUNTER_MAX (1000+QEI_COUNTER_OFFSET)

/*
 * QEI counter initial position
 */
#define QEI_COUNTER_INIT (QEI_COUNTER_OFFSET)

/*
 * QEI measure time period in micro seconds
 */
#define QEI_MEASUREMENT_TIME 10000000

void vQEIInit(void);
void QEIIncrementHandler(void);
void vQEIPositionSet(uint16_t);
uint16_t uiQEIPositionGet(uint16_t,uint16_t);
void vQEIBoundaryModifierSet();

#endif /* QUADRATURE_INTERFACE_H_ */
