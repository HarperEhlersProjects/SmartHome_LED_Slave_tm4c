#ifndef DATA_RECEIVER_H_
#define DATA_RECEIVER_H_

#include "stdint.h"
#include "stdbool.h"

#include "sysctl.h"
#include "gpio.h"

#define DATARECEIVER_USBSEQUENCE_PACKAGENUMBER 9
#define DATARECEIVER_BAUDRATE 2000000

#define DATARECEIVER_UART_PINS (GPIO_PIN_4 | GPIO_PIN_5)
#define DATARECEIVER_UART_PORT GPIO_PORTC_BASE
#define DATARECEIVER_UART_SYSCTL_PORT SYSCTL_PERIPH_GPIOC

void vDataReceiverInit(void);

void vDataReceiverReceive(void);
void vDataReceiverTransmissionInitiate(void);

#endif
