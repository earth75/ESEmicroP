/*****************************************************************************
 * GPS.c
 *****************************************************************************/
#include <cdefbf533.h>
#include <sys\exception.h>   // system exception

/* */

typedef enum UARTstate {ATT, TSTG, TSTA, LOAD, RDY} UART_State;
UART_State state = ATT;

char buf;
volatile char buffer [100];
volatile int stream=0, pend=0, i=0;

EX_INTERRUPT_HANDLER(TX_IRQ)
{
	if(pend == 1) *pUART_THR = buffer[i++];
	
	if (i == stream) {pend = 0; i=0;}
}

EX_INTERRUPT_HANDLER(RX_IRQ)
{
	char rx=*pUART_RBR;
	if(pend == 0){ //le if empeche le full duplex (TODO : le réactiver)
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
					}
					break;
		case RDY :	if(pend == 0) state = ATT;
					break;
		default: state=ATT;
	}
	buffer[stream++] = rx;
	}
}

void main(void)
{

	*pUART_LCR = 0x0083;  //initialiser le peripherique avant de demasquer 
	*pUART_DLL = 0x0009;
	*pUART_DLH = 0x0005;
	*pUART_LCR = 0x0003;
	*pUART_IER = 0x0003;
	*pUART_GCTL= 0x0001;	
	
	*pSIC_IAR1 = 0x43000000;
	register_handler(ik_ivg10,RX_IRQ);
	register_handler(ik_ivg11,TX_IRQ);
	
	*pSIC_IMASK= 0x0000C000;
	
	
	

	
	*pUART_THR = 'A';
	
	
	while(1)
	{	
	}
	
}

