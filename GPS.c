/*****************************************************************************
 * GPS.c
 *****************************************************************************/

#include <sys\exception.h>				// system exception
#include <cdefbf533.h>
/*#include <drivers\adi_dev.h>			// device manager includes
#include <drivers\uart\adi_uart.h>		// uart driver includes
#include "ezkitutilities.h"				// EZ-Kit utilities*/
 
/* */

/*#define UART_LCR 0xFFC0040C
#define pUART_LCR (volatile unsigned int *) UART_LCR
#define UART_MCR 0xFFC00410
#define pUART_MCR (volatile unsigned int *) UART_MCR

#define UART_LSR 0xFFC00414
#define pUART_LSR (volatile unsigned int *) UART_LSR
#define UART_THR 0xFFC00400
#define pUART_THR (volatile unsigned int *) UART_THR
#define UART_RBR 0xFFC00400
#define pUART_RBR (volatile unsigned int *) UART_RBR
#define UART_DLL 0xFFC00400
#define pUART_DLL (volatile unsigned int *) UART_DLL
#define UART_DLH 0xFFC00404
#define pUART_DLH (volatile unsigned int *) UART_DLH
#define UART_GCTL 0xFFC00424
#define pUART_GCTL (volatile unsigned int *) UART_GCTL

#define SIC_IMASK 0xFFC0010C
#define pSIC_IMASK (volatile unsigned int *) SIC_IMASK
#define UART_IER 0xFFC00404
#define pUART_IER (volatile unsigned int *) UART_IER

#define DMA7_CONFIG 0xFFC00DC8
#define pDMA7_CONFIG (volatile unsigned int *) DMA7_CONFIG
#define DMA7_X_COUNT 0xFFC00DD0
#define pDMA7_X_COUNT (volatile unsigned int *) DMA7_X_COUNT*/

typedef enum UARTstate {ATT, TSTG, TSTA, LOAD, RDY} UART_State;
UART_State state = ATT;

char buf;
//volatile char bufferRX [100];
volatile int stream=0, pend=0;

EX_INTERRUPT_HANDLER(UART_IRQ)
{
	char rx=*pUART_RBR;
	switch(state){
		case ATT :	if(rx=='$')
					{
						stream=0;
						state = TSTG;
					}
					break;
		case TSTG:	if(stream < 4) ;
					else if(stream == 4 && rx == 'G')
					{
						state = TSTA;
					}
					else {state = ATT;}
					break;
		case TSTA:  if(rx == 'A')
					{
						state = LOAD;
					}
					else {state = ATT;}
					break;
		case LOAD:  
					if(rx == 10)
					{
						state = RDY;
						pend = 1;
						*pUART_THR = rx;
					}
					else *pUART_THR = rx;
					break;
		case RDY :	if(pend == 0) state = ATT;
					break;
		default: state=ATT; *pUART_THR = rx;
	}
	stream++;
}

void main(void)
{
	int i;
	
	*pSIC_IMASK=0x4000;
	register_handler(ik_ivg10,UART_IRQ);
	*pUART_IER = 0x0001;
	
	*pUART_LCR = 0x0083;
	*pUART_DLL = 0x0009;
	*pUART_DLH = 0x0005;
	*pUART_LCR = 0x0003;
	*pUART_GCTL = 0x0001;
	
	
	while(1)
	{
			if(pend == 1)
				pend=0;
	}
	
}

