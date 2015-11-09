#ifndef __IMAGE
  #define __IMAGE

#define BLACK   0x10801080
#define BLUE    0x296E29F0
#define RED     0x51F0515A
#define MAGENTA 0x6ADE6ACA
#define GREEN   0x91229136
#define CYAN    0xAA10AAA6
#define YELLOW  0xD292D210
#define WHITE   0xEB80EB80
#define FOND	BLUE


void imageInit(unsigned int *image, unsigned int bg_color);
void putPixel(unsigned int *image, short x, short y, unsigned int color);
void line(unsigned int *image, short x1, short y1, short x2, short y2, unsigned int color);
void horLine(unsigned int *image, short startx, short starty, short length, unsigned int color);
void vertLine(unsigned int *image, short startx, short starty, short length, unsigned int color);
void rectangle(unsigned int *image, short startx, short starty, short lengthx, short lenghty, unsigned int color);
void rectangleFill(unsigned int *image, short startx, short starty, short lenghtx, short lenghty, unsigned int color);
void printChar(unsigned int *image, short x, short y, short lx, short ly, char car, unsigned int color);
void printString(unsigned int *image, short x, short y, short lx, short ly, char *str, unsigned int color);

#endif
