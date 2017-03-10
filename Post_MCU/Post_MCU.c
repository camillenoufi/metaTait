#include "F28x_Project.h"
#include "F2837xS_device.h"
#include "F2837xS_Examples.h"

// hhihih
// Defines
//

// Choose a word size for MCBSP DMA.  Uncomment one of the following lines
//
//#define WORD_SIZE  8      // Run a loopback test in 8-bit mode
#define WORD_SIZE 16      // Run a loopback test in 16-bit mode
//#define WORD_SIZE 32      // Run a loopback test in 32-bit mode

// Values for I2C Communication
#define I2C_SLAVE_ADDR        0x50
#define I2C_NUMBYTES          2
#define I2C_EEPROM_HIGH_ADDR  0x00
#define I2C_EEPROM_LOW_ADDR   0x30
#define Post1_SLAVE_ADDR      0x25
#define Post2_SLAVE_ADDR      0x35
#define Post3_SLAVE_ADDR      0x45

//Values for LED Data Parsing
#define StreamSize           171072     //LED data stream size in bytes
#define ChunkSizeI            1584      // Size of a single chunk pulled from FRAM in 16 bit words
#define ChunkSizeB            3168      // Size of a single chunk pulled from FRAM in bytes

//Values for McBSP Communication
#define CLKGDV_VAL   1

#define CPU_SPD              200E6
#define MCBSP_SRG_FREQ       CPU_SPD/4   // SRG input is LSPCLK (SYSCLKOUT/4)
                                         // for examples

// # of CPU cycles in 2 CLKG cycles-init delay
#define MCBSP_CLKG_DELAY     2*(CPU_SPD/(MCBSP_SRG_FREQ/(1+CLKGDV_VAL)))


//
// Globals
//

#pragma DATA_SECTION(ChunkDMA, "ramgs0") //Place LED Chunkdata in DMA-accessible RAM
#pragma DATA_SECTION(ChunkDMAT, "ramgs1")
#pragma DATA_SECTION(ChunkTMP, "ramgs2")



Uint16 ChunkDMA[ChunkSizeI];                // data array which DMA stores FRAM data
Uint16 ChunkTMP[ChunkSizeB];             //data array which is used to send data out to strips, ChunkDMA will be copied into ChunkTMP
Uint16 ChunkDMAT[ChunkSizeI];            //array used to test MCBSP DMA code
unsigned long chunk1Addr;            //address of the first third of data in FRAM
unsigned long chunk2Addr;            //address of the first third of data in FRAM
unsigned long chunk3Addr;            //address of the

uint16_t globalBrt = 0;                       // Global brightness value received from Base, user programmed
uint16_t Refresh = 1000;                  //Refresh rate value sent from Base used to calculate timer interrupt value
uint16_t stframe = 0x0000;  	// 32 0 bits signal start of each LED frame, each stframe is 16 bits, must send 2
uint16_t startBlue = 0;        //First 16 bits of an LED frame
uint16_t greenRed = 0;            //Second 16 bits of an LED frame
uint16_t L1L2[264];
uint16_t L3L4[264];
uint16_t L5L6[264];


uint16_t rgbDim = 3;		//hardcode? for now.
uint16_t stripsPerPort = 2;
uint16_t numPorts = 3;
uint16_t numPosts = 3;
uint16_t LEDupdates = 216;
uint16_t updatesPerChunk = 8;  //currently 8
uint16_t  height = 44*2;
uint16_t pixelStart =0;
uint16_t DMAupdates =0;


uint16_t updateLength = 44*3*3*2;  //total number of bytes sent out per update

uint16_t u = 0;

/*
struct I2CMSG I2CRMSG={ I2C_MSGSTAT_SEND_WITHSTOP,
                           I2C_SLAVE_ADDR,
                           I2C_NUMBYTES,
                           I2C_EEPROM_HIGH_ADDR,
                           I2C_EEPROM_LOW_ADDR,
                              };


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
*/

//
// Function Prototypes
//

//
//CPU Timer ISR
__interrupt void cpu_timer0_isr(void);

// I2C Prototypes
void   I2CA_Init(void);
Uint16 I2CA_WriteData(struct I2CMSG *msg);
Uint16 I2CA_ReadData(struct I2CMSG *msg);
void I2CA_SlaveRead(struct I2CMSG*msg);
__interrupt void i2c_int1a_isr(void);
void pass(void);
void fail(void);

//MCBSP DMA Prototypes
__interrupt void local_D_INTCH1_ISR(void);
__interrupt void local_D_INTCH2_ISR(void);
void mcbsp_init_dlb(void);
void init_mcbsp_spi(void);
void mcbsp_xmit(Uint16 a);
void init_dma(void);
void init_dma_32(void);
void start_dma(void);
void error(void);


//High Speed SPI Prototypes
void spi_xmita(Uint16 a);		//16 bit transmit out of SPI Port A
void spi_xmitb(Uint16 a);		//16 bit transmit out of SPI Port B
void spi_xmitc(Uint16 a);		//16 bit transmit out of SPI Port C
void spi_fifo_init(void);		//Initialize FIFO Buffers

//LED Data Handling Prototypes
void getLEDChunk(Uint16 startL1L2, Uint16 endL1L2,Uint16 startL3L4, Uint16 endL3L4,Uint16 startL5L6, Uint16 endL5L6, Uint16* data);
void Runtime();
void switchDataChunk(int u);




int main(void)
{
DMAupdates = StreamSize/ChunkSizeI;
	//
	// Initialize System Control:
	// PLL, WatchDog, enable Peripheral Clocks
	// This example function is found in the F2837xS_SysCtrl.c file.
	//

	 InitSysCtrl();				//Initialize System Control, Where you can change PLL system clock

	 InitGpio();

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

	 GPIO_SetupPinMux(65, GPIO_MUX_CPU1, 0);     // For timer interrupt
	 GPIO_SetupPinOptions(65, GPIO_OUTPUT, GPIO_PUSHPULL); // For timer interrupt

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
	    PieVectTable.TIMER0_INT = &cpu_timer0_isr;
	    PieVectTable.I2CA_INT = &i2c_int1a_isr;
	    PieVectTable.DMA_CH1_INT= &local_D_INTCH1_ISR;
	    PieVectTable.DMA_CH2_INT= &local_D_INTCH2_ISR;
	    EDIS;      // This is needed to disable write to EALLOW protected registers


	    //
	    // Initialize I2CA
	    //
	   //    I2CA_Init();

	    //
	    // Ensure DMA is connected to PF2SEL bridge (EALLOW protected)
	    //
	       EALLOW;
	       CpuSysRegs.SECMSEL.bit.PF2SEL = 1;
	       EDIS;

	       //For testing McBSP DMA writing and reading
	       int i;
	       int k = 0;
	       for (i=0;i<ChunkSizeI;i++)
	       {
	    	   ChunkDMAT[i] = 0x22FF;
	    	   ChunkDMA[i]=0;

	       }

	       init_dma();        // 1. When using DMA, initialize DMA with
	      	                                //    peripheral interrupts first.

	      	         start_dma();       // not sure if start DMA should go first
	      	          //mcbsp_init_dlb();      // 2. Then initialize and release peripheral
	      	         init_mcbsp_spi();      //   (McBSP) from Reset.

	      	         //
	      	         // Enable interrupts required for this example
	      	         //
	      	            PieCtrlRegs.PIECTRL.bit.ENPIE = 1;   // Enable the PIE block
	      	            PieCtrlRegs.PIEIER7.bit.INTx1 = 1;   // Enable PIE Group 7, INT 1 (DMA CH1)
	      	            PieCtrlRegs.PIEIER7.bit.INTx2 = 1;   // Enable PIE Group 7, INT 2 (DMA CH2)

	      	            IER=0x40;                            // Enable CPU INT groups 6 and 7
	      	            EINT;                                // Enable Global Interrupts

	      Runtime();

	       while(1)
	       {
	    	  // switchDataChunk(u);
	    	   //DELAY_US(100);
	       }
/*
	       //
	       //I2C code

	       //
	       // Clear incoming message buffer
	       //
	       	   int i;
	          for (i = 0; i < I2C_MAX_BUFFER_SIZE; i++)
	          {
	              I2cMsgIn1.MsgBuffer[i] = 0x0000;
	          }
	       //
	       // Enable I2C __interrupt 1 in the PIE: Group 8 __interrupt 1
	       //
	          PieCtrlRegs.PIEIER8.bit.INTx1 = 1;

	          //
	          // Enable CPU INT8 which is connected to PIE group 8
	          //
	             IER |= M_INT8;
	             EINT;

	    //         Uint16 Error;
	             CurrentMsgPtr = &I2cMsgOut1;
	             */




	        	// I2C Slave Code


	        	//I2C Master Code
	/*        	//
	        	      // Write data to EEPROM section
	        	      //

	        	      //
	        	      // Check the outgoing message to see if it should be sent.
	        	      // In this example it is initialized to send with a stop bit.
	        	      //
	        	      if(I2cMsgOut1.MsgStatus == I2C_MSGSTAT_SEND_WITHSTOP)
	        	      {
	        	         Error = I2CA_WriteData(&I2cMsgOut1);

	        	         //
	        	         // If communication is correctly initiated, set msg status to busy
	        	         // and update CurrentMsgPtr for the __interrupt service routine.
	        	         // Otherwise, do nothing and try again next loop. Once message is
	        	         // initiated, the I2C __interrupts will handle the rest. Search for
	        	         // ICINTR1A_ISR in the i2c_eeprom_isr.c file.
	        	         //
	        	         if (Error == I2C_SUCCESS)
	        	         {
	        	            CurrentMsgPtr = &I2cMsgOut1;
	        	            I2cMsgOut1.MsgStatus = I2C_MSGSTAT_WRITE_BUSY;
	        	         }
	        	      }

	        	      //
	        	      // Read data from EEPROM section
	        	      //

	        	      //
	        	      // Check outgoing message status. Bypass read section if status is
	        	      // not inactive.
	        	      //
	        	      if (I2cMsgOut1.MsgStatus == I2C_MSGSTAT_INACTIVE)
	        	      {
	        	         //
	        	         // Check incoming message status.
	        	         //
	        	         if(I2cMsgIn1.MsgStatus == I2C_MSGSTAT_SEND_NOSTOP)
	        	         {
	        	            //
	        	            // EEPROM address setup portion
	        	            //
	        	            while(I2CA_ReadData(&I2cMsgIn1) != I2C_SUCCESS)
	        	            {
	        	               //
	        	               // Maybe setup an attempt counter to break an infinite while
	        	               // loop. The EEPROM will send back a NACK while it is performing
	        	               // a write operation. Even though the write communique is
	        	               // complete at this point, the EEPROM could still be busy
	        	               // programming the data. Therefore, multiple attempts are
	        	               // necessary.
	        	               //
	        	            }
	        	            //
	        	            // Update current message pointer and message status
	        	            //
	        	            CurrentMsgPtr = &I2cMsgIn1;
	        	            I2cMsgIn1.MsgStatus = I2C_MSGSTAT_SEND_NOSTOP_BUSY;
	        	         }
	        	         //
	        	         // Once message has progressed past setting up the internal address
	        	         // of the EEPROM, send a restart to read the data bytes from the
	        	         // EEPROM. Complete the communique with a stop bit. MsgStatus is
	        	         // updated in the __interrupt service routine.
	        	         //
	        	         else if(I2cMsgIn1.MsgStatus == I2C_MSGSTAT_RESTART)
	        	         {
	        	            //
	        	            // Read data portion
	        	            //
	        	            while(I2CA_ReadData(&I2cMsgIn1) != I2C_SUCCESS)
	        	            {
	        	               //
	        	               // Maybe setup an attempt counter to break an infinite while
	        	               // loop.
	        	               //
	        	            }
	        	            //
	        	            // Update current message pointer and message status
	        	            //
	        	            CurrentMsgPtr = &I2cMsgIn1;
	        	            I2cMsgIn1.MsgStatus = I2C_MSGSTAT_READ_BUSY;
	        	         }
	        	      }
*/



}

void Runtime() {

	//run initial DMA transfer

	pixelStart = 0b11100000 | globalBrt;   //define start byte for each LED frame

	//use Refresh value to configure timer interrupt
	ConfigCpuTimer(&CpuTimer0, 200, Refresh);
	CpuTimer0Regs.TCR.all = 0x4000;// set up timer interrupt

	IER |= M_INT1;  //Enable timer0 interrupt

	//
	// Enable TINT0 in the PIE: Group 1 interrupt 7
	//
	    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;

	    EINT;  // Enable Global interrupt INTM
	    ERTM;  // Enable Global realtime interrupt DBGM

	while(u<LEDupdates) {

		if (u==0||(u+1) % updatesPerChunk == 0)
			// get new chunk and initiaite DMA transfer
			 switchDataChunk(u);  // may have to include specific size of chunk



	}

}




void switchDataChunk(int u)
{
				  //mcbsp_xmit to FRAM to determine which part of FRAM to pull from
	start_dma();  //pull chunk of data from FRAM into ChunkDMA
	DELAY_US(30);
	int i = -1;
	int k = 0;

	for (i=0;i<ChunkSizeI;i++)
	{
	    ChunkTMP[k] = chunkDMA[i] & 0x00FF;      //mask out lower 8-bits
        ChunkTMP[k+1] = chunkDMA[i] & 0xFF00;    //mask out upper 8-bits
        k = k+2;
	}



}

void getLEDChunk(Uint16 startL1L2, Uint16 endL1L2, Uint16 startL3L4, Uint16 endL3L4, Uint16 startL5L6, Uint16 endL5L6, Uint16* data)
{

	//char stripData[264];   //initialize array to hold LED strip data
	int i = 0;
	int k = 0;

	for (i=startL1L2; i<=endL1L2; i++)
	{
		L1L2[k] = data[i];
		k++;
	}
	k=0;
	for (i=startL3L4; i<=endL3L4; i++)
	{
			L3L4[k] = data[i];
			k++;
		}
	k=0;
	for (i=startL5L6; i<=endL5L6; i++)
	{
			L5L6[k] = data[i];
			k++;
		}
	k=0;

}

//
// cpu_timer0_isr - CPU Timer0 ISR with interrupt counter
//
__interrupt void cpu_timer0_isr(void)
{
   CpuTimer0.InterruptCount++;
	// Get
    //
    //
	Uint16 i;


	uint16_t index = updateLength*(u%updatesPerChunk);
	uint16_t startL1L2 = index;
	uint16_t endL1L2 = index+height*rgbDim-1;
	uint16_t startL3L4 = index + height*rgbDim;
	uint16_t endL3L4 = index + 2*height*rgbDim - 1;
	uint16_t startL5L6 = index + 2*height*rgbDim;
	uint16_t endL5L6 = index+3*height*rgbDim - 1;


		// extract corresponding data for LED strip pairs
		getLEDChunk(startL1L2, endL1L2,startL3L4, endL3L4, startL5L6, endL5L6, ChunkTMP);

		// transmit Strip Start Packets (32-bits)
	    spi_xmita(stframe);
	    spi_xmita(stframe);
	 	 DELAY_US(3);

	   	spi_xmitb(stframe);
	   	spi_xmitb(stframe);
	    DELAY_US(3);

	   	spi_xmitc(stframe);
	   	spi_xmitc(stframe);
	   	DELAY_US(3);

		// transmit pixel data
		for(i=0; i<height*rgbDim; i+=rgbDim)
		{
			startBlue = ( pixelStart << 8 ) | L1L2[i];    //pixel start (MSB) and blue byte (LSB)
			greenRed = ( L1L2[i+1] << 8 ) | L1L2[i+2];  //green byte (MSB) and red byte (LSB)
			spi_xmita(startBlue);
	    	spi_xmita(greenRed);
	    	DELAY_US(3);

			startBlue = ( pixelStart << 8 ) | L3L4[i];    //pixel start (MSB) and blue byte (LSB)
		    greenRed = ( L3L4[i+1] << 8 ) | L3L4[i+2]; //green byte (MSB) and red byte (LSB)
			spi_xmitb(startBlue);
			spi_xmitb(greenRed);
			DELAY_US(3);

			Uint16 startBlue = ( pixelStart << 8 ) | L5L6[i];    //pixel start (MSB) and blue byte (LSB)
			Uint16 greenRed = ( L5L6[i+1] << 8 ) | L5L6[i+2];  //green byte (MSB) and red byte (LSB)
			spi_xmitc(startBlue);
			spi_xmitc(greenRed);
			DELAY_US(3);
		}

		u=(u+1)%LEDupdates;
	////  END STUFF IN ISR

   //
   // Acknowledge this interrupt to receive more interrupts from group 1
   //
   PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

//
// I2CA_Init - Initialize I2CA settings
//
void I2CA_Init(void)
{
	// I2C Slave code
	I2caRegs.I2COAR.all = Post1_SLAVE_ADDR;
	I2caRegs.I2CPSC.all = 6;          // Prescaler - need 7-12 Mhz on module clk
	I2caRegs.I2CCLKL = 10;            // NOTE: must be non zero
	I2caRegs.I2CCLKH = 5;             // NOTE: must be non zero
	I2caRegs.I2CIER.all = 0x24;       // Enable SCD & ARDY __interrupts
	I2caRegs.I2CMDR.all = 0x0020;     // Take I2C out of reset
	                                  // Stop I2C when suspended
	   	   	   	   	   	   	   	   	  //
	I2caRegs.I2CFFTX.all = 0x6000;    // Enable FIFO mode and TXFIFO
	I2caRegs.I2CFFRX.all = 0x2040;    // Enable RXFIFO, clear RXFFINT,

// I2C Master code
/*
   I2caRegs.I2CSAR.all = 0x0050;     // Slave address - EEPROM control code

   I2caRegs.I2CPSC.all = 6;          // Prescaler - need 7-12 Mhz on module clk
   I2caRegs.I2CCLKL = 10;            // NOTE: must be non zero
   I2caRegs.I2CCLKH = 5;             // NOTE: must be non zero
   I2caRegs.I2CIER.all = 0x24;       // Enable SCD & ARDY __interrupts

   I2caRegs.I2CMDR.all = 0x0060;     // Take I2C out of reset
                                     // Stop I2C when suspended
   	   	   	   	   	   	   	   	   	 // Enable DLB mode

   I2caRegs.I2CFFTX.all = 0x6000;    // Enable FIFO mode and TXFIFO
   I2caRegs.I2CFFRX.all = 0x2040;    // Enable RXFIFO, clear RXFFINT,
*/
   return;
}


//
// I2CA_WriteData - Transmit I2CA message
//
/*
Uint16 I2CA_WriteData(struct I2CMSG *msg)
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
Uint16 I2CA_ReadData(struct I2CMSG *msg)
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
      I2caRegs.I2CMDR.all = 0x2660; // Send data to setup EEPROM address
   }
   else if(msg->MsgStatus == I2C_MSGSTAT_RESTART)
   {
      I2caRegs.I2CCNT = msg->NumOfBytes;    // Setup how many bytes to expect
      I2caRegs.I2CMDR.all = 0x2C60;         // Send restart as master receiver
   }

   return I2C_SUCCESS;
}
*/
//
// i2c_int1a_isr - I2CA ISR
//
__interrupt void i2c_int1a_isr(void)
{
  /* Uint16 IntSource, i;

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

            for(i=0; i < I2C_NUMBYTES; i++)
             {
              CurrentMsgPtr->MsgBuffer[i] = I2caRegs.I2CDRR.all;
             }

            for(i=0; i < I2C_NUMBYTES; i++)
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
   */
}

//
// pass - Halt debugger and signify pass
//
void pass()
{
   asm("   ESTOP0");
   for(;;);
}

//
// fail - Halt debugger and signify fail
//
void fail()
{
   asm("   ESTOP0");
   for(;;);
}



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
// mcbsp_xmit - Transmit MCBSP data
//
void mcbsp_xmit(Uint16 a)
{
    McbspaRegs.DXR1.all = a;
}

//
// init_mcbsp_spi - Configure McBSP settings
//
void init_mcbsp_spi()
{
    //
    // McBSP-A register settings
    //
    McbspaRegs.SPCR2.all = 0x0000;       // Reset FS generator, sample rate
                                         // generator & transmitter
    McbspaRegs.SPCR1.all = 0x0000;       // Reset Receiver, Right justify word,

    McbspaRegs.PCR.all = 0x0F08;         //(CLKXM=CLKRM=FSXM=FSRM= 1, FSXP = 1)
    McbspaRegs.SPCR1.bit.DLB = 1;        // DLB mode for testing
    McbspaRegs.SPCR1.bit.CLKSTP = 2;     // Together with CLKXP/CLKRP
                                         // determines clocking scheme
    McbspaRegs.PCR.bit.CLKXP = 0;        // CPOL = 0, CPHA = 0 rising edge
                                         // no delay
    McbspaRegs.PCR.bit.CLKRP = 0;
    McbspaRegs.RCR2.bit.RDATDLY = 01;    // FSX setup time 1 in master mode.
                                         // 0 for slave mode (Receive)
    McbspaRegs.XCR2.bit.XDATDLY = 01;    // FSX setup time 1 in master mode.
                                         // 0 for slave mode (Transmit)

    McbspaRegs.RCR1.bit.RWDLEN1 = 2;     //16-bit word
    McbspaRegs.XCR1.bit.XWDLEN1 = 2;     //16-bit word

    McbspaRegs.SRGR2.all = 0x2000;       // CLKSM=1, FPER = 1 CLKG periods
    McbspaRegs.SRGR1.all = 0x000F;       // Frame Width = 1 CLKG period,
                                         // CLKGDV=16

    McbspaRegs.SPCR2.bit.GRST = 1;       // Enable the sample rate generator
    delay_loop();                        // Wait at least 2 SRG clock cycles
    McbspaRegs.SPCR2.bit.XRST = 1;       // Release TX from Reset
    McbspaRegs.SPCR1.bit.RRST = 1;       // Release RX from Reset
    McbspaRegs.SPCR2.bit.FRST = 1;       // Frame Sync Generator reset
}





//
// mcbsp_init_dlb - Configure McBSP with DLB mode enabled and initialize
//                  requested loopback bit mode based on data size
//
/*
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

*/

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
  DmaRegs.CH1.TRANSFER_SIZE = 3167;      // Interrupt every frame
                                        // (3167 bursts/transfer)
  DmaRegs.CH1.SRC_TRANSFER_STEP = 1;    // Move to next word in buffer after
                                        // each word in a burst
  DmaRegs.CH1.DST_TRANSFER_STEP = 0;    // Don't move destination address
  DmaRegs.CH1.SRC_ADDR_SHADOW = (Uint32) &ChunkDMAT[0];   // Start address = buffer
  DmaRegs.CH1.SRC_BEG_ADDR_SHADOW = (Uint32) &ChunkDMAT[0];  // Not needed unless
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
  DmaRegs.CH2.TRANSFER_SIZE = 3167;       // Interrupt every 3167 bursts/transfer
  DmaRegs.CH2.SRC_TRANSFER_STEP = 0;     // Don't move source address
  DmaRegs.CH2.DST_TRANSFER_STEP = 1;     // Move to next word in buffer after
                                         // each word in a burst
  DmaRegs.CH2.SRC_ADDR_SHADOW = (Uint32) &McbspaRegs.DRR1.all; // Start address
                                                               // = McBSPA DRR
  //
  // Not needed unless using wrap function
  //
  DmaRegs.CH2.SRC_BEG_ADDR_SHADOW = (Uint32) &McbspaRegs.DRR1.all;
  DmaRegs.CH2.DST_ADDR_SHADOW = (Uint32) &ChunkDMA[0];      // Start address =
                                                         // Receive buffer
                                                         // (for McBSP-A)
  DmaRegs.CH2.DST_BEG_ADDR_SHADOW = (Uint32) &ChunkDMA[0];  // Not needed unless
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

    EALLOW;      // NEED TO EXECUTE EALLOW INSIDE ISR !!!
    DmaRegs.CH2.CONTROL.bit.HALT = 1;

    //
    // To receive more interrupts from this PIE group, acknowledge this
    // interrupt
    //
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP7;
    /*
     * Uint16 i;
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
   */
   EDIS;

   return;

}




