#include <metaTait_HighLevel.h>
#include "F28x_Project.h"
#include "metaTait_I2C.h"
#include "metaTait_SDCard.h"



void InitSpi(void)
{
    // Initialize SPI-A

    // Set reset low before configuration changes
    // Clock polarity (0 == rising, 1 == falling)
    // 16-bit character
    // Enable loop-back
    SpiaRegs.SPICCR.bit.SPISWRESET = 0;
    SpiaRegs.SPICCR.bit.CLKPOLARITY = 0;
    SpiaRegs.SPICCR.bit.SPICHAR = (16-1);
    SpiaRegs.SPICCR.bit.SPILBK = 1;

    // Enable master (0 == slave, 1 == master)
    // Enable transmission (Talk)
    // Clock phase (0 == normal, 1 == delayed)
    // SPI interrupts are disabled
    SpiaRegs.SPICTL.bit.MASTER_SLAVE = 1;
    SpiaRegs.SPICTL.bit.TALK = 1;
    SpiaRegs.SPICTL.bit.CLK_PHASE = 0;
    SpiaRegs.SPICTL.bit.SPIINTENA = 0;

    // Set the baud rate
    SpiaRegs.SPIBRR.bit.SPI_BIT_RATE = SPI_BRR;

    // Set FREE bit
    // Halting on a breakpoint will not halt the SPI
    SpiaRegs.SPIPRI.bit.FREE = 1;

    // Release the SPI from reset
    SpiaRegs.SPICCR.bit.SPISWRESET = 1;
}


void send_initial_clock_train (void)        //Change to whichever pins we are using.
{
    volatile unsigned long dat;

    /* Ensure CS (STE) is held high. */
    GpioDataRegs.GPASET.bit.GPIO19 = 1;

    /* Switch the SPI TX line to a GPIO and drive it high too. */
    EALLOW;
    GpioCtrlRegs.GPALOCK.bit.GPIO16 = 0;
    GpioCtrlRegs.GPAMUX2.bit.GPIO16 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO16 = 1;
    GpioCtrlRegs.GPALOCK.bit.GPIO16 = 1;
    EDIS;

    GpioDataRegs.GPASET.bit.GPIO16 = 1;        //Drive TX High

    /* Send 10 bytes over the SPI. This causes the clock to wiggle the */
    /* required number of times. */
    unsigned int i;
    for(i = 0 ; i < 10 ; i++)
    {
        /* Write DUMMY data */
        while(SpiaRegs.SPISTS.bit.BUFFULL_FLAG);
        SpiaRegs.SPITXBUF = 0xFF00;

        /* Flush data read during data write. */
        while(SpiaRegs.SPISTS.bit.INT_FLAG !=1);        //May be possible to switch to '!SpicRegs.SPISTS.bit.INT_FLAG'
        dat = (SpiaRegs.SPIRXBUF & 0xFF);
    }

    /* Revert to hardware control of the SPI TX line. */
    EALLOW;
    GpioCtrlRegs.GPALOCK.bit.GPIO16 = 0;
    GpioCtrlRegs.GPAMUX2.bit.GPIO16 = 2;
    GpioCtrlRegs.GPALOCK.bit.GPIO16 = 1;
    EDIS;
}

/*-----------------------------------------------------------------------*/
/* Send a command packet to MMC                                          */
/*-----------------------------------------------------------------------*/

BYTE send_cmd (
    BYTE cmd,        /* Command byte */
    DWORD arg        /* Argument */
)
{
    BYTE n, res;


    if (wait_ready() != 0xFF) return 0xFF;

    /* Send command packet */
    xmit_spi(cmd);                        /* Command */
    xmit_spi((BYTE)(arg >> 24));        /* Argument[31..24] */
    xmit_spi((BYTE)(arg >> 16));        /* Argument[23..16] */
    xmit_spi((BYTE)(arg >> 8));            /* Argument[15..8] */
    xmit_spi((BYTE)arg);                /* Argument[7..0] */
    n = 0;
    if (cmd == CMD0) n = 0x95;            /* CRC for CMD0(0) */
    if (cmd == CMD8) n = 0x87;            /* CRC for CMD8(0x1AA) */
    xmit_spi(n);

    /* Receive command response */
    if (cmd == CMD12) rcvr_spi();        /* Skip a stuff byte when stop reading */
    n = 10;                                /* Wait for a valid response in timeout of 10 attempts */
    do
        res = rcvr_spi();
    while ((res & 0x80) && --n);

    return res;            /* Return with the response value */
}

BYTE wait_ready (void)
{
    BYTE res;


    Timer2 = 50;    /* Wait for ready in timeout of 500ms */
    rcvr_spi();
    do{
        res = rcvr_spi();
        DELAY_US(175);
    } while ((res != 0xFF) && Timer2);

    return res;
}


/*-----------------------------------------------------------------------*/
/* Transmit a byte to MMC via SPI  (Platform dependent)                  */
/*-----------------------------------------------------------------------*/

static
void xmit_spi (BYTE dat)    //EDITED
{
    volatile DWORD rcvdat;                      //included file like integer.h for DWORD definition

    /* Write the data to the tx fifo */
    while(SpiaRegs.SPISTS.bit.BUFFULL_FLAG);    //Wait for room to write data
    SpiaRegs.SPITXBUF = ((DWORD)dat)<<8;                        //Write data

    /* flush data read during the write */
    while(SpiaRegs.SPISTS.bit.INT_FLAG !=1);        //May be possible to switch to '!SpicRegs.SPISTS.bit.INT_FLAG'
    rcvdat = (SpiaRegs.SPIRXBUF && 0xFF);                   //Clear Receive Buffer
}


/*-----------------------------------------------------------------------*/
/* Receive a byte from MMC via SPI  (Platform dependent)                 */
/*-----------------------------------------------------------------------*/

BYTE rcvr_spi (void)        //EDITED
{
    volatile DWORD rcvdat;

    //Disable transmission channel
    //SpicRegs.SPICTL.bit.TALK = 0;

    /* write dummy data */
    while(SpiaRegs.SPISTS.bit.BUFFULL_FLAG);    //Wait for space to write
    SpiaRegs.SPITXBUF = 0xFF00;                     //Write dummy data

    /* read data from RX fifo */
    while(SpiaRegs.SPISTS.bit.INT_FLAG !=1);        //May be possible to switch to '!SpicRegs.SPISTS.bit.INT_FLAG'
    rcvdat = (SpiaRegs.SPIRXBUF & 0xFF);                    //Read Transferred data

    return (BYTE)rcvdat;
}


void rcvr_spi_m (BYTE *dst)
{
    *dst = rcvr_spi();
}


void power_on (void)    //Configure to actual pins
{
    /*
     * This doesn't really turn the power on, but initializes the
     * SPI port and pins needed to talk to the SD card.
     */
    EALLOW;
    /* Enable the peripherals used to drive the SDC on SPI */
    CpuSysRegs.PCLKCR8.bit.SPI_C = 1;

    /*
     * Configure the appropriate pins to be SPI instead of GPIO. The CS
     * signal is directly driven to ensure that we can hold it low through a
     * complete transaction with the SD card.
     */


    //122 - 16
    //123 - 17
    //124 - 18
    //125 - 19
    //Unlock the SD-Card SPI GPIOs
    GpioCtrlRegs.GPALOCK.bit.GPIO16 = 0;        //MOSI
    GpioCtrlRegs.GPALOCK.bit.GPIO17 = 0;        //MISO
    GpioCtrlRegs.GPALOCK.bit.GPIO18 = 0;        //SCLK
    GpioCtrlRegs.GPALOCK.bit.GPIO19 = 0;        //Slave Transmit Enable

    //Set up  MUX & DIR
    GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 0;   //Leave as GPIO for manual CS control

    GpioCtrlRegs.GPAGMUX2.bit.GPIO16 = 1;
    GpioCtrlRegs.GPAGMUX2.bit.GPIO17 = 1;
    GpioCtrlRegs.GPAGMUX2.bit.GPIO18 = 1;

    GpioCtrlRegs.GPAMUX2.bit.GPIO16 = 2;
    GpioCtrlRegs.GPAMUX2.bit.GPIO17 = 2;
    GpioCtrlRegs.GPAMUX2.bit.GPIO18 = 2;

    GpioCtrlRegs.GPADIR.bit.GPIO19 = 1;

    //Set up GPIO Pull-up disables/enables
    GpioCtrlRegs.GPAPUD.bit.GPIO16 = 0;    //Needs to be normally pulled high
    GpioCtrlRegs.GPAPUD.bit.GPIO17 = 0;    //Needs to be normally pulled high
    GpioCtrlRegs.GPAPUD.bit.GPIO18 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO19 = 1;

    //Set up GPIOs in asynch mode
    GpioCtrlRegs.GPAQSEL2.bit.GPIO16 = 3; // Asynch input
    GpioCtrlRegs.GPAQSEL2.bit.GPIO17 = 3;
    GpioCtrlRegs.GPAQSEL2.bit.GPIO18 = 3;
    GpioCtrlRegs.GPAQSEL2.bit.GPIO19 = 3;

    //Configure GPIOs for CPU1
    GpioCtrlRegs.GPACSEL3.bit.GPIO16 = 0;
    GpioCtrlRegs.GPACSEL3.bit.GPIO17 = 0;
    GpioCtrlRegs.GPACSEL3.bit.GPIO18 = 0;
    GpioCtrlRegs.GPACSEL3.bit.GPIO19 = 0;

    //Lock the SD-Card SPI GPIOs
    GpioCtrlRegs.GPALOCK.bit.GPIO16 = 1;
    GpioCtrlRegs.GPALOCK.bit.GPIO17 = 1;
    GpioCtrlRegs.GPALOCK.bit.GPIO18 = 1;
    GpioCtrlRegs.GPALOCK.bit.GPIO19 = 1;
    EDIS;


    /* Deassert the SPIA chip selects for both the SD card and serial flash */
    GpioDataRegs.GPASET.bit.GPIO19 = 1;

    /* Configure the SPI A port */
    SpiaRegs.SPICCR.bit.SPISWRESET = 0;         //Set reset bit low
    SpiaRegs.SPICTL.bit.CLK_PHASE = 0;
    SpiaRegs.SPICCR.bit.CLKPOLARITY = 1;
    SpiaRegs.SPICTL.bit.MASTER_SLAVE = 1;       //Master mode
    SpiaRegs.SPIBRR.all = 63;                       //update value to proper setting for correct bitrate ( current: ~500kHz)
    SpiaRegs.SPICCR.bit.SPICHAR = 0x7;          //Set char length to 8 bits
    SpiaRegs.SPICTL.bit.TALK = 1;
    SpiaRegs.SPICCR.bit.SPISWRESET = 1;         //Release SPI from reset
    SpiaRegs.SPIPRI.bit.FREE = 1;
    SpiaRegs.SPIPRI.bit.SOFT = 1;

    /* Set DI and CS high and apply more than 74 pulses to SCLK for the card */
    /* to be able to accept a native command. */
    //send_initial_clock_train();

    //DELAY_US(50);

    PowerFlag = 1;
}





