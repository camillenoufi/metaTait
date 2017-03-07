
#ifndef METATAIT_PWM_H_
#define METATAIT_PWM_H_

#include "F28x_Project.h"

#define EEPROM_24LC256_ADDRESS 0x50         //Will not use EEPROM, but need #define.
#define EPWM2_TIMER_TBPRD  2778  // Period register
#define EPWM2_MAX_CMPA     1253  //1667 = starting duty cycle - 40 %   51.71% for 500  54.88% for 600 rpm
#define EPWM2_MIN_CMPA     1667  //Provides start value for PWM duty cycle.
#define EPWM2_MAX_CMPB     1100  //Not used in this code, for Pin 79.
#define EPWM2_MIN_CMPB       50
#define EPWM_CMP_UP           1   //Determine Direction of Adjustment of comparator values
#define EPWM_CMP_DOWN         0

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



/* EPWM Motor Control  */
void update_compare(EPWM_INFO*);
void InitEPwm2Example(void);


#endif /* METATAIT_PWM_H_ */
