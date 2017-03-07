#include <metaTait_HighLevel.h>
#include "metaTait_ISR.h"
#include "F28x_Project.h"
#include "metaTait_PWM.h"
#include "metaTait_I2C.h"
#include "metaTait_SCI.h"

unsigned long counter1 = 0;
extern int rpm;
extern int target;
EPWM_INFO epwm2_info;

struct I2CMSG *CurrentMsgPtr;
Uint16 PassCount;
Uint16 FailCount;

extern struct I2CMSG I2cMsgOut1;
extern struct I2CMSG I2cMsgOut2;
extern struct I2CMSG I2cMsgOut3;
extern struct I2CMSG I2cMsgIn1;
extern struct I2CMSG I2cMsgIn2;
extern struct I2CMSG I2cMsgIn3;



__interrupt void cpu_timer0_isr(void)
{
   counter1++;
   CpuTimer0.InterruptCount++;
   PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}



__interrupt void xint1_isr(void)
{
    float uts = 20000.0;                    //This is the conversion from counter ticks to seconds. 20000*50us = 1 s
    float seconds = counter1/uts;           //seconds per rotation
    float espf = (seconds*1000000.0)/216.0;       //microseconds per refresh
    Uint16 spf = (Uint16)espf;
    float rotations_per_second = 1/seconds;
    float stm = 60.0;                       //Seconds to minutes
    float arpm = rotations_per_second * stm; //rotations per minute.
    rpm = (int)arpm + 1;
    counter1 = 0;

    scia_xmit(4, rpm);                      //Send for debugging purposes the rpm to terminal.

    spf = spf*1;
    //send out spf
    I2C_Write(&I2cMsgOut1);
    I2C_Write(&I2cMsgOut2);
    I2C_Write(&I2cMsgOut3);

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}



__interrupt void epwm2_isr(void)
{
    update_compare(&epwm2_info);        //Change Duty Cycle
    EPwm2Regs.ETCLR.bit.INT = 1;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}


//
// i2c_int1a_isr - I2CA ISR
//
__interrupt void i2c_int1a_isr(void)
{
   Uint16 IntSource, i;

   //
   // Read __interrupt source
   //
   IntSource = I2caRegs.I2CISRC.all;

   //
   // Interrupt source = stop condition detected
   //
   if(IntSource == I2C_SCD_ISRC)
   {
      //
      // If completed message was writing data, reset msg to inactive state
      //
      if(CurrentMsgPtr->MsgStatus == I2C_MSGSTAT_WRITE_BUSY)
      {
         CurrentMsgPtr->MsgStatus = I2C_MSGSTAT_INACTIVE;
      }
      else
      {
         //
         // If a message receives a NACK during the address setup portion of
         // the EEPROM read, the code further below included in the register
         // access ready __interrupt source code will generate a stop
         // condition. After the stop condition is received (here), set the
         // message status to try again. User may want to limit the number of
         // retries before generating an error.
         if(CurrentMsgPtr->MsgStatus == I2C_MSGSTAT_SEND_NOSTOP_BUSY)
         {
            CurrentMsgPtr->MsgStatus = I2C_MSGSTAT_SEND_NOSTOP;
         }
         //
         // If completed message was reading EEPROM data, reset msg to inactive
         // state and read data from FIFO.
         //
         else if(CurrentMsgPtr->MsgStatus == I2C_MSGSTAT_READ_BUSY)
         {
            CurrentMsgPtr->MsgStatus = I2C_MSGSTAT_INACTIVE;

            for(i = 0; i < I2C_NUMBYTES-1; i++)
            {
                CurrentMsgPtr->MsgBuffer[i] = I2caRegs.I2CDRR.all;
            }
            for(i=0; i < I2C_NUMBYTES+1; i++)
            {
              CurrentMsgPtr->MsgBuffer[i] = I2caRegs.I2CDRR.all;
            }
         }
         //
         // Check received data
         //
         for(i=0; i < I2C_NUMBYTES; i++)
         {
            if(I2cMsgIn1.MsgBuffer[i] == I2cMsgOut1.MsgBuffer[i])
            {
                PassCount++;
            }
            else
            {
                FailCount++;
            }
         }
         if(PassCount == I2C_NUMBYTES)
         {
            pass();
         }
         else
         {
            fail();
         }
        }
       }



   //
   // Interrupt source = Register Access Ready
   // This __interrupt is used to determine when the EEPROM address setup
   // portion of the read data communication is complete. Since no stop bit is
   // commanded, this flag tells us when the message has been sent instead of
   // the SCD flag. If a NACK is received, clear the NACK bit and command a
   // stop. Otherwise, move on to the read data portion of the communication.
   //
   else if(IntSource == I2C_ARDY_ISRC)
   {
      if(I2caRegs.I2CSTR.bit.NACK == 1)
      {
         I2caRegs.I2CMDR.bit.STP = 1;
         I2caRegs.I2CSTR.all = I2C_CLR_NACK_BIT;
      }
      else if(CurrentMsgPtr->MsgStatus == I2C_MSGSTAT_SEND_NOSTOP_BUSY)
      {
         CurrentMsgPtr->MsgStatus = I2C_MSGSTAT_RESTART;
      }
   }
   else
   {
      //
      // Generate some error due to invalid __interrupt source
      //
      asm("   ESTOP0");
   }

   //
   // Enable future I2C (PIE Group 8) __interrupts
   //
   PieCtrlRegs.PIEACK.all = PIEACK_GROUP8;
}




void enable_pie_block(void)
{
    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;          // Enable the PIE block
    PieCtrlRegs.PIEIER1.bit.INTx4 = 1;
    PieCtrlRegs.PIEIER3.bit.INTx2 = 1;
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;
    PieCtrlRegs.PIEIER1.bit.INTx5 = 1;          // Enable PIE Group 1 INT5
}



void vector_table_init(void)
{
    EALLOW; // This is needed to write to EALLOW protected registers
    PieVectTable.EPWM2_INT = &epwm2_isr;
    PieVectTable.TIMER0_INT = &cpu_timer0_isr;
    PieVectTable.XINT1_INT = &xint1_isr;
    EDIS;   // This is needed to disable write to EALLOW protected registers
}



void external_interrupt_init(void)
{
    //GPIO4 is Pin 93 on PCB
    EALLOW;
    GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 0;         // GPIO
    GpioCtrlRegs.GPADIR.bit.GPIO4 = 0;          // input
    GpioCtrlRegs.GPAQSEL1.bit.GPIO4 = 0;        // XINT1 Synch to SYSCLKOUT only
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;
    EDIS;

    GPIO_SetupXINT1Gpio(4);                     //Pin 19 is external interrupt
    XintRegs.XINT1CR.bit.POLARITY = 0;          // Falling edge interrupt
    XintRegs.XINT1CR.bit.ENABLE = 1;            // Enable XINT1
}








