#include "i2c_interface.h"

#include "system_set_up.h"
#include "pin_map.h"
#include "delay.h"

uint8_t uiPortExpanderAddress[6][2]={{PORTEXPANDER_ADRESS_BASE1,PORTEXPANDER_ADRESS_OUTPUTA},{PORTEXPANDER_ADRESS_BASE1,PORTEXPANDER_ADRESS_OUTPUTB},
                                    {PORTEXPANDER_ADRESS_BASE2,PORTEXPANDER_ADRESS_OUTPUTA},{PORTEXPANDER_ADRESS_BASE2,PORTEXPANDER_ADRESS_OUTPUTB},
                                    {PORTEXPANDER_ADRESS_BASE3,PORTEXPANDER_ADRESS_OUTPUTA},{PORTEXPANDER_ADRESS_BASE3,PORTEXPANDER_ADRESS_OUTPUTB}};

void vI2CInterfaceInit()
{
    //Enable clock to the QEI input GPIO Port
    SysCtlPeripheralEnable(I2C_INTERFACE_SYSCTL_MODULE);
    //wait 3 clocksycles for SysCtlPeripheralEnable
    asm("\tnop;\r\n\tnop;\r\n\tnop;\r\n");

    //Enable clock to the QEI input GPIO Port
    SysCtlPeripheralEnable(I2C_INTERFACE_SYSCTL_PIN_PORT);
    //wait 3 clocksycles for SysCtlPeripheralEnable
    asm("\tnop;\r\n\tnop;\r\n\tnop;\r\n");

    //Set pins for as alternative function
    GPIOPinConfigure(GPIO_PB2_I2C0SCL);
    GPIOPinConfigure(GPIO_PB3_I2C0SDA);

    //Configure pins for alternative functionality
    GPIOPinTypeI2CSCL(I2C_INTERFACE_PIN_PORT,I2C_INTERFACE_SCL_PIN);
    GPIOPinTypeI2C(I2C_INTERFACE_PIN_PORT,I2C_INTERFACE_SDA_PIN);


    I2CMasterInitExpClk(I2C_INTERFACE_MODULE,SYS_CLK,false);

    //Enable I2C module
    I2CMasterEnable(I2C_INTERFACE_MODULE);


    //Initial transmission for configuring the i2c-portexpander

    //Initialize Ports as outputs

    vDelayMicroSec(1000);
    vI2CInterfaceWrite(0,PORTEXPANDER_ADRESS_IODIRA,PORTEXPANDER_DATA_DIR_OUTPUT);
    vDelayMicroSec(1000);
    vI2CInterfaceWrite(1,PORTEXPANDER_ADRESS_IODIRB,PORTEXPANDER_DATA_DIR_OUTPUT);
    vDelayMicroSec(1000);
    vI2CInterfaceWrite(2,PORTEXPANDER_ADRESS_IODIRA,PORTEXPANDER_DATA_DIR_OUTPUT);
    vDelayMicroSec(1000);
    vI2CInterfaceWrite(3,PORTEXPANDER_ADRESS_IODIRB,PORTEXPANDER_DATA_DIR_OUTPUT);
    vDelayMicroSec(1000);
    vI2CInterfaceWrite(4,PORTEXPANDER_ADRESS_IODIRA,PORTEXPANDER_DATA_DIR_OUTPUT);
    vDelayMicroSec(1000);
    vI2CInterfaceWrite(5,PORTEXPANDER_ADRESS_IODIRB,PORTEXPANDER_DATA_DIR_OUTPUT);

    vDelayMicroSec(1000);


}

void vI2CInterfaceOutputWrite(uint8_t uiPortNumber,uint8_t uiData)
{

    I2CMasterSlaveAddrSet(I2C_INTERFACE_MODULE,uiPortExpanderAddress[uiPortNumber][0],false);

    I2CMasterDataPut(I2C_INTERFACE_MODULE,uiPortExpanderAddress[uiPortNumber][1]);

    I2CMasterControl(I2C_INTERFACE_MODULE,I2C_MASTER_CMD_BURST_SEND_START);

    vDelayMicroSec(200);

    I2CMasterDataPut(I2C_INTERFACE_MODULE,uiData);

    I2CMasterControl(I2C_INTERFACE_MODULE,I2C_MASTER_CMD_BURST_SEND_FINISH);

    vDelayMicroSec(200);
}

void vI2CInterfaceWrite(uint8_t uiPortNumber,uint8_t uiRegisterAddress,uint8_t uiData)
{
    I2CMasterSlaveAddrSet(I2C_INTERFACE_MODULE,uiPortExpanderAddress[uiPortNumber][0],false);

    I2CMasterDataPut(I2C_INTERFACE_MODULE,uiRegisterAddress);

    I2CMasterControl(I2C_INTERFACE_MODULE,I2C_MASTER_CMD_BURST_SEND_START);

    vDelayMicroSec(200);

    I2CMasterDataPut(I2C_INTERFACE_MODULE,uiData);

    I2CMasterControl(I2C_INTERFACE_MODULE,I2C_MASTER_CMD_BURST_SEND_FINISH);

    vDelayMicroSec(200);
}

/*
void I2CInterfaceRead(uint8_t address,uint8_t data)
{
    I2CMasterSlaveAddrSet(I2C_INTERFACE_MODULE,address,true);

}
*/
