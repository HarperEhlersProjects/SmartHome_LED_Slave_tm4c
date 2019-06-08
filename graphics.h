#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include "stdint.h"
#include "led_controller_interface.h"
#include "system_set_up.h"

/*
 * Graphics module use Timer 2.
 * change here for the use of an other timer.
 */
#define GRAPHICS_SYSCTL_TIMER SYSCTL_PERIPH_TIMER2
#define GRAPHICS_TIMER TIMER2_BASE

//define framerate in Hz
#define GRAPHICS_FRAMRATE_HZ 30

//define fram period for previous defined framerate
#define GRAPHICS_FRAME_PERIOD (SYS_CLK/GRAPHICS_FRAMRATE_HZ)

typedef struct{
uint8_t uiRed;
uint8_t uiGreen;
uint8_t uiBlue;
}tsColorStruct;

void vGraphicsInit(void);

void vGraphicsHandler(void);

void vGraphicsModeCalc();
void vGraphicsTransmissionInitiate();
void vGraphicsLedSet(uint8_t,uint16_t,tsColorStruct);

void vGraphicsMode0(uint8_t);
void vGraphicsMode1(uint8_t);
void vGraphicsMode2(uint8_t);
#endif /* GRAPHICS_H_ */
