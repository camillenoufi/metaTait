#include "F28x_Project.h"

#define NumLeds 5		//Number of Leds we want to blink

#define Blue	0		//For all Blue Leds
#define Red 	0		//For all Red Leds      Only have one of these be 1 at any given time
#define Green 	0		//For all Green Leds
#define Purple  1		//For all Purple Leds

#define noblink 	0	//If 1, it will not blink, 0 it will blink off and on

void spi_xmit(Uint16 a);		//16 bit transmit
void spi_fifo_init(void);		//Initialize FIFO Buffers
void spi_init(void);			//Initialize SPI



void main(void)
{
   Uint16 stframe1 = 0x0000;  	// start of send data cycle
   Uint16 stframe2 = 0x0000;

   Uint16 LedOff1 = 0xE000;		//Black Led Color, Leds are turned off
   Uint16 LedOff2 = 0x0000;

   Uint16 Ledframe1;			//Houses led color data
   Uint16 Ledframe2;

   Uint16 endframe1 = 0xFFFF;	// end of send data cycle

#if Blue
   Ledframe1 = 0xE5FF;			//Solid Blue Led Color
   Ledframe2 = 0x0000;
#endif

#if Red
   Ledframe1 = 0xE500;			//Solid Red Led Color
   Ledframe2 = 0x00FF;
#endif

#if Green
   Ledframe1 = 0xE500;			//Solid Green Led Color
   Ledframe2 = 0xFF00;
#endif

#if  Purple
   Ledframe1 = 0xE5FF;			//Solid Purple Led Color
   Ledframe2 = 0x00FF;
#endif

   InitSysCtrl();				//Initialize System Control, Where you can change PLL system clock
   InitSpiaGpio();				//Initialize GPIO			Where you can change GPIO pins for SPI Communication
   DINT;						//Clear all interrupts
   InitPieCtrl();
   IER = 0x0000;				//Disable CPU interrupts
   IFR = 0x0000;				//Clear interrupt flags
   InitPieVectTable();
   spi_fifo_init();     		// Initialize the SPI FIFO, Possible Issue!!!
   int j= 0;					//Variable that will control loops for sending out data to Leds
   /*
    * Led Packet Structure:
    * 32 bits of 0's is the start packet
    * 3 bits of ones, 5 bits for global brightness, then one byte each for color content (RGB)
    * n/2 bits of 1's is the end packet.
    */
#if noblink
   while(1)					//Loop here forever so Leds continue to blink.
   {
	   j= 0;
	   spi_xmit(stframe1);						//First 16 bits of Start Frame for Dotstars
	   spi_xmit(stframe2);						//Last 16 bits of Start Frame for Dotstars
	   while(j < NumLeds)						//Send the color out to NumLeds
	   {
		   spi_xmit(Ledframe1);					//Chose Color, 1st 16 bits of Led frame
		   spi_xmit(Ledframe2);					//Color, 2nd 16 bits of Led frame
		   j++;
	   }
	   for(j= 0; (j < ((NumLeds+31)/32)); j++)	//End frame for Led Packet.
	   {
		   spi_xmit(endframe1);
	   }
   }
}

#else

   while(1)					//Loop here forever so Leds continue to blink.
   {
	   spi_xmit(stframe1);						//First 16 bits of Start Frame for Dotstars
	   spi_xmit(stframe2);						//Last 16 bits of Start Frame for Dotstars
	   while(j < NumLeds)						//Send the color out to NumLeds
	   {
		   spi_xmit(Ledframe1);					//Chose Color, 1st 16 bits of Led frame
		   spi_xmit(Ledframe2);					//Color, 2nd 16 bits of Led frame
		   j++;
	   }
	   for(j= 0; (j < ((NumLeds+31)/32)); j++)	//End frame for Led Packet.
	   {
		   spi_xmit(endframe1);
	   }
	   DELAY_US(500000);						//Add a delay for visibility

	   j = 0;
	   spi_xmit(stframe1);
	   spi_xmit(stframe2);
	   while(j < NumLeds)
	   {
		   spi_xmit(LedOff1);		//1st 16 bits of turn off all Leds
		   spi_xmit(LedOff2);		//2nd 16 bits of turn off all Leds
		   j++;
	   }
	   for(j= 0; (j < ((NumLeds+31)/32)); j++)
	   {
		   spi_xmit(endframe1);
	   }
	  DELAY_US(500000);
	   j= 0;
   }
}
#endif


   void spi_xmit(Uint16 a)		//Transmit via SPI
{
    SpiaRegs.SPITXBUF = a;
}

void spi_fifo_init() 		//Most likely a problem!!!
{
    SpiaRegs.SPIFFTX.all = 0xE040;	//Initialize SPI FIFO Registers
    SpiaRegs.SPIFFRX.all = 0x2044;
    SpiaRegs.SPIFFCT.all = 0x0;

    InitSpi();	//Initialize Core SPI Registers
}

