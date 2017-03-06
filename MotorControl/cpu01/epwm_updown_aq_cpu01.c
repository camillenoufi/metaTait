
// Included Files
//
#include "F28x_Project.h"
#include "stdio.h"

#define DUTY_CYCLE				0.60								//If you want, say 80% duty cycle, you do not type 4/5, you type in 1/5.
#define	 EPWM2_TIMER_TBPRD  	5556								//TBPRD = CPU TIMER /DESIRED PWM FREQUENCY   I THINK CPU TIMER = 50 MHZ RIGHT NOW.
#define EPWM_CMP_UP           	1
#define EPWM_CMP_DOWN         	0

//
// Globals
//
long counter1= 0;
long counter2= 0;
int countup = 0;
int changeup1 = 0;
int changeup2 = 0;
float duty_cycle = 0.60;

typedef struct
{
    volatile struct EPWM_REGS *EPwmRegHandle;
    Uint16 EPwm_CMPA_Direction;
    Uint16 EPwm_CMPB_Direction;
    Uint16 EPwmTimerIntCount;
    Uint16 EPwmMaxCMPA;
    Uint16 EPwmMinCMPA;
    Uint16 EPwmMaxCMPB;
    Uint16 EPwmMinCMPB;
}EPWM_INFO;

EPWM_INFO epwm2_info;

//
// Function Prototypes
//
void InitEPwm2Example(void);
__interrupt void cpu_timer0_isr(void);
__interrupt void epwm2_isr(void);
__interrupt void xint1_isr(void);

//
// Main
//
void main(void)
{
    InitSysCtrl();
    InitCpuTimers();   // For this example, only initialize the Cpu Timers

 //
 // Configure CPU-Timer 0 to __interrupt every 50 microseconds:
 // 200MHz CPU Freq

    ConfigCpuTimer(&CpuTimer0, 200, 50);
    CpuTimer0Regs.TCR.all = 0x4001;


    InitGpio();

    EALLOW;

    InputXbarRegs.INPUT4SELECT = 16;
    XintRegs.XINT1CR.bit.POLARITY = 1;     // Trigger XINT1 on a rising edge
    XintRegs.XINT1CR.bit.ENABLE = 1;       // Enable XINT1
    GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO12 = 1;

    GpioCtrlRegs.GPAMUX1.bit.GPIO13 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO13 = 1;
    CpuSysRegs.PCLKCR2.bit.EPWM2=1;		//Allow EPWM
    EDIS;


//
// For this case just init GPIO pins for ePWM2
// These functions are in the F2837xS_EPwm.c file
//
    InitEPwm2Gpio();

// Disable CPU interrupts
//
    DINT;
    InitPieCtrl();

// Disable CPU interrupts and clear all CPU interrupt flags:

    IER = 0x0000;
    IFR = 0x0000;

    InitPieVectTable();

    EALLOW; // This is needed to write to EALLOW protected registers
    PieVectTable.EPWM2_INT = &epwm2_isr;
    PieVectTable.TIMER0_INT = &cpu_timer0_isr;
    PieVectTable.XINT1_INT = &xint1_isr;						//GPIO Int
    EDIS;   // This is needed to disable write to EALLOW protected registers

    EALLOW;

    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 0;
    EDIS;

    GPIO_SetupXINT1Gpio(61);				//GPIO Int
    InitEPwm2Example();

    EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;
    EDIS;

// Enable CPU INT3 which is connected to EPWM1-3 INT:

    IER |= M_INT3;
    // Enable CPU INT1 which is connected to CPU-Timer 0 and GPIO Interrupt:
    IER |= M_INT1;

    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;          // Enable the PIE block
// Enable EPWM INTn in the PIE: Group 3 interrupt 1-3
    PieCtrlRegs.PIEIER3.bit.INTx2 = 1;
// Enable TINT0 in the PIE: Group 1 __interrupt 7
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;
    //Enabling GPIO Interrupt Group 1 interrupt 4
    PieCtrlRegs.PIEIER1.bit.INTx4 = 1;          // Enable PIE Group 1 INT4 GPIO



//
// Enable global Interrupts and higher priority real-time debug events:
//
    EINT;  // Enable Global interrupt INTM
    ERTM;  // Enable Global realtime interrupt DBGM

//
// Step 5. IDLE loop. Just sit and loop forever (optional):
//
	GpioDataRegs.GPATOGGLE.bit.GPIO12 = 1;
    for(;;)
    {
        asm ("    NOP");
    }
}

interrupt void xint1_isr(void)
{
	GpioDataRegs.GPATOGGLE.bit.GPIO12 = 1;
	float uts = 20000.0;											//This is the conversion from counter ticks to seconds.
	float seconds = counter1/uts; //time in seconds per rotation
	float rotations_per_second = 1/seconds;
	float stm = 60.0;
	float rpm = rotations_per_second * stm; //rotations per minute.
	float change = 0.05;
	float target = 500.0;
	if(counter2 == 10000)
	{
		if(rpm != target)
		{
			if(rpm < target)
			{
				changeup1 = 0;
				duty_cycle = duty_cycle - change;
                epwm2_info->EPwmRegHandle->CMPA.bit.CMPA = EPWM2_TIMER_TBPRD*duty_cycle;
                epwm2_info->EPwmRegHandle->CMPB.bit.CMPB = EPWM2_TIMER_TBPRD*duty_cycle;
				if(changeup1 != changeup2)
				{
					change = change*0.97;
					changeup2 = changeup1;
				}
			}
			if(rpm > target)
			{
				changeup2 = 1;
				duty_cycle = duty_cycle + change;
                epwm2_info->EPwmRegHandle->CMPA.bit.CMPA = EPWM2_TIMER_TBPRD*duty_cycle;
                epwm2_info->EPwmRegHandle->CMPB.bit.CMPB = EPWM2_TIMER_TBPRD*duty_cycle;

				if(changeup2 != changeup1)
				{
					change = change*0.97;
					changeup1 = changeup2;
				}
			}
		}
		countup = 0;
		counter2 = 0;
	}
	countup++;
	counter1 = 0;
	// Acknowledge this interrupt to get more from group 1
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}


__interrupt void cpu_timer0_isr(void)
{
   counter1++;
   counter2++;
   CpuTimer0.InterruptCount++;
   //
   // Acknowledge this __interrupt to receive more __interrupts from group 1
   //
   PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}



__interrupt void epwm2_isr(void)
{
    //
    // Update the CMPA and CMPB values
    //
 //   update_compare(&epwm2_info);

    //
    // Clear INT flag for this timer
    //
    EPwm2Regs.ETCLR.bit.INT = 1;

    //
    // Acknowledge this interrupt to receive more interrupts from group 3
    //
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}


void InitEPwm2Example()
{
    //
    // Setup TBCLK
    //
    EPwm2Regs.TBPRD = EPWM2_TIMER_TBPRD;         // Set timer period 801 TBCLKs
    EPwm2Regs.TBPHS.bit.TBPHS = 0x0000;          // Phase is 0
    EPwm2Regs.TBCTR = 0x0000;                    // Clear counter

    //
    // Set Compare values
    //

    EPwm2Regs.CMPA.bit.CMPA = EPWM2_TIMER_TBPRD*duty_cycle;    // Set compare A value
    EPwm2Regs.CMPB.bit.CMPB = EPWM2_TIMER_TBPRD*duty_cycle;    // Set Compare B value

    //
    // Setup counter mode
    //
    EPwm2Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up and down
    EPwm2Regs.TBCTL.bit.PHSEN = TB_DISABLE;        // Disable phase loading
    EPwm2Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;       // Clock ratio to SYSCLKOUT
    EPwm2Regs.TBCTL.bit.CLKDIV = TB_DIV1;


    EPwm2Regs.TBCTR = 0x0000; // Clear counter
    EPwm2Regs.TBCTL.bit.HSPCLKDIV = 0x0; // Clock ratio to SYSCLKOUT
    EPwm2Regs.TBCTL.bit.CLKDIV = 0x0;

    //
    // Setup shadowing
    //
    EPwm2Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
    EPwm2Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    EPwm2Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO; // Load on Zero
    EPwm2Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

    //
    // Set actions
    //
    EPwm2Regs.AQCTLA.bit.CAU = AQ_SET;         // Set PWM2A on event A, up
                                               // count
    EPwm2Regs.AQCTLA.bit.CBD = AQ_CLEAR;       // Clear PWM2A on event B, down
                                               // count

    EPwm2Regs.AQCTLB.bit.ZRO = AQ_CLEAR;       // Clear PWM2B on zero
    EPwm2Regs.AQCTLB.bit.PRD = AQ_SET;         // Set PWM2B on period

    //
    // Interrupt where we will change the Compare Values
    //
    EPwm2Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;    // Select INT on Zero event
    EPwm2Regs.ETSEL.bit.INTEN = 1;               // Enable INT
    EPwm2Regs.ETPS.bit.INTPRD = ET_3RD;          // Generate INT on 3rd event

    //
    // Information this example uses to keep track
    // of the direction the CMPA/CMPB values are
    // moving, the min and max allowed values and
    // a pointer to the correct ePWM registers
    //
    epwm2_info.EPwm_CMPA_Direction = EPWM_CMP_UP;  // Start by increasing CMPA
    epwm2_info.EPwm_CMPB_Direction = EPWM_CMP_UP;  // & increasing CMPB
    epwm2_info.EPwmTimerIntCount = 0;              // Zero the interrupt counter
    epwm2_info.EPwmRegHandle = &EPwm2Regs;         // Set the pointer to the
                                                   // ePWM module
    epwm2_info.EPwmMaxCMPA = EPWM2_TIMER_TBPRD*duty_cycle;       // Setup min/max CMPA/CMPB
                                                   // values
    epwm2_info.EPwmMinCMPA = EPWM2_TIMER_TBPRD*duty_cycle;
    epwm2_info.EPwmMaxCMPB = EPWM2_TIMER_TBPRD*duty_cycle;
    epwm2_info.EPwmMinCMPB = EPWM2_TIMER_TBPRD*duty_cycle;
}





void update_compare(EPWM_INFO *epwm_info)
{
    //
    // Every 10'th interrupt, change the CMPA/CMPB values
    //
    if(epwm_info->EPwmTimerIntCount == 10)
    {
        epwm_info->EPwmTimerIntCount = 0;

        //
        // If we were increasing CMPA, check to see if
        // we reached the max value.  If not, increase CMPA
        // else, change directions and decrease CMPA
        //
        if(epwm_info->EPwm_CMPA_Direction == EPWM_CMP_UP)
        {
            if(epwm_info->EPwmRegHandle->CMPA.bit.CMPA <
               epwm_info->EPwmMaxCMPA)
            {
                epwm_info->EPwmRegHandle->CMPA.bit.CMPA++;
            }
            else
            {
                epwm_info->EPwm_CMPA_Direction = EPWM_CMP_DOWN;
                epwm_info->EPwmRegHandle->CMPA.bit.CMPA--;
            }
        }

        //
        // If we were decreasing CMPA, check to see if
        // we reached the min value.  If not, decrease CMPA
        // else, change directions and increase CMPA
        //
        else
        {
            if(epwm_info->EPwmRegHandle->CMPA.bit.CMPA ==
               epwm_info->EPwmMinCMPA)
            {
                epwm_info->EPwm_CMPA_Direction = EPWM_CMP_UP;
                epwm_info->EPwmRegHandle->CMPA.bit.CMPA++;
            }
            else
            {
                epwm_info->EPwmRegHandle->CMPA.bit.CMPA--;
            }
        }

        //
        // If we were increasing CMPB, check to see if
        // we reached the max value.  If not, increase CMPB
        // else, change directions and decrease CMPB
        //
        if(epwm_info->EPwm_CMPB_Direction == EPWM_CMP_UP)
        {
            if(epwm_info->EPwmRegHandle->CMPB.bit.CMPB < epwm_info->EPwmMaxCMPB)
            {
                epwm_info->EPwmRegHandle->CMPB.bit.CMPB++;
            }
            else
            {
                epwm_info->EPwm_CMPB_Direction = EPWM_CMP_DOWN;
                epwm_info->EPwmRegHandle->CMPB.bit.CMPB--;
            }
        }

        //
        // If we were decreasing CMPB, check to see if
        // we reached the min value.  If not, decrease CMPB
        // else, change directions and increase CMPB
        //
        else
        {
            if(epwm_info->EPwmRegHandle->CMPB.bit.CMPB == epwm_info->EPwmMinCMPB)
            {
                epwm_info->EPwm_CMPB_Direction = EPWM_CMP_UP;
                epwm_info->EPwmRegHandle->CMPB.bit.CMPB++;
            }
            else
            {
                epwm_info->EPwmRegHandle->CMPB.bit.CMPB--;
            }
        }
    }
    else
    {
        epwm_info->EPwmTimerIntCount++;
    }

    return;
}

//
// End of file
//
