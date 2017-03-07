
#ifndef METATAIT_ISR_H_
#define METATAIT_ISR_H_
#include "F28x_Project.h"


//Initialization for Vector Table/PIE control registers
void external_interrupt_init(void);
void vector_table_init(void);
void enable_pie_block(void);

/* Interrupt Service Routines */
__interrupt void cpu_timer0_isr(void);
__interrupt void epwm2_isr(void);
__interrupt void xint1_isr(void);
__interrupt void i2c_int1a_isr(void);






#endif /* METATAIT_ISR_H_ */
