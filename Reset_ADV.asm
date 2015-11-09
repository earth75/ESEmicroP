/*****************************************************************************
**																			**
**	 Project Name: 	Video_Output											**	
**																			**
******************************************************************************

(C) Copyright 2006 - Analog Devices, Inc.  All rights reserved.

File Name:		Reset_ADV.asm

Date Modified:	4/4/03		Rev 1.0

Software:       VisualDSP++4.5

Hardware:		ADSP-BF533 EZ-KIT Board

Special Connections:  None

Purpose:		This file resets the ADV7171 device.  The ADP7171 device's 
				reset pin is connected to a pin on Flash A, which is mapped to
				asynchronous memory.  (See ADSP-BF533 EZ-KIT schematics.)  
				

******************************************************************************/


#include "defBF533.h"
#include "flash_config.h"

.global _Flash_Setup_ADV_Reset;			


/*************************** Reset the encoder. ******************************/

.section L1_code;

_Flash_Setup_ADV_Reset:

	//Set asynch mem timing registers.
	P0.H = hi(EBIU_AMBCTL0);
	P0.L = lo(EBIU_AMBCTL0);
	R0.L = amb0_timing;
	R0.H = amb1_timing;
	[P0] = R0;
	ssync;
	
	P0.H = hi(EBIU_AMBCTL1);
	P0.L = lo(EBIU_AMBCTL1);
	R0.L = amb2_timing;
	R0.H = amb3_timing;
	[P0] = R0;
	ssync;

	//Set asynch mem global ctrl register.
	P0.H = hi(EBIU_AMGCTL);
	P0.L = lo(EBIU_AMGCTL);
	R0.L = W[P0];
	R1 = en_async_mem;		// Enable asynch mem.
	R0 = R0 | R1;
	W[P0] = R0;		
	ssync;

	//Initialize flash A csio regs (port A and B).

    //Clear data registers.
	P1.H = hi(flashA_csio);
	P1.L = lo(flashA_csio) + portA_data_out;
	R0 = 0x0;
	W[P1] = R0;
	ssync;
	
    //Direction control registers.
	P1.H = hi(flashA_csio);
	P1.L = lo(flashA_csio) + portA_data_dir;
	R0 = 0xFFFF(Z);     
	W[P1] = R0;			 //Set direction to output.
	ssync;
	
	//De-assert reset from ADP7171 device (flashA portA bit 2) 

	P1.H = hi(flashA_csio);
	P1.L = lo(flashA_csio) + portA_data_out;
	R0 = B[P1] (z);
	bitset(R0,RST_7171);     // de-assert reset of the encoder device
	B[P1] = R0;

	
	
	RTS;

_Flash_Setup_ADV_Reset.END:
