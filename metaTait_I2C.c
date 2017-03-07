#include "metaTait_I2C.h"
#include "F28x_Project.h"
#include "metaTait_PWM.h"
#include "metaTait_ISR.h"



#define I2C_SLAVE_ADDR        0x50
#define I2C_EEPROM_HIGH_ADDR  0x00
#define I2C_EEPROM_LOW_ADDR   0x30

int passs = 0;
int fails = 0;
//
// Globals
//
struct I2CMSG I2cMsgOut1={ I2C_MSGSTAT_SEND_WITHSTOP,
                           I2C_SLAVE_ADDR,
                           I2C_NUMBYTES,
                           I2C_EEPROM_HIGH_ADDR,
                           I2C_EEPROM_LOW_ADDR,
                           0x12,                   // Msg Byte 1
                           0x34
                           };                  // Msg Byte 2

struct I2CMSG I2cMsgIn1={ I2C_MSGSTAT_SEND_NOSTOP,
                          I2C_SLAVE_ADDR,
                          I2C_NUMBYTES,
                          I2C_EEPROM_HIGH_ADDR,
                          I2C_EEPROM_LOW_ADDR,
                            0x00,
                            0x00    };
struct I2CMSG I2cMsgOut2={ I2C_MSGSTAT_SEND_WITHSTOP,
                           I2C_SLAVE_ADDR,
                           I2C_NUMBYTES,
                           I2C_EEPROM_HIGH_ADDR,
                           I2C_EEPROM_LOW_ADDR,
                           0x12,                   // Msg Byte 1
                           0x34
                           };                  // Msg Byte 2

struct I2CMSG I2cMsgIn2={ I2C_MSGSTAT_SEND_NOSTOP,
                          I2C_SLAVE_ADDR,
                          I2C_NUMBYTES,
                          I2C_EEPROM_HIGH_ADDR,
                          I2C_EEPROM_LOW_ADDR,
                            0x00,
                            0x00    };
struct I2CMSG I2cMsgOut3={ I2C_MSGSTAT_SEND_WITHSTOP,
                           I2C_SLAVE_ADDR,
                           I2C_NUMBYTES,
                           I2C_EEPROM_HIGH_ADDR,
                           I2C_EEPROM_LOW_ADDR,
                           0x12,                   // Msg Byte 1
                           0x34
                           };                  // Msg Byte 2

struct I2CMSG I2cMsgIn3={ I2C_MSGSTAT_SEND_NOSTOP,
                          I2C_SLAVE_ADDR,
                          I2C_NUMBYTES,
                          I2C_EEPROM_HIGH_ADDR,
                          I2C_EEPROM_LOW_ADDR,
                            0x00,
                            0x00    };



//
//
// I2CA_WriteData - Transmit I2CA message
//
Uint16 I2C_Write(struct I2CMSG *msg)
{
   Uint16 i;

   //
   // Wait until the STP bit is cleared from any previous master communication.
   // Clearing of this bit by the module is delayed until after the SCD bit is
   // set. If this bit is not checked prior to initiating a new message, the
   // I2C could get confused.
   //
   if(I2caRegs.I2CMDR.bit.STP == 1)
   {
      return I2C_STP_NOT_READY_ERROR;
   }

   //
   // Setup slave address
   //
   I2caRegs.I2CSAR.all = msg->SlaveAddress;

   //
   // Check if bus busy
   //
   if(I2caRegs.I2CSTR.bit.BB == 1)
   {
      return I2C_BUS_BUSY_ERROR;
   }

   //
   // Setup number of bytes to send
   // MsgBuffer + Address
   //
   I2caRegs.I2CCNT = msg->NumOfBytes+2;

   //
   // Setup data to send
   //
   I2caRegs.I2CDXR.all = msg->MemoryHighAddr;
   I2caRegs.I2CDXR.all = msg->MemoryLowAddr;

   for (i=0; i < msg->NumOfBytes; i++)
   {
      I2caRegs.I2CDXR.all = *(msg->MsgBuffer+i);
   }

   //
   // Send start as master transmitter
   //
   I2caRegs.I2CMDR.all = 0x6E60;

   return I2C_SUCCESS;
}

//
// I2CA_ReadData - Read I2CA Message
//
Uint16 I2C_Read(struct I2CMSG *msg)
{
   //
   // Wait until the STP bit is cleared from any previous master communication.
   // Clearing of this bit by the module is delayed until after the SCD bit is
   // set. If this bit is not checked prior to initiating a new message, the
   // I2C could get confused.
   //
   if(I2caRegs.I2CMDR.bit.STP == 1)
   {
      return I2C_STP_NOT_READY_ERROR;
   }

   I2caRegs.I2CSAR.all = msg->SlaveAddress;

   if(msg->MsgStatus == I2C_MSGSTAT_SEND_NOSTOP)
   {
      //
      // Check if bus busy
      //
      if(I2caRegs.I2CSTR.bit.BB == 1)
      {
         return I2C_BUS_BUSY_ERROR;
      }
      I2caRegs.I2CCNT = 2;
      I2caRegs.I2CDXR.all = msg->MemoryHighAddr;
      I2caRegs.I2CDXR.all = msg->MemoryLowAddr;
      I2caRegs.I2CMDR.all = 0x2660; // Send data to setup EEPROM address 0x2660
   }
   else if(msg->MsgStatus == I2C_MSGSTAT_RESTART)
   {
      I2caRegs.I2CCNT = msg->NumOfBytes;    // Setup how many bytes to expect
      I2caRegs.I2CMDR.all = 0x2C60;         // Send restart as master receiver
   }

   return I2C_SUCCESS;
}

void I2C_Init(void)
{
    /*
    GpioCtrlRegs.GPBPUD.bit.GPIO42 = 0;        // Enable pull-up for GPIO32 (SDAA)
    GpioCtrlRegs.GPBQSEL1.bit.GPIO42 = 3;      // Asynch input GPIO32 (SDAA)
    GpioCtrlRegs.GPBMUX1.bit.GPIO42 = 1;    // 0=GPIO,  1=I2CSDA,  2=SYNCI,  3=ADCSOCA

        // GPIO33 - uC_CSAC_SCL
    GpioCtrlRegs.GPBPUD.bit.GPIO43 = 0;        // Enable pull-up for GPIO32 (SCLA)
    GpioCtrlRegs.GPBQSEL1.bit.GPIO43 = 3;      // Asynch input GPIO32 (SLCA)
    GpioCtrlRegs.GPBMUX1.bit.GPIO43 = 1;    // 0=GPIO,  1=I2CSCL,  2=SYNCO,  3=ADCSOCB
*/

    I2caRegs.I2CSAR.all = 0x0050;     // Slave address - EEPROM control code

    I2caRegs.I2CPSC.all = 6;          // Prescaler - need 7-12 Mhz on module clk
    I2caRegs.I2CCLKL = 10;            // NOTE: must be non zero
    I2caRegs.I2CCLKH = 5;             // NOTE: must be non zero
    I2caRegs.I2CIER.all = 0x24;       // Enable SCD & ARDY __interrupts

    I2caRegs.I2CMDR.all = 0x0060;     // Take I2C out of reset
                                      // Stop I2C when suspended

    I2caRegs.I2CFFTX.all = 0x6000;    // Enable FIFO mode and TXFIFO
    I2caRegs.I2CFFRX.all = 0x2040;    // Enable RXFIFO, clear RXFFINT,
    return;
}


//
// pass - Halt debugger and signify pass
//
void pass()
{
    passs = 1;
 //  asm("   ESTOP0");
  // for(;;);
}

//
// fail - Halt debugger and signify fail
//
void fail()
{
    fails = 1;
   asm("   ESTOP0");
   for(;;);
}




