/*****************************************************************************
**																			**
**	 Project Name: 	Video_Output											**	
**																			**
******************************************************************************

(C) Copyright 2006 - Analog Devices, Inc.  All rights reserved.

File Name:		config_dma.asm

Date Modified:	4/4/03			Rev 1.0

Software:       VisualDSP++4.5

Hardware:		ADSP-BF533 EZ-KIT Board

Special Connections:  None

Purpose:		This config_dma.asm file sets up DMA0 (PPI DMA) for transmit,
				2D, autobuffer transfers from SDRAM memory to the PPI port.
				
******************************************************************************/


#include "defBF533.h"

#define SDRAM_START_ADDR 0x00000000

.global config_dma;


.section L1_code;

config_dma:

	
	//DMA0_START_ADDR
	R0.L = SDRAM_START_ADDR;		
	R0.H = SDRAM_START_ADDR;
	P0.L = lo(DMA0_START_ADDR);
	P0.H = hi(DMA0_START_ADDR);	
	[P0] = R0;							
	
	//DMA0_CONFIG
	R0.L = 0x1090;		
	P0.L = lo(DMA0_CONFIG);
	P0.H = hi(DMA0_CONFIG);	
	W[P0] = R0.L;						
	
	//DMA0_X_COUNT
	R0.L = 0x06B4;		
	P0.L = lo(DMA0_X_COUNT);
	P0.H = hi(DMA0_X_COUNT);	
	W[P0] = R0.L;						
		
	//DMA0_X_MODIFY
	R0.L = 0x0001;					
	P0.L = lo(DMA0_X_MODIFY);
	P0.H = hi(DMA0_X_MODIFY);	
	W[P0] = R0.L;						

	//DMA0_Y_COUNT
	R0.L = 0x020D;		
	P0.L = lo(DMA0_Y_COUNT);
	P0.H = hi(DMA0_Y_COUNT);	
	W[P0] = R0.L;						
		
	//DMA0_Y_MODIFY
	R0.L = 0x0001;					
	P0.L = lo(DMA0_Y_MODIFY);
	P0.H = hi(DMA0_Y_MODIFY);	
	W[P0] = R0.L;
		
	RTS;
	
config_dma.END:
