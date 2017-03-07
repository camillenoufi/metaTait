
#ifndef METATAIT_I2C_H_
#define METATAIT_I2C_H_
#include "F28x_Project.h"


#define Slave_Addr1             0x0000      //Post 1 Slave Address
#define Slave_Addr2             0x3232      //Post 2 Slave Address
#define Slave_Addr3             0x3232      //Post 3 Slave Address

#define Start_Addr1             0x0050      //Post 1 Start Address, Offset from Slave Address
#define Start_Addr2             0x3232      //Post 2 Start Address, Offset from Slave Address
#define Start_Addr3             0x3232      //Post 3 Start Address, Offset from Slave Address

#define I2C_NUMBYTES          2

/*struct I2CMSG {     For Reference
  Uint16 MsgStatus;             // Word stating what state msg is in:
                                //  I2C_MSGCMD_INACTIVE = do not send msg
                                //  I2C_MSGCMD_BUSY = msg start has been sent,
                                //                    awaiting stop
                                //  I2C_MSGCMD_SEND_WITHSTOP = command to send
                                //    master trans msg complete with a stop bit
                                //  I2C_MSGCMD_SEND_NOSTOP = command to send
                                //    master trans msg without the stop bit
                                //  I2C_MSGCMD_RESTART = command to send a
                                //    restart as a master receiver with a
                                //    stop bit
  Uint16 SlaveAddress;          // I2C address of slave msg is intended for
  Uint16 NumOfBytes;            // Num of valid bytes in (or to be put
                                // in MsgBuffer)
  Uint16 MemoryHighAddr;        // EEPROM address of data associated with
                                // msg (high byte)
  Uint16 MemoryLowAddr;         // EEPROM address of data associated with
                                // msg (low byte)
  Uint16 MsgBuffer[I2C_MAX_BUFFER_SIZE];  // Array holding msg data - max that
                                          // MAX_BUFFER_SIZE can be is 16 due
                                          // to the FIFO's
};*/



/* I2C Send/Receive/Initialization Functions */
Uint16 I2C_Write(struct I2CMSG *);
Uint16 I2C_Read(struct I2CMSG *);
void I2C_Init(void);
//
// pass - Halt debugger and signify pass
//
void pass(void);

//
// fail - Halt debugger and signify fail
//
void fail(void);




#endif /* METATAIT_I2C_H_ */
