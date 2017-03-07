#include "metaTait_SCI.h"
#include "F28x_Project.h"

#define Integration_Test        1


Uint16 scia_receive()                                       //UART Receive
{
    Uint16 b = SciaRegs.SCIRXBUF.all;
    return b;
}


void sci_init(void)
{
#if !Integration_Test
    //Initialize GPIO 70 & 71 for PCB

    GPIO_SetupPinMux(71, GPIO_MUX_CPU1, 1);                     //RX-TX Enable for SCIA
    GPIO_SetupPinOptions(71, GPIO_INPUT, GPIO_PUSHPULL);        //RX Pin
    GPIO_SetupPinMux(70, GPIO_MUX_CPU1, 1);
    GPIO_SetupPinOptions(70, GPIO_OUTPUT, GPIO_ASYNC);          //TX-Pin*/
#endif
#if Integration_Test
    GPIO_SetupPinMux(28, GPIO_MUX_CPU1, 1);
    GPIO_SetupPinOptions(28, GPIO_INPUT, GPIO_PUSHPULL);
    GPIO_SetupPinMux(29, GPIO_MUX_CPU1, 1);
    GPIO_SetupPinOptions(29, GPIO_OUTPUT, GPIO_ASYNC);
#endif
    SciaRegs.SCICCR.all = 0x0007;  // 1 stop bit,  No loopback, No parity, 8 char bits, async mode, idle-line protocol
    SciaRegs.SCICTL1.all = 0x0003; // enable TX, RX, internal SCICLK   Disable RX ERR, SLEEP, TXWAKE
    SciaRegs.SCICTL2.all = 0x0003;
    SciaRegs.SCICTL2.bit.TXINTENA = 1;
    SciaRegs.SCICTL2.bit.RXBKINTENA = 1;
    SciaRegs.SCIHBAUD.all = 0x0001;             //0x0002
    SciaRegs.SCILBAUD.all = 0x0046;           //0x008B
    SciaRegs.SCICTL1.all = 0x0023;     // Relinquish SCI from Reset
#if Integration_Test
    SciaRegs.SCICCR.bit.LOOPBKENA = 1;
#endif
    SciaRegs.SCIFFTX.all = 0xE040;      //SCIA FIFO Init
    SciaRegs.SCIFFRX.all = 0x2044;
    SciaRegs.SCIFFCT.all = 0x0;
}


void scia_xmit(int command, int var)
{
    int i = 0;
    switch(command)
    {
        case 0:
        {
            char comm[] = {"Ready to begin? Enter 1 to Start. \r\n"};
            for(i= 0; i < sizeof(comm); i++)
            {
                SciaRegs.SCITXBUF.all= comm[i];
            }
            break;
        }

        case 1:
        {
            char comm[] = {"Enter the RPM value, one character at a time \r\n"};
            for(i= 0; i < sizeof(comm); i++)
            {
                SciaRegs.SCITXBUF.all= comm[i];
            }
            break;
        }

        case 2:
        {
            char comm[] = {"Enter the global brightness value \r\n"};
            for(i= 0; i < sizeof(comm); i++)
            {
                SciaRegs.SCITXBUF.all= comm[i];
            }
            break;
        }

        case 3:
        {
            char comm[] = {"Target rpm value:"};
            for(i= 0; i < sizeof(comm); i++)
            {
                SciaRegs.SCITXBUF.all= comm[i];
            }
            char com = (char)var;
            SciaRegs.SCITXBUF.all= com;
            break;
        }

        case 4:
        {
            while (SciaRegs.SCIFFTX.bit.TXFFST != 0) {}
            char com = (char)var;
            SciaRegs.SCITXBUF.all= com;
            break;
        }
    }
}



