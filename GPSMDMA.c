/*****************************************************************************
 * GPS.c
 *****************************************************************************/
extern void Flash_Setup_ADV_Reset(void);
 
#include <cdefbf533.h>
#include <sys\exception.h>   // system exception
#include <string.h>
#include "image.h" //librairie �cran

#define IMAGE_START 0x00000000
#define BUFSIZE 150
#define BAUDRATE 4800
#define CLKIN 27000000

void initGPGGA(void);
void initScreen(void);
void initComm(void);
int sendUART(volatile char* msg);
void parserGPGGA(volatile char* buf);

typedef struct gpgga{
	char h[3];
	char m[3];
	char s[3];
	char lat[12];
	char lon[13];
}gpgga;
gpgga parse;

volatile char buffer [BUFSIZE], msg[BUFSIZE], del[BUFSIZE];
volatile int stream=0, TIP = 1, refresh = 0; //tip : transfer in progress


EX_INTERRUPT_HANDLER(PPI_IRQ)
{
	*pDMA0_IRQ_STATUS = 0x0001; //acquitter l'interruption PPI
}

EX_INTERRUPT_HANDLER(DMA_IRQ)
{
	*pDMA7_IRQ_STATUS = 0x0001; //acquitter l'interruption PPI
	TIP = 0; //signaler la fin de transfert au reste du programme
}

EX_INTERRUPT_HANDLER(RX_IRQ)
{
	char rx=*pUART_RBR;
	if(rx=='$') stream=0;
	buffer[stream++] = rx;
	if (rx == 10 && buffer[4] == 'G' && buffer[5] == 'A' && stream>6) 
	{
		buffer[stream++] = 13; //ajouter un caract�re de terminaison
		buffer[stream++] = 0; //ajouter un caract�re de terminaison
		refresh = 1;
		sendUART(buffer); //charger le message dans le DMA
		stream=0; //rewind
	}
}

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
	Flash_Setup_ADV_Reset(); //Activation du codec video via un broche flash
	//On configure l'adresse du buffer du DMA
	*pDMA0_START_ADDR = IMAGE_START;
	//Taille de l'�cran : 
	*pDMA0_X_COUNT 	= 1716;
	*pDMA0_X_MODIFY = 1;
	*pDMA0_Y_COUNT 	= 525;
	*pDMA0_Y_MODIFY = 1;
	
	register_handler(ik_ivg8, PPI_IRQ);	//Int par d�faut ikvg8
	*pSIC_IMASK |= 0x00000100;
	*pPPI_CONTROL = 0x0003; //Config DMA et PPI
	*pDMA0_CONFIG = 0x1091; //4,2,2 -> 8 bit color, output
	
	//TODO : init the ADV7171
}


void initComm(void)
{
	/* calcul automatique du diviseur  */
	int CTL = *pPLL_CTL; //r�cup�rer a valeur de CTL
	int DIV = *pPLL_DIV; //r�cup�rer la valeur de DIV
	int MSL = (CTL<<1)>>10; //MSEL = CTL 14:9
	int SSL = DIV%16; //SSEL = DIV 3:0
	int div = (CLKIN*MSL)/(SSL*16*BAUDRATE); //calcul du diviseur pour avoir le baudrate
	/* * * * * * * * * * * * * * * * * */
	
	*pDMA7_START_ADDR = &buffer;
	*pDMA7_X_COUNT = BUFSIZE;
	*pDMA7_X_MODIFY = 0x0001;
	*pDMA7_CONFIG = 0x00A0;
	
	*pUART_LCR = 0x0083;  //initialiser le peripherique avant de demasquer 
	*pUART_DLL = div%0x100;  //diviseur en mode speed : 0x060B = 1547
	*pUART_DLH = div>>8;
	*pUART_LCR = 0x0003;
	*pUART_IER = 0x0003;
	*pUART_GCTL= 0x0001;
	
	*pSIC_IAR1 = 0x43322221;
	register_handler(ik_ivg10,RX_IRQ);
	register_handler(ik_ivg11, DMA_IRQ);
	*pSIC_IMASK |= 0x0000C000;
}	
	
int sendUART(volatile char* msg){
	if(TIP == 0){
			TIP = 1;
			*pDMA7_START_ADDR = (void*)msg;
			*pDMA7_X_COUNT = strlen((char*)msg);
			*pDMA7_CONFIG |= 0x1;
			return 1;
	}
	else return 0;
}

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
	strcpy ((char*)msg,"\n\r##TRANSMISSIONS START##\n\r\0");
	
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
	while(!sendUART(msg));
	
	while(1)
	if (refresh) parserGPGGA(buffer);
}