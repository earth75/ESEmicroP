/* * * * * * * * *   GPSfinal.c    * * * * * * * * * *\
   This is the final program which includes all the 
       features that we aimed for in the TP:
      - GPS readings over UART
      - GPGGA frame detection and parsing
      - Display of the info in color over PPI
\* * * * * * * * * * * * * * * * * * * * * * * * * * */


//In order to fully initialize the system, we needed to reset the codec
//We did not want to spend much time doing this so we used an asm function provided with the IDE
extern void Flash_Setup_ADV_Reset(void); 

 
#include <cdefbf533.h>
#include <sys\exception.h>   	// system exception
#include <string.h>
#include "image.h" 		//screen library

#define IMAGE_START 0x00000000
#define BUFSIZE 150
#define BAUDRATE 4800
#define CLKIN 27000000

void initGPGGA(void);
void initScreen(void);
void initComm(void);
int sendUART(volatile char* msg);
void parserGPGGA(volatile char* buf);


//In order to hold the parsing results in a easily manageable way, we created a gpgga structure
typedef struct gpgga{
	char h[3];
	char m[3];
	char s[3];
	char lat[12];
	char lon[13];
}gpgga;
gpgga parse;

volatile char buffer [BUFSIZE], msg[BUFSIZE], del[BUFSIZE];
volatile int stream=0, TIP = 1, refresh = 0; //TIP : Transfer In Progress


EX_INTERRUPT_HANDLER(PPI_IRQ)
{
	*pDMA0_IRQ_STATUS = 0x0001; 	//acknowledging the interruption PPI
}

EX_INTERRUPT_HANDLER(DMA_IRQ)
{
	*pDMA7_IRQ_STATUS = 0x0001; 	//acknowledge the interruption PPI
	TIP = 0; 			//transfer end flag
}

EX_INTERRUPT_HANDLER(RX_IRQ)
{
	char rx=*pUART_RBR;
	if(rx=='$') stream=0;
	buffer[stream++] = rx;
	if (rx == 10 && buffer[4] == 'G' && buffer[5] == 'A' && stream>6) 
	{
		buffer[stream++] = 13; 	//adding terminating character
		buffer[stream++] = 0; 	//addin terminating character
		refresh = 1;
		sendUART(buffer); 	//charging the message in the DMA
		stream=0; 		//rewind
	}
}

//We chose to initialise the fields of the structure 
//to known values for easier debugging and less unknown behavior
void initGPGGA(void)
{
	parse.h[0]='T';
	parse.h[1]='T';
	parse.h[2]='\0';
	parse.m[0]='T';
	parse.m[1]='T';
	parse.m[2]='\0';
	parse.s[0]='T';
	parse.s[1]='T';
	parse.s[2]='\0';
	parse.lat[0]='T';
	parse.lat[1]='T';
	parse.lat[2]='T';
	parse.lat[3]='T';
	parse.lat[4]='T';
	parse.lat[5]='T';
	parse.lat[6]='T';
	parse.lat[7]='T';
	parse.lat[8]='T';
	parse.lat[9]='T';
	parse.lat[10]='T';
	parse.lat[11]='\0';
	parse.lon[0]='T';
	parse.lon[1]='T';
	parse.lon[2]='T';
	parse.lon[3]='T';
	parse.lon[4]='T';
	parse.lon[5]='T';
	parse.lon[6]='T';
	parse.lon[7]='T';
	parse.lon[8]='T';
	parse.lon[9]='T';
	parse.lon[10]='T';
	parse.lon[11]='T';
	parse.lon[12]='\0';
}

void initScreen(void)
{
	Flash_Setup_ADV_Reset(); 		//Activation of video codec through flash pin (asm default code)
	//configurating DMA buffer adress..
	*pDMA0_START_ADDR = IMAGE_START;
	//..and the screen size 
	*pDMA0_X_COUNT 	= 1716;
	*pDMA0_X_MODIFY = 1;
	*pDMA0_Y_COUNT 	= 525;
	*pDMA0_Y_MODIFY = 1;
	
	register_handler(ik_ivg8, PPI_IRQ);	//IT by default ikvg8
	*pSIC_IMASK |= 0x00000100;
	*pPPI_CONTROL = 0x0003; 		//Config DMA et PPI
	*pDMA0_CONFIG = 0x1091; 		//4,2,2 -> 8 bit color, output
	
}


void initComm(void)
{
	/* Automatic baudrate divisor calculation  */
	
	int CTL = *pPLL_CTL; 				//retrieve CTL value
	int DIV = *pPLL_DIV; 				//retrieve DIV value
	int MSL = (CTL<<1)>>10; 			//MSEL = CTL 14:9
	int SSL = DIV%16; 				//SSEL = DIV 3:0
	int div = (CLKIN*MSL)/(SSL*16*BAUDRATE);	//calculating the divisor for the baudrate set
	
	/* Now div holds the 16 divisor bits  */
	
	*pDMA7_START_ADDR = &buffer;
	*pDMA7_X_COUNT = BUFSIZE;
	*pDMA7_X_MODIFY = 0x0001;
	*pDMA7_CONFIG = 0x00A0;
	
	*pUART_LCR = 0x0083;  				//initializing the peripheral before unmasking 
	*pUART_DLL = div%0x100;  			//divisor on speed mode: 0x060B = 1547
	*pUART_DLH = div>>8;
	*pUART_LCR = 0x0003;
	*pUART_IER = 0x0003;
	*pUART_GCTL= 0x0001;
	
	*pSIC_IAR1 = 0x43322221;
	register_handler(ik_ivg10,RX_IRQ);
	register_handler(ik_ivg11, DMA_IRQ);
	*pSIC_IMASK |= 0x0000C000;
}	

/* * * * * * * * *   sendUART    * * * * * * * * * *\
   We wrote the sendUART function because we needed
   to send debug messages but wanted to use the DMA
                 for all messages
\* * * * * * * * * * * * * * * * * * * * * * * * * * */	
int sendUART(volatile char* msg){
	if(TIP == 0){					//testing if the transfer's still in progress
		TIP = 1;
		*pDMA7_START_ADDR = (void*)msg;
		*pDMA7_X_COUNT = strlen((char*)msg);
		*pDMA7_CONFIG |= 0x1;
		return 1;
	}
	else return 0;
}


/* * * * * * * * *   parserGPGGA   * * * * * * * * * *\
   This is the function that fills the GPGGA structure
   with the GPGGA string an input. The rather long code
   was intended because we wanted to make the function
very fast and we knew that GPGGA frames follow a pattern

For simplicity is erases the old data before parsing the
new data and then writing it. It is also easier to use a
  local struct with this function, although we did not 
  implement it yet and thus use a global variable ATM.
\* * * * * * * * * * * * * * * * * * * * * * * * * * */
void parserGPGGA(volatile char* buf)
{
	printString(IMAGE_START, 166, 170, 2, 4, parse.lat, BLUE);
	printString(IMAGE_START, 166, 240, 2, 4, parse.lon, BLUE);
	printString(IMAGE_START, 166, 100, 2, 4, parse.h, BLUE);
	printString(IMAGE_START, 202, 100, 2, 4, parse.m, BLUE);
	printString(IMAGE_START, 238, 100, 2, 4, parse.s, BLUE);
	
	if (atoi((char*)parse.s) == 11) printString(IMAGE_START, 5, 320, 6, 6, "MOUSTACHE", BLUE);
	
	parse.h[0]=buf[7];
	parse.h[1]=buf[8];
	parse.h[2]='\0';
	parse.m[0]=buf[9];
	parse.m[1]=buf[10];
	parse.m[2]='\0';
	parse.s[0]=buf[11];
	parse.s[1]=buf[12];
	parse.s[2]='\0';
	parse.lat[0]=buf[18];
	parse.lat[1]=buf[19];
	parse.lat[2]=buf[20];
	parse.lat[3]=buf[21];
	parse.lat[4]=buf[22];
	parse.lat[5]=buf[23];
	parse.lat[6]=buf[24];
	parse.lat[7]=buf[25];
	parse.lat[8]=buf[26];
	parse.lat[9]=' ';
	parse.lat[10]=buf[28];
	parse.lat[11]='\0';
	parse.lon[0]=buf[30];
	parse.lon[1]=buf[31];
	parse.lon[2]=buf[32];
	parse.lon[3]=buf[33];
	parse.lon[4]=buf[34];
	parse.lon[5]=buf[35];
	parse.lon[6]=buf[36];
	parse.lon[7]=buf[37];
	parse.lon[8]=buf[38];
	parse.lon[9]=buf[39];
	parse.lon[10]=' ';
	parse.lon[11]=buf[41];
	parse.lon[12]='\0';
	
	if (atoi((char*)parse.s) == 11) printString(IMAGE_START, 5, 320, 6, 6, "MOUSTACHE", WHITE);
	
	printString(IMAGE_START, 166, 100, 2, 4, parse.h, WHITE);
	printString(IMAGE_START, 202, 100, 2, 4, parse.m, WHITE);
	printString(IMAGE_START, 238, 100, 2, 4, parse.s, WHITE);
	printString(IMAGE_START, 166, 170, 2, 4, parse.lat, WHITE);
	printString(IMAGE_START, 166, 240, 2, 4, parse.lon, WHITE);
	
	refresh = 0;
}

void main(void) 
{
	int i;
	initGPGGA();
	strcpy ((char*)msg,"\n\r##TRANSMISSIONS START##\n\r\0"); //debug string sent in the stream
	
	for(i=0;i<BUFSIZE;i++)
		del[i]=i+32;
	imageInit(IMAGE_START, BLUE);
	printString(IMAGE_START, 80, 30, 8, 4, "GPS", WHITE);
	printString(IMAGE_START, 8, 40, 30, 4, "_", WHITE);
	printString(IMAGE_START, 10, 100, 2, 4, "Heure GMT:     :  :", WHITE);
	printString(IMAGE_START, 10, 170, 2, 4, "Latitude:", WHITE);
	printString(IMAGE_START, 10, 240, 2, 4, "Longitude:", WHITE);
	printString(IMAGE_START, 62, 450, 2, 2, "Copyright ENSEA 2015 (C)", WHITE);
	initScreen();
	
	initComm();
	while(!sendUART(msg));			//waiting for the transfer to finish
	
	while(1)
	if (refresh) parserGPGGA(buffer);	//refreshing the screen
}
