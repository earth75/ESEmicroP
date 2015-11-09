/*****************************************************************************
 * GPS Iterruption *
 *****************************************************************************/

#include <sys\exception.h>				// system exception


#define UART_LCR 0xFFC0040C
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

char buf;
int pend=1;

EX_INTERRUPT_HANDLER(UART_IRQ)
{
	buf=*pUART_RBR;
	pend=1;
}


void main(void)
{

	*pSIC_IMASK = 0x4000;
	register_handler(ik_ivg10,UART_IRQ);
	*pUART_IER = 0x0001;
	
	*pUART_LCR = 0x0083;
	*pUART_DLL = 0x0009;
	*pUART_DLH = 0x0005;
	*pUART_LCR = 0x0003;
	*pUART_GCTL = 0x0001;
	
	while(1)
	{
			while (!pend) {} //Write
				*pUART_THR=buf;
				pend=0;
	}
	
}
