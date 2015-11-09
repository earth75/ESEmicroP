/*****************************************************************************
**																			**
**	 Project Name: 	Video_Output											**	
**																			**
******************************************************************************

(C) Copyright 2006 - Analog Devices, Inc.  All rights reserved.

File Name:		config_ppi.asm

Date Modified:	4/4/03			Rev 1.0

Software:       VisualDSP++4.5

Hardware:		ADSP-BF533 EZ-KIT Board

Special Connections:  None

Purpose:		This config_ppi.asm file configures the PPI for output, 
				ITU-655 mode.  
				
******************************************************************************/


#include "defBF533.h"

.global config_ppi;

.section L1_code;

config_ppi:
	

	/* PPI Control Register:  Output direction, 656 mode.	*/

	P0.L = PPI_CONTROL & 0xffff;
	P0.H = PPI_CONTROL >> 16;
	R0.L = PORT_DIR;
	W[P0] = R0.L;
	
	RTS;
	
config_ppi.END:	
