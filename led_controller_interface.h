#ifndef LED_CONTROLLER_INTERFACE_H_
#define LED_CONTROLLER_INTERFACE_H_

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <hw_memmap.h>
#include "settings.h"

/*
 * Use GPIO Port K as led controller interface output port.
 * change makros for using an other port.
 */
#define LED_CONTROLLER_INTERFACE_SYSCTL_OUTPUT_PORT SYSCTL_PERIPH_GPIOK
#define LED_CONTROLLER_INTERFACE_OUTPUT_PORT GPIO_PORTK_BASE
#define LED_CONTROLLER_INTERFACE_OUTPUT_DATA GPIO_PORTK_DATA_R

/*
 * Use all pins for the output port.
 * change here for the use of other pins.
 */
#define LED_CONTROLLER_INTERFACE_OUTPUT_PINS 0x3

/*
 * Use Timer 1 for timing the Bits of the NZR Protocol
 */
#define LED_CONTROLLER_INTERFACE_SYSCTL_TIMER_BIT SYSCTL_PERIPH_TIMER1
#define LED_CONTROLLER_INTERFACE_TIMER_BIT TIMER1_BASE

/*
 * Use Timer 2 for timing High and Low of the NZR Protocol
 */
#define LED_CONTROLLER_INTERFACE_SYSCTL_TIMER_HL SYSCTL_PERIPH_TIMER2
#define LED_CONTROLLER_INTERFACE_TIMER_HL TIMER2_BASE

/*
 * Use TimerA for value 1 high to low transition and TimerB for value 0 high to low transition
 */
#define LED_CONTROLLER_INTERFACE_TIMER_HL_0 TIMER_A
#define LED_CONTROLLER_INTERFACE_TIMER_HL_1 TIMER_B

/*
 * Time interval for Timers
 */
#define TIMER_INTERVAL_1us25 (((SYS_CLK/1000000)*125)/100)
#define TIMER_INTERVAL_0us9 (((SYS_CLK/1000000)*9)/10)
#define TIMER_INTERVAL_0us35 (((SYS_CLK/1000000)*35)/100)

#define TIMER_INTERVAL_50us (SYS_CLK/100000)*5
#define TIMER_INTERVAL_2us5 (SYS_CLK/10000000)*25
#define TIMER_INTERVAL_0us5 (SYS_CLK/10000000)*5
#define TIMER_INTERVAL_0us1 (SYS_CLK/10000000)
#define TIMER_INTERVAL_1us2 (SYS_CLK/10000000)*12

#define TIMER_INTERVAL_HIGHLOW_OFFSET (-TIMER_INTERVAL_0us1)
#define TIMER_INTERVAL_PULLDOWN_OFFSET TIMER_INTERVAL_0us5

#define TIMER_INTERVAL_BIT_INTERRUPT (TIMER_INTERVAL_1us25+TIMER_INTERVAL_0us5)

#define BIT_SEQUENCE_LENGTH 24

/*
 * Data transmission statemachine makros
 */
#define LED_CONTROLLER_INTERFACE_STATE_TRANSMISSION 0x1
#define LED_CONTROLLER_INTERFACE_STATE_RESET 0x2

uint8_t uiLEDCIDoubleBuffer[2][SETTINGS_SLA_LENGTH_MAX*BIT_SEQUENCE_LENGTH];

uint8_t TransmitterBufferIndex;
uint8_t* TransmitterBufferPtr;

uint8_t uiLEDCITransmissionRun;


void vLEDControllerInterfaceInit(void);

void vLEDControllerInterfaceOutputSet(uint8_t uiOutputBitfield);

void vLEDControllerInterfaceOutputMaskSet(void);

void highHandler(void);
void dataHandler(void);
void lowHandler(void);

void uDMAPingPongHandler(void);

#endif /* LED_CONTROLLER_INTERFACE_H_ */
