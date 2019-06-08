#ifndef I2C_INTERFACE_H_
#define I2C_INTERFACE_H_

#include "stdint.h"
#include "stdbool.h"

#include "hw_memmap.h"
#include "i2c.h"
#include "sysctl.h"
#include "gpio.h"

/*
 * Use I2C0 module
 */
#define I2C_INTERFACE_MODULE I2C0_BASE
#define I2C_INTERFACE_SYSCTL_MODULE SYSCTL_PERIPH_I2C0

/*
 * Use GPIO Port B as i2c port.
 * change makros for using an other port.
 */
#define I2C_INTERFACE_SYSCTL_PIN_PORT SYSCTL_PERIPH_GPIOB
#define I2C_INTERFACE_PIN_PORT GPIO_PORTB_BASE

/*
 * Use Pin 2 for SCL
 */
#define I2C_INTERFACE_SCL_PIN 0x4

/*
 * Use Pin 3 for SDA
 */
#define I2C_INTERFACE_SDA_PIN 0x8

/*
 * PORTEXPANDER MAKROS
 */
#define PORTEXPANDER_ADRESS_BASE1 0x20      //0x20 is general base address of port expander
#define PORTEXPANDER_ADRESS_BASE2 0x21
#define PORTEXPANDER_ADRESS_BASE3 0x22

#define PORTEXPANDER_ADRESS_OUTPUTA 0x14
#define PORTEXPANDER_ADRESS_OUTPUTB 0x15

#define PORTEXPANDER_ADRESS_IODIRA 0x00
#define PORTEXPANDER_ADRESS_IODIRB 0x01

#define PORTEXPANDER_ADRESS_IPOLA 0x02
#define PORTEXPANDER_ADRESS_IPOLB 0x03


#define PORTEXPANDER_DATA_DIR_OUTPUT 0x00
/*
 * PORTEXPANDER MAKROS
 */

void vI2CInterfaceInit(void);
void vI2CInterfaceOutputWrite(uint8_t,uint8_t);
void vI2CInterfaceWrite(uint8_t,uint8_t,uint8_t);

#endif /* I2C_INTERFACE_H_ */
