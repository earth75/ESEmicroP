/*****************************************************************************
 * GPS.c
 *****************************************************************************/
#include <cdefbf533.h>
#include <sys\exception.h>   // system exception

/* */

#define BUFSIZE 90

typedef enum UARTstate {ATT, TSTG, TSTA, LOAD, RDY} UART_State;
UART_State state = ATT;

volatile char buffer [BUFSIZE];
volatile int stream=0, done = 0;

EX_INTERRUPT_HANDLER(DMA_IRQ)
{
	done = 1;
	*pDMA7_IRQ_STATUS = 0x0001; //
}

EX_INTERRUPT_HANDLER(RX_IRQ)
{
	char rx=*pUART_RBR;
	if(done == 1) 
	switch(state)
	{
		case ATT :	if(rx=='$')
					{
						stream=0;
						state = TSTG;
						buffer[stream++] = rx;
					}
					break;
		case TSTG:	if(stream < 4) buffer[stream++] = rx;
					else if(stream == 4 && rx == 'G')
					{
						state = TSTA;
						buffer[stream++] = rx;
					}
					else {state = ATT;}
					break;
		case TSTA:  if(rx == 'A')
					{
						state = LOAD;
						buffer[stream++] = rx;
					}
					else {state = ATT;}
					break;
		case LOAD:  buffer[stream++] = rx;
					if(rx == 10)
					{
						buffer[stream++] = rx;

						state = RDY;
						done = 0;
						*pDMA7_X_COUNT = stream;
						*pDMA7_CONFIG |= 0x1;
					}
					break;
		case RDY :	if(done == 1) state = ATT;
					break;
		default: state=ATT;
	}
}

void initUART(void)
{
	*pDMA7_START_ADDR = &buffer;
	*pDMA7_X_COUNT = BUFSIZE;
	*pDMA7_X_MODIFY = 0x0001;
	*pDMA7_CONFIG = 0x00A0;
	
	*pUART_LCR = 0x0083;  //initialiser le peripherique avant de demasquer 
	*pUART_DLL = 0x0009;
	*pUART_DLH = 0x0005;
	*pUART_LCR = 0x0003;
	*pUART_IER = 0x0003;
	*pUART_GCTL= 0x0001;
}	
	

void main(void)
{
	initUART();
	*pSIC_IAR1 = 0x43322221;
	register_handler(ik_ivg10,RX_IRQ);
	register_handler(ik_ivg11, DMA_IRQ);
	*pSIC_IMASK= 0xC000;
}

