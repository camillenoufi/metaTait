#include "metaTait_PWM.h"
#include "F28x_Project.h"
extern int rpm;
extern int target;
extern EPWM_INFO epwm2_info;


void InitEPwm2Example()
{
   EPwm2Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP; // Count up
   EPwm2Regs.TBPRD = EPWM2_TIMER_TBPRD;       // Set timer period
   EPwm2Regs.TBCTL.bit.PHSEN = TB_DISABLE;    // Disable phase loading
   EPwm2Regs.TBPHS.bit.TBPHS = 0x0000;        // Phase is 0
   EPwm2Regs.TBCTR = 0x0000;                  // Clear counter
   EPwm2Regs.TBCTL.bit.HSPCLKDIV = TB_DIV2;   // Clock ratio to SYSCLKOUT
   EPwm2Regs.TBCTL.bit.CLKDIV = TB_DIV2;
   EPwm2Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
   EPwm2Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
   EPwm2Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
   EPwm2Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;
   EPwm2Regs.CMPA.bit.CMPA = EPWM2_MIN_CMPA;      // Set compare A value
   EPwm2Regs.CMPB.bit.CMPB = EPWM2_MAX_CMPB;      // Set Compare B value
   EPwm2Regs.AQCTLA.bit.PRD = AQ_CLEAR;            // Clear PWM2A on Period
   EPwm2Regs.AQCTLA.bit.CAU = AQ_SET;              // Set PWM2A on event A, up count
   EPwm2Regs.AQCTLB.bit.PRD = AQ_CLEAR;            // Clear PWM2B on Period
   EPwm2Regs.AQCTLB.bit.CBU = AQ_SET;              // Set PWM2B on event B, up count
   EPwm2Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;       // Select INT on Zero event
   EPwm2Regs.ETSEL.bit.INTEN = 1;                  // Enable INT
   EPwm2Regs.ETPS.bit.INTPRD = ET_3RD;             // Generate INT on 3rd event
   epwm2_info.EPwm_CMPA_Direction = EPWM_CMP_UP;   // Start by increasing CMPA
   epwm2_info.EPwm_CMPB_Direction = EPWM_CMP_DOWN; // and decreasing CMPB
   epwm2_info.EPwmTimerIntCount = 0;               // Zero the interrupt counter
   epwm2_info.EPwmRegHandle = &EPwm2Regs;          // Set the pointer to the ePWM module
   epwm2_info.EPwmMaxCMPA = EPWM2_MAX_CMPA;        // Setup min/max
   epwm2_info.EPwmMinCMPA = EPWM2_MIN_CMPA;        // CMPA/CMPB values
   epwm2_info.EPwmMaxCMPB = EPWM2_MAX_CMPB;
   epwm2_info.EPwmMinCMPB = EPWM2_MIN_CMPB;
}




void update_compare(EPWM_INFO *epwm_info)
{
    if(epwm_info->EPwmTimerIntCount == 100)  //after period of time, compare rpm to target, increment or decrement duty cycle
    {
        epwm_info->EPwmTimerIntCount = 0; // reset counter

        if(rpm < target - (target/100))         //1%
        {
            if((epwm_info->EPwm_CMPA_Direction == EPWM_CMP_UP))
                epwm_info->EPwmRegHandle->CMPA.bit.CMPA--;

            if(epwm_info->EPwm_CMPA_Direction == EPWM_CMP_DOWN)
            {
                epwm_info->EPwm_CMPA_Direction = EPWM_CMP_UP;
                epwm_info->EPwmRegHandle->CMPA.bit.CMPA--;
            }
        }
        if (rpm > target + (target/100))            //1%
        {
            if(epwm_info->EPwm_CMPA_Direction == EPWM_CMP_DOWN)
                epwm_info->EPwmRegHandle->CMPA.bit.CMPA++;

            if(epwm_info->EPwm_CMPA_Direction == EPWM_CMP_UP)
            {
                epwm_info->EPwm_CMPA_Direction = EPWM_CMP_DOWN;
                epwm_info->EPwmRegHandle->CMPA.bit.CMPA++;
            }
        }
    }
    else
        epwm_info->EPwmTimerIntCount++;

   return;
}




