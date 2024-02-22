#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/I2C.h>

#include "Board.h"

// In this example GY-521 is connected to EK-TM4C1294-XL launchpad
//
// PD0   (launchpad) --> SCL (GY-521)
// PD1   (launchpad) --> SDA (GY-521)
// +3.3V (launchpad) --> GND (GY-521)
// GND   (launchpad) --> VCC (GY-521)
//

#define TASKSTACKSIZE       1024

Task_Struct tsk_struct;
Char tsk_stack[TASKSTACKSIZE];

#define DEVICE_ID               0x68

I2C_Handle      i2c;
I2C_Params      i2cParams;
I2C_Transaction i2cTransaction;

bool IIC_OpenComm(void)
{
    bool retval = false;

    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_400kHz;                 // minimum speed first
    i2c = I2C_open(Board_I2C0, &i2cParams);
    if (i2c == NULL) {
        System_abort("Error Initializing I2C\n");   // error, retval is false
    }
    else {
        System_printf("I2C Initialized!\n");        // everything alright, retval is true
        retval = true;
    }
    System_flush();
    return retval;                                  // return true or false
}

//
// IIC_writeReg()
//
// This function is used to write an unsigned 8 bit to a register on the the slave device
//
// device_ID:  IIC device ID of the slave
// addr:       register address
// val:        register value to be written

bool IIC_writeReg(int device_ID, int addr, uint8_t val)
{
    uint8_t txBuffer[2];
    uint8_t rxBuffer[2];
    bool retval=false;

    // place parameters
    txBuffer[0] = addr;                             // register address
    txBuffer[1] = val;                              // value to be written
    i2cTransaction.slaveAddress = device_ID;        // device IIC ID
    i2cTransaction.writeBuf = txBuffer;             // buffer that holds the values to be written
    i2cTransaction.writeCount = 2;                  // 2 bytes will be sent
    i2cTransaction.readBuf = rxBuffer;              // receive buffer (in this case it is not used)
    i2cTransaction.readCount = 0;                   // no bytes to be received

    if (I2C_transfer(i2c, &i2cTransaction)) {       // start the transaction
        System_printf("IIC %d %d\n", addr, val);    // if the slave responds, write the received value
        retval = true;                              // true will be returned to indicate that transaction is successful
    }
    else {
        System_printf("I2C Bus fault\n");           // there is an error, returns false
    }
    System_flush();

    return retval;
}

bool IIC_readReg(int device_ID, int addr, int no_of_bytes, char *buf)
{
    uint8_t txBuffer[2];
    bool retval=false;

    // addr: register number
    txBuffer[0] = addr;                             // 1 byte: register address
    i2cTransaction.slaveAddress = device_ID;        // Device Id
    i2cTransaction.writeBuf = txBuffer;             // buffer to be sent
    i2cTransaction.writeCount = 1;                  // send just register address
    i2cTransaction.readBuf = buf;                   // read into this buffer
    i2cTransaction.readCount = no_of_bytes;         // number of bytes to be read


    if (I2C_transfer(i2c, &i2cTransaction)) {
        System_printf("IIC_readReg(%d,%d)\n", addr, buf[0]);
        retval=true;
    }
    else {
        System_printf("I2C Bus fault\n");
    }
    System_flush();

    return retval;
}

void IIC_CloseComm(void)
{
    I2C_close(i2c);
    System_printf("IIC_CloseComm()   I2C closed!\n");
    System_flush();
}

/*
 *  ======== taskFxn ========
 *  Task for this function is created statically. See the project's .cfg file.
 */
Void taskFxn(UArg arg0, UArg arg1)
{
    char buf[10];

    IIC_OpenComm();

    while(1) {
        IIC_readReg(0x68, 0x75, 1,buf);
        System_printf("Register 0x75 (WHO_AM_I) = 0x%x\n", buf[0]);
        System_flush();
        Task_sleep(5000);
    }

    IIC_CloseComm();
}

/*
 *  ======== main ========
 */
int main(void)
{
    Task_Params taskParams;

    /* Call board init functions */
    Board_initGeneral();
    Board_initGPIO();
    Board_initI2C();

    /* Construct tmp006 Task thread */
    Task_Params_init(&taskParams);
    taskParams.stackSize = TASKSTACKSIZE;
    taskParams.stack = &tsk_stack;
    Task_construct(&tsk_struct, (Task_FuncPtr)taskFxn, &taskParams, NULL);

    /* Turn on user LED */
    GPIO_write(Board_LED0, Board_LED_ON);

    System_printf("Starting the I2C example\nSystem provider is set to SysMin."
                  " Halt the target to view any SysMin contents in ROV.\n");
    /* SysMin will only print to the console when you call flush or exit */
    System_flush();

    /* Start BIOS */
    BIOS_start();

    return (0);
}
