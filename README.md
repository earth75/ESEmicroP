# ESEmicroP

There are several versions of the program, which were roughly made at the end of each TP session.

 - GPS.c selects the GPGGA frames with a state machine upon the UART RX interrupt and prints them directly in the UART TX
 - GPSFullInt selects the GPGGA like GPS.c frame and prints it directly using UART TX interrupt
 - GPSDMA selects the GPGGA frames like GPS.c but prints the buffer in the UART using DMA.
 - GPSFinal selects the GPGGA fames using simpler logic and prints them to the UARS using DMA. It also parses the Frame's informations ans displays it on a color screen using PPI over DMA.
 - image.c is the provided library for manipulating the screen output. A small change was made to the Printtring function, see the end of image.c for more info.
