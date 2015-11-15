/*****************************************************************************
 * GPS.c
 *****************************************************************************/

#include <sys\exception.h>				// system exception
#include <cdefbf533.h>

typedef enum UARTstate {ATT, TSTG, TSTA, LOAD, RDY} UART_State;
UART_State state = ATT;

char buf;
volatile int stream=0; 	//characters' counter
volatile int pend=0; 	//synchronizing flag

EX_INTERRUPT_HANDLER(UART_IRQ)
{
	char rx=*pUART_RBR;
	/* Finite state machine */
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
		case TSTA:  	if(rx == 'A')
				{
					state = LOAD;
				}
				else {state = ATT;}
				break;
		case LOAD:  
				if(rx == 10)		//end of frame = new line (LF)
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
	
	/* Initializing IT and UART */
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

