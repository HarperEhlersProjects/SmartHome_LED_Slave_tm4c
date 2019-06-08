#ifndef BUTTONS_H_
#define BUTTONS_H_

#include <stdint.h>
#include <stdbool.h>

#include <hw_memmap.h>
#include "sysctl.h"


#include "settings.h"
/*
 * Use Port E for buttons
 */
#define BUTTONS_SYSCTL_GPIO_PORT SYSCTL_PERIPH_GPIOE
#define BUTTONS_GPIO_PORT GPIO_PORTE_BASE

/*
 * define Pin 0-2 as pins for buttons
 */
#define BUTTONS_PINS 0x7

/*
 * buttons interrupt type
 */
#define BUTTONS_INTERRUPT_TYPE GPIO_RISING_EDGE

/*
 * button values
 */
#define BUTTONS_PUSHED_INCREMENTER 0x1
#define BUTTONS_PUSHED_RIGHT 0x2
#define BUTTONS_PUSHED_LEFT 0x4
#define BUTTONS_PUSHED_LEFTANDRIGHT 0x6

void vButtonsInit(void);
void vButtonsHandler(void);

#endif /* BUTTONS_H_ */
