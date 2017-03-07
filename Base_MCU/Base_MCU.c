#include "F28x_Project.h"
#include <metaTait_HighLevel.h>           //Houses all of the prototypes, structures, and #defines
#include <metaTait_I2C.h>
#include <metaTait_ISR.h>
#include <metaTait_SCI.h>
#include <metaTait_PWM.h>
#include <metaTait_SCI.h>

extern int rpm;

int main(void)
{
    InitSysCtrl();
    InitCpuTimers();   //Enable CPU Timers
    InitGpio();
    GPIO_SetupPinMux(32, GPIO_MUX_CPU1, 1);
    GPIO_SetupPinMux(33, GPIO_MUX_CPU1, 1);
    DINT;
    InitPieCtrl();

  //
  // Disable CPU __interrupts and clear all CPU __interrupt flags:
  //
     IER = 0x0000;
     IFR = 0x0000;

  //
  // Initialize the PIE vector table with pointers to the shell Interrupt
  // Service Routines (ISR).
  // This will populate the entire table, even if the __interrupt
  // is not used in this example.  This is useful for debug purposes.
  // The shell ISR routines are found in F2837xS_DefaultIsr.c.
  // This function is found in F2837xS_PieVect.c.
  //
     InitPieVectTable();
     EALLOW;    // This is needed to write to EALLOW protected registers
     PieVectTable.I2CA_INT = &i2c_int1a_isr;
     EDIS;      // This is needed to disable write to EALLOW protected registers

     I2C_Init();                        //Initialize I2C
     PieCtrlRegs.PIEIER8.bit.INTx1 = 1;
     IER |= M_INT8;
     EINT;



    sci_init();                        //Initialize UART


    EINT;  // Enable Global interrupt INTM
    ERTM;  // Enable Global realtime interrupt DBGM

    receive_data();     //Receive Start Command and RPM value from terminal.

    DINT;


    CpuSysRegs.PCLKCR2.bit.EPWM2=1;

    ConfigCpuTimer(&CpuTimer0, 200, 50);        //Initialize CPU Timer Interrupt to 50 us
    CpuTimer0Regs.TCR.all = 0x4001;

    InitEPwm2Gpio();                            //Initialize PWM

    vector_table_init();                        //Enable vector table of interrupts

    EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 0;
    EDIS;

    InitEPwm2Example();                         //Initialize PWM waveform

    external_interrupt_init();

    IER |= M_INT3;
    IER |= M_INT1;

    enable_pie_block();

    EINT;  // Enable Global interrupt INTM
    ERTM;  // Enable Global realtime interrupt DBGM

    for(;;)
    {
        /*if(SciaRegs.SCIFFRX.bit.RXFFST == 0);
            stop_Command();*/

        if(rpm == 10000);
            //return 0;
    }
}


