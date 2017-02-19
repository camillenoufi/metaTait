#include "F28x_Project.h"

//
// Defines
//

// Choose a word size for MCBSP DMA.  Uncomment one of the following lines
//
#define WORD_SIZE  8      // Run a loopback test in 8-bit mode
//#define WORD_SIZE 16      // Run a loopback test in 16-bit mode
//#define WORD_SIZE 32      // Run a loopback test in 32-bit mode

// Values for I2C Communication
#define I2C_SLAVE_ADDR        0x50
#define I2C_NUMBYTES          2
#define I2C_EEPROM_HIGH_ADDR  0x00
#define I2C_EEPROM_LOW_ADDR   0x30

//
// Globals
//

#pragma DATA_SECTION(sdata, "ramgs0") // Place sdata and rdata buffers in
#pragma DATA_SECTION(rdata, "ramgs1") // DMA-accessible RAM
Uint16 sdata[128];                    // Sent Data
Uint16 rdata[128];                    // Received Data
Uint16 data_size;                     // Word Length variable


struct I2CMSG I2cMsgOut1={ I2C_MSGSTAT_SEND_WITHSTOP,
                           I2C_SLAVE_ADDR,
                           I2C_NUMBYTES,
                           I2C_EEPROM_HIGH_ADDR,
                           I2C_EEPROM_LOW_ADDR,
                           0x12,                   // Msg Byte 1
                           0x34};                  // Msg Byte 2

struct I2CMSG I2cMsgIn1={ I2C_MSGSTAT_SEND_NOSTOP,
                          I2C_SLAVE_ADDR,
                          I2C_NUMBYTES,
                          I2C_EEPROM_HIGH_ADDR,
                          I2C_EEPROM_LOW_ADDR};

struct I2CMSG *CurrentMsgPtr;
Uint16 PassCount;
Uint16 FailCount;

//
// Function Prototypes
//

// I2C Prototypes
void   I2CA_Init(void);
Uint16 I2CA_WriteData(struct I2CMSG *msg);
Uint16 I2CA_ReadData(struct I2CMSG *msg);
__interrupt void i2c_int1a_isr(void);
void pass(void);
void fail(void);

//MCBSP DMA Prototypes
__interrupt void local_D_INTCH1_ISR(void);
__interrupt void local_D_INTCH2_ISR(void);
void mcbsp_init_dlb(void);
void init_dma(void);
void init_dma_32(void);
void start_dma(void);
void error(void);

//High Speed SPI Prototypes
void spi_xmita(Uint16 a);		//16 bit transmit out of SPI Port A
void spi_xmitb(Uint16 a);		//16 bit transmit out of SPI Port B
void spi_xmitc(Uint16 a);		//16 bit transmit out of SPI Port C
void spi_fifo_init(void);		//Initialize FIFO Buffers


int main(void) {

	//
	// Initialize System Control:
	// PLL, WatchDog, enable Peripheral Clocks
	// This example function is found in the F2837xS_SysCtrl.c file.
	//

	 InitSysCtrl();				//Initialize System Control, Where you can change PLL system clock

	 //
	 // Initialize GPIO:
	 // These example functions are found in the F2837xS_Gpio.c file and
	 // illustrates how to set the GPIO to it's default state.
	 // Setup only the GP I/O only for McBSP-A functionality
	 //

	 InitSpiGpio();				//Initialize GPIO pins for SPI Communication

	 InitMcbspaGpio();          // Initialize GPIO pins for MCBSP DMA

	 GPIO_SetupPinMux(32, GPIO_MUX_CPU1, 1);     // For I2C communication
	 GPIO_SetupPinMux(33, GPIO_MUX_CPU1, 1);     // For I2C communication

	 //
	 // Clear all interrupts and initialize PIE vector table:
	 // Disable CPU interrupts
	 //

	 DINT;						//Clear all interrupts

	 //
	 // Initialize PIE control registers to their default state.
	 // The default state is all PIE interrupts disabled and flags
	 // are cleared.
	 // This function is found in the F2837xS_PieCtrl.c file.
	 //

	 InitPieCtrl();

	    IER = 0x0000;				//Disable CPU interrupts
	    IFR = 0x0000;				//Clear interrupt flags

	 //
	 // Initialize the PIE vector table with pointers to the shell Interrupt
     // Service Routines (ISR).
	 // This will populate the entire table, even if the interrupt
     // is not used in this example.  This is useful for debug purposes.
     // The shell ISR routines are found in F2837xS_DefaultIsr.c.
	 // This function is found in F2837xS_PieVect.c.
	 //

	 InitPieVectTable();
	 spi_fifo_init();     		// Initialize the SPI FIFO,

	 // Interrupts that are used in this example are re-mapped to
	 // ISR functions found within this file.
	 //  Write address of I2C ISR and DMA ISRs into PieVectTable register

	    EALLOW;    // This is needed to write to EALLOW protected registers
	    PieVectTable.I2CA_INT = &i2c_int1a_isr;
	    PieVectTable.DMA_CH1_INT= &local_D_INTCH1_ISR;
	     PieVectTable.DMA_CH2_INT= &local_D_INTCH2_ISR;
	    EDIS;      // This is needed to disable write to EALLOW protected registers

	    //
	    // Ensure DMA is connected to PF2SEL bridge (EALLOW protected)
	    //
	       EALLOW;
	       CpuSysRegs.SECMSEL.bit.PF2SEL = 1;
	       EDIS;
	    //
	    //MCBSP DMA Code
	    //
	       data_size = WORD_SIZE;

	       for (i=0; i<128; i++)
	         {
	             sdata[i] = i;      // Fill sdata with values between 0 and 0x007F
	             rdata[i] = 0;      // Initialize rdata to all 0x0000.
	         }

	       if (data_size == 32)
	         {
	             init_dma_32();     // DMA Initialization for 32-bit transfers
	         }
	         else
	         {
	             init_dma();        // 1. When using DMA, initialize DMA with
	                                //    peripheral interrupts first.
	         }
	         start_dma();
	         mcbsp_init_dlb();      // 2. Then initialize and release peripheral
	                                //   (McBSP) from Reset.

	         //
	         // Enable interrupts required for this example
	         //
	            PieCtrlRegs.PIECTRL.bit.ENPIE = 1;   // Enable the PIE block
	            PieCtrlRegs.PIEIER7.bit.INTx1 = 1;   // Enable PIE Group 7, INT 1 (DMA CH1)
	            PieCtrlRegs.PIEIER7.bit.INTx2 = 1;   // Enable PIE Group 7, INT 2 (DMA CH2)

	            IER=0x40;                            // Enable CPU INT groups 6 and 7
	            EINT;                                // Enable Global Interrupts

	         //
	         // Step 5. IDLE loop. Just sit and loop forever (optional):
	         //
	            for(;;);

	return 0;
}

//
// I2CA_Init - Initialize I2CA settings
//
void I2CA_Init(void)
{
   I2caRegs.I2CSAR.all = 0x0050;     // Slave address - EEPROM control code

   I2caRegs.I2CPSC.all = 6;          // Prescaler - need 7-12 Mhz on module clk
   I2caRegs.I2CCLKL = 10;            // NOTE: must be non zero
   I2caRegs.I2CCLKH = 5;             // NOTE: must be non zero
   I2caRegs.I2CIER.all = 0x24;       // Enable SCD & ARDY __interrupts

   I2caRegs.I2CMDR.all = 0x0020;     // Take I2C out of reset
                                     // Stop I2C when suspended

   I2caRegs.I2CFFTX.all = 0x6000;    // Enable FIFO mode and TXFIFO
   I2caRegs.I2CFFRX.all = 0x2040;    // Enable RXFIFO, clear RXFFINT,

   return;
}


/*
	    * Led Packet Structure:
	    * 32 bits of 0's is the start packet
	    * 3 bits of ones, 5 bits for global brightness, then one byte each for color content (RGB)
 */

void spi_xmita(Uint16 a)		//Transmit via SPIA
{
   SpiaRegs.SPITXBUF = a;
}

void spi_xmitb(Uint16 a)		//Transmit via SPIB
{
   SpiaRegs.SPITXBUF = a;
}

void spi_xmitc(Uint16 a)		//Transmit via SPIC
{
   SpiaRegs.SPITXBUF = a;
}

void spi_fifo_init()
{
   SpiaRegs.SPIFFTX.all = 0xE040;	//Initialize SPIA FIFO Registers
   SpiaRegs.SPIFFRX.all = 0x2044;
   SpiaRegs.SPIFFCT.all = 0x0;

   SpibRegs.SPIFFTX.all = 0xE040;	//Initialize SPIB FIFO Registers
   SpibRegs.SPIFFRX.all = 0x2044;
   SpibRegs.SPIFFCT.all = 0x0;

   SpicRegs.SPIFFTX.all = 0xE040;	//Initialize SPIC FIFO Registers
   SpicRegs.SPIFFRX.all = 0x2044;
   SpicRegs.SPIFFCT.all = 0x0;

   InitSpi();	//Initialize Core SPI Registers
}

//
// error - Function to handle errors and halt debugger
//
void error(void)
{
   __asm("     ESTOP0"); // Test failed!! Stop!
    for (;;);
}

//
// mcbsp_init_dlb - Configure McBSP with DLB mode enabled and initialize
//                  requested loopback bit mode based on data size
//
void mcbsp_init_dlb()
{
    McbspaRegs.SPCR2.all=0x0000;        // Reset FS generator, sample rate
                                        // generator & transmitter
    McbspaRegs.SPCR1.all=0x0000;        // Reset Receiver, Right justify word
    McbspaRegs.SPCR1.bit.DLB = 1;       // Enable DLB mode.

    McbspaRegs.MFFINT.all=0x0;          // Disable all interrupts

    McbspaRegs.RCR2.all=0x0;            // Single-phase frame, 1 word/frame,
                                        // No companding (Receive)
    McbspaRegs.RCR1.all=0x0;

    McbspaRegs.XCR2.all=0x0;            // Single-phase frame, 1 word/frame,
                                        // No companding (Transmit)
    McbspaRegs.XCR1.all=0x0;

    McbspaRegs.SRGR2.bit.CLKSM = 1;     // CLKSM=1 (If SCLKME=0, i/p clock to
                                        //          SRG is LSPCLK)
    McbspaRegs.SRGR2.bit.FPER = 31;     // FPER = 32 CLKG periods

    McbspaRegs.SRGR1.bit.FWID = 0;      // Frame Width = 1 CLKG period
    McbspaRegs.SRGR1.bit.CLKGDV = 0;    // CLKG frequency = LSPCLK/(CLKGDV+1)

    McbspaRegs.PCR.bit.FSXM = 1;        // FSX generated internally, FSR
                                        // derived from an external source
    McbspaRegs.PCR.bit.CLKXM = 1;       // CLKX generated internally, CLKR
                                        // derived from an external source

    //
    // Initialize McBSP Data Length
    //
    if(data_size == 8)                  // Run a loopback test in 8-bit mode
    {
      InitMcbspa8bit();
    }
    if(data_size == 16)                 // Run a loopback test in 16-bit mode
    {
      InitMcbspa16bit();
    }
    if(data_size == 32)                 // Run a loopback test in 32-bit mode
    {
      InitMcbspa32bit();
    }

    //
    // Enable Sample rate generator
    //
    McbspaRegs.SPCR2.bit.GRST=1;        // Enable the sample rate generator
    delay_loop();                       // Wait at least 2 SRG clock cycles
    McbspaRegs.SPCR2.bit.XRST=1;        // Release TX from Reset
    McbspaRegs.SPCR1.bit.RRST=1;        // Release RX from Reset
    McbspaRegs.SPCR2.bit.FRST=1;        // Frame Sync Generator reset
}


//
// init_dma - DMA Initialization for data size <= 16-bit
//
void init_dma()
{
  EALLOW;
  DmaRegs.DMACTRL.bit.HARDRESET = 1;
  __asm(" NOP");                        // Only 1 NOP needed per Design

  DmaRegs.CH1.MODE.bit.CHINTE = 0;
  // Channel 1, McBSPA transmit
  DmaRegs.CH1.BURST_SIZE.all = 0;       // 1 word/burst
  DmaRegs.CH1.SRC_BURST_STEP = 0;       // no effect when using 1 word/burst
  DmaRegs.CH1.DST_BURST_STEP = 0;       // no effect when using 1 word/burst
  DmaRegs.CH1.TRANSFER_SIZE = 127;      // Interrupt every frame
                                        // (127 bursts/transfer)
  DmaRegs.CH1.SRC_TRANSFER_STEP = 1;    // Move to next word in buffer after
                                        // each word in a burst
  DmaRegs.CH1.DST_TRANSFER_STEP = 0;    // Don't move destination address
  DmaRegs.CH1.SRC_ADDR_SHADOW = (Uint32) &sdata[0];   // Start address = buffer
  DmaRegs.CH1.SRC_BEG_ADDR_SHADOW = (Uint32) &sdata[0];  // Not needed unless
                                                         // using wrap function
  DmaRegs.CH1.DST_ADDR_SHADOW = (Uint32) &McbspaRegs.DXR1.all; // Start address
                                                               // = McBSPA DXR
  //
  // Not needed unless using wrap function
  //
  DmaRegs.CH1.DST_BEG_ADDR_SHADOW = (Uint32) &McbspaRegs.DXR1.all;
  DmaRegs.CH1.CONTROL.bit.PERINTCLR = 1;   // Clear peripheral interrupt event
                                           // flag.
  DmaRegs.CH1.CONTROL.bit.ERRCLR = 1;      // Clear sync error flag
  DmaRegs.CH1.DST_WRAP_SIZE = 0xFFFF;      // Put to maximum - don't want
                                           // destination wrap.
  DmaRegs.CH1.SRC_WRAP_SIZE = 0xFFFF;      // Put to maximum - don't want
                                           // source wrap.
  DmaRegs.CH1.MODE.bit.CHINTE = 1;         // Enable channel interrupt
  DmaRegs.CH1.MODE.bit.CHINTMODE = 1;      // Interrupt at end of transfer
  DmaRegs.CH1.MODE.bit.PERINTE = 1;        // Enable peripheral interrupt event
  DmaRegs.CH1.MODE.bit.PERINTSEL = 1;      // Peripheral interrupt select =
                                           // McBSP MXSYNCA
  DmaClaSrcSelRegs.DMACHSRCSEL1.bit.CH1 = DMA_MXEVTA;
  DmaRegs.CH1.CONTROL.bit.PERINTCLR = 1;   // Clear any spurious interrupt flags

  //
  // Channel 2, McBSPA Receive
  //
  DmaRegs.CH2.MODE.bit.CHINTE = 0;
  DmaRegs.CH2.BURST_SIZE.all = 0;        // 1 word/burst
  DmaRegs.CH2.SRC_BURST_STEP = 0;        // no effect when using 1 word/burst
  DmaRegs.CH2.DST_BURST_STEP = 0;        // no effect when using 1 word/burst
  DmaRegs.CH2.TRANSFER_SIZE = 127;       // Interrupt every 127 bursts/transfer
  DmaRegs.CH2.SRC_TRANSFER_STEP = 0;     // Don't move source address
  DmaRegs.CH2.DST_TRANSFER_STEP = 1;     // Move to next word in buffer after
                                         // each word in a burst
  DmaRegs.CH2.SRC_ADDR_SHADOW = (Uint32) &McbspaRegs.DRR1.all; // Start address
                                                               // = McBSPA DRR
  //
  // Not needed unless using wrap function
  //
  DmaRegs.CH2.SRC_BEG_ADDR_SHADOW = (Uint32) &McbspaRegs.DRR1.all;
  DmaRegs.CH2.DST_ADDR_SHADOW = (Uint32) &rdata[0];      // Start address =
                                                         // Receive buffer
                                                         // (for McBSP-A)
  DmaRegs.CH2.DST_BEG_ADDR_SHADOW = (Uint32) &rdata[0];  // Not needed unless
                                                         // using wrap function
  DmaRegs.CH2.CONTROL.bit.PERINTCLR = 1; // Clear peripheral interrupt event
                                         // flag.
  DmaRegs.CH2.CONTROL.bit.ERRCLR = 1;    // Clear sync error flag
  DmaRegs.CH2.DST_WRAP_SIZE = 0xFFFF;    // Put to maximum - don't want
                                         // destination wrap.
  DmaRegs.CH2.SRC_WRAP_SIZE = 0xFFFF;    // Put to maximum - don't want
                                         // source wrap.
  DmaRegs.CH2.MODE.bit.CHINTE = 1;       // Enable channel interrupt
  DmaRegs.CH2.MODE.bit.CHINTMODE = 1;    // Interrupt at end of transfer
  DmaRegs.CH2.MODE.bit.PERINTE = 1;      // Enable peripheral interrupt event
  DmaRegs.CH2.MODE.bit.PERINTSEL = 2;    // Peripheral interrupt select =
                                         // McBSP MRSYNCA
  DmaClaSrcSelRegs.DMACHSRCSEL1.bit.CH2 = DMA_MREVTA;
  DmaRegs.CH2.CONTROL.bit.PERINTCLR = 1; // Clear any spurious interrupt flags
  EDIS;
}

//
// init_dma_32 - DMA Initialization for data size > 16-bit and <= 32-bit.
//
void init_dma_32()
{
  EALLOW;
  DmaRegs.DMACTRL.bit.HARDRESET = 1;
  __asm(" NOP");                        // Only 1 NOP needed per Design

  // Channel 1, McBSPA transmit
  DmaRegs.CH1.MODE.bit.PERINTSEL = DMA_MXEVTA;
  DmaRegs.CH1.BURST_SIZE.all = 1;       // 2 word/burst
  DmaRegs.CH1.SRC_BURST_STEP = 1;       // increment 1 16-bit addr. btwn words
  DmaRegs.CH1.DST_BURST_STEP = 1;       // increment 1 16-bit addr. btwn words
  DmaRegs.CH1.TRANSFER_SIZE = 63;       // Interrupt every 63 bursts/transfer
  DmaRegs.CH1.SRC_TRANSFER_STEP = 1;    // Move to next word in buffer after
                                        // each word in a burst.
  DmaRegs.CH1.DST_TRANSFER_STEP = 0xFFFF;    // Go back to DXR2
  DmaRegs.CH1.SRC_ADDR_SHADOW = (Uint32) &sdata[0];  // Start address = buffer
  DmaRegs.CH1.SRC_BEG_ADDR_SHADOW = (Uint32) &sdata[0]; // Not needed unless
                                                        // using wrap function.
  DmaRegs.CH1.DST_ADDR_SHADOW = (Uint32) &McbspaRegs.DXR2.all; // Start address
                                                               // = McBSPA DXR2

  //
  // Not needed unless using wrap function
  //
  DmaRegs.CH1.DST_BEG_ADDR_SHADOW = (Uint32) &McbspaRegs.DXR2.all;
  DmaRegs.CH1.CONTROL.bit.ERRCLR = 1;   // Clear sync error flag
  DmaRegs.CH1.DST_WRAP_SIZE = 0xFFFF;   // Put to maximum - don't want
                                        // destination wrap
  DmaRegs.CH1.SRC_WRAP_SIZE = 0xFFFF;   // Put to maximum - don't want
                                        // source wrap
  DmaRegs.CH1.MODE.bit.CHINTE = 1;      // Enable channel interrupt
  DmaRegs.CH1.MODE.bit.CHINTMODE = 1;   // Interrupt at end of transfer
  DmaRegs.CH1.MODE.bit.PERINTE = 1;     // Enable peripheral interrupt event
  DmaRegs.CH1.MODE.bit.PERINTSEL = 1;   // Peripheral interrupt select = McBSP
                                        // MXSYNCA
  DmaClaSrcSelRegs.DMACHSRCSEL1.bit.CH1 = DMA_MXEVTA;
  DmaRegs.CH1.CONTROL.bit.PERINTCLR = 1;  // Clear any spurious interrupt flags

  //
  // Channel 2, McBSPA Receive
  //
  DmaRegs.CH2.BURST_SIZE.all = 1;       // 2 words/burst
  DmaRegs.CH2.SRC_BURST_STEP = 1;       // Increment 1 16-bit addr. btwn words
  DmaRegs.CH2.DST_BURST_STEP = 1;       // Increment 1 16-bit addr. btwn words
  DmaRegs.CH2.TRANSFER_SIZE = 63;       // Interrupt every 63 bursts/transfer
  DmaRegs.CH2.SRC_TRANSFER_STEP = 0xFFFF;  // Decrement  back to DRR2
  DmaRegs.CH2.DST_TRANSFER_STEP = 1;       // Move to next word in buffer after
                                           // each word in a burst
  DmaRegs.CH2.SRC_ADDR_SHADOW = (Uint32) &McbspaRegs.DRR2.all; // Start address
                                                               // = McBSPA DRR
  //
  // Not needed unless using wrap function
  //
  DmaRegs.CH2.SRC_BEG_ADDR_SHADOW = (Uint32) &McbspaRegs.DRR2.all;
  DmaRegs.CH2.DST_ADDR_SHADOW = (Uint32) &rdata[0];  // Start address =
                                                     // Receive buffer(McBSP-A)
  //
  // Not needed unless using wrap function
  //
  DmaRegs.CH2.DST_BEG_ADDR_SHADOW = (Uint32) &rdata[0];
  DmaRegs.CH2.CONTROL.bit.ERRCLR = 1;    // Clear sync error flag
  DmaRegs.CH2.DST_WRAP_SIZE = 0xFFFF;    // Put to maximum - don't want
                                         // destination wrap
  DmaRegs.CH2.SRC_WRAP_SIZE = 0xFFFF;    // Put to maximum - don't want
                                         // source wrap
  DmaRegs.CH2.MODE.bit.CHINTE = 1;       // Enable channel interrupt
  DmaRegs.CH2.MODE.bit.CHINTMODE = 1;    // Interrupt at end of transfer
  DmaRegs.CH2.MODE.bit.PERINTE = 1;      // Enable peripheral interrupt event
  DmaRegs.CH2.MODE.bit.PERINTSEL = 2;    // Peripheral interrupt select =
                                         // McBSP MRSYNCA
  DmaClaSrcSelRegs.DMACHSRCSEL1.bit.CH2 = DMA_MREVTA;
  DmaRegs.CH2.CONTROL.bit.PERINTCLR = 1;  // Clear any spurious interrupt flags
  EDIS;
}

//
// start_dma - Start the DMA channels' 1 and 2
//
void start_dma (void)
{
  EALLOW;
  DmaRegs.CH1.CONTROL.bit.RUN = 1;      // Start DMA Transmit from McBSP-A
  DmaRegs.CH2.CONTROL.bit.RUN = 1;      // Start DMA Receive from McBSP-A
  EDIS;
}

//
// local_D_INTCH1_ISR - DMA ISR for channel 1 (INT7.1)
//
__interrupt void local_D_INTCH1_ISR(void)
{
    EALLOW;      // NEED TO EXECUTE EALLOW INSIDE ISR !!!
    DmaRegs.CH1.CONTROL.bit.HALT = 1;

    //
    // To receive more interrupts from this PIE group, acknowledge
    // this interrupt
    //
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP7;
    EDIS;
    return;
}

//
// local_D_INTCH2_ISR - DMA ISR for channel 2 (INT7.2)
//
__interrupt void local_D_INTCH2_ISR(void)
{
    Uint16 i;
    EALLOW;      // NEED TO EXECUTE EALLOW INSIDE ISR !!!
    DmaRegs.CH2.CONTROL.bit.HALT = 1;

    //
    // To receive more interrupts from this PIE group, acknowledge this
    // interrupt
    //
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP7;
    for (i=0; i<128; i++)
    {
        if(data_size == 8)
        {
            if((rdata[i]&0x00FF) != (sdata[i]&0x00FF))
            {
              error();  // STOP if there is an error !!
            }
        }
        else if(data_size == 16)
        {
            if (rdata[i] != sdata[i])
            {
              error();  // STOP if there is an error !!
            }
        }
        else if(data_size == 32)
        {
            if ((rdata[i])!=(sdata[i]))
            {
              error ();
            }
        }
   }
   EDIS;

   return;
}


