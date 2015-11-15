#include "charset.h"

// Début de trame vidéo : synchronisation
#define BLANK 0x10801080

// Nombre de colonnes total et sur l'écran
#define NB_COLONNES_TOTAL 429
#define NB_COLONNES_BLANK 68
#define NB_COLONNES NB_COLONNES_TOTAL-NB_COLONNES_BLANK

// Nombre de lignes total et sur l'écran
#define NB_LIGNES_1 20
#define NB_LIGNES_2 282
#define NB_LIGNES	NB_LIGNES_2*2-NB_LIGNES_1 *2

// Tempo pour respecter l'accès à la mémoire SDRAM
#define TEMPO 	0x50

int abs(int nb)
{
	if(nb < 0)
		return -nb;
	else
		return nb;
}

void imageInit(unsigned int *image, unsigned int bg_color)
{
   int ligne, colonne;
   unsigned int *pixel = image;

    /* Lignes 1 a 3*/
    for (ligne = 0; ligne <= 2; ligne++)
    {
        *pixel = 0xF10000FF;
        pixel++;

        for (colonne = 1; colonne <= 67; colonne++)
        {
            *pixel = BLANK;
            pixel++;
        }

        *pixel = 0xEC0000FF;
        pixel++;

        for (colonne = 69; colonne <= 428; colonne++)
        {
            *pixel = BLANK;
            pixel++;
        }
    }

    /* Lignes 4 a 19 */
    for (ligne = 3; ligne <= 18; ligne++)
    {
        *pixel = 0xB60000FF;
        pixel++;

        for (colonne = 1; colonne <= 67; colonne++)
        {
            *pixel = BLANK;
            pixel++;
        }

        *pixel = 0xAB00FF;
        pixel++;

        for (colonne = 69; colonne <= 428; colonne++)
        {
            *pixel = BLANK;
            pixel++;
        }
    }

    /* Lignes 20 a 263 */
    for (ligne = 19; ligne <= 262; ligne++)
    {
        *pixel = 0x9D0000FF;
        pixel++;

        for (colonne = 1; colonne <= 67; colonne++)
        {
            *pixel = BLANK;
            pixel++;
        }

        *pixel = 0x800000FF;
        pixel++;

        for (colonne = 69; colonne <= 428; colonne++)
        {
            *pixel = bg_color;
            pixel++;
        }
    }

    /* Lignes 264 a 265 */
    for (ligne = 263; ligne <= 264; ligne++)
    {
        *pixel = 0xB60000FF;
        pixel++;

        for (colonne = 1; colonne <= 67; colonne++)
        {
            *pixel = BLANK;
            pixel++;
        }

        *pixel = 0xAB0000FF;
        pixel++;

        for (colonne = 69; colonne <= 428; colonne++)
        {
            *pixel = BLANK;
            pixel++;
        }
    }

    /* lignes 266 a 282 */
    for (ligne = 265; ligne <= 281; ligne++)
    {
        *pixel = 0xF10000FF;
        pixel++;

        for (colonne = 1; colonne <= 67; colonne++)
        {
            *pixel = BLANK;
            pixel++;
        }

        *pixel = 0xEC0000FF;
        pixel++;

        for (colonne = 69; colonne <= 428; colonne++)
        {
            *pixel = BLANK;
            pixel++;
        }
    }

    /* Lignes 283 a 525 */
    for (ligne = 282; ligne <= 524; ligne++)
    {
        *pixel = 0xDA0000FF;
        pixel++;

        for (colonne = 1; colonne <= 67; colonne++)
        {
            *pixel = BLANK;
            pixel++;
        }

        *pixel = 0xC70000FF;
        pixel++;

        for ( colonne = 69; colonne <= 428; colonne++)
        {
            *pixel = bg_color;
            pixel++;
        }
    }
}

void putPixel(unsigned int *image, short x, short y, unsigned int color)
{
    unsigned int *pixel = image;
	int i;
    
    for(i = 0; i < TEMPO; i++);
    
    if (y % 2 == 0)
        pixel += (NB_LIGNES_1 * NB_COLONNES_TOTAL) + (y / 2 * NB_COLONNES_TOTAL) + NB_COLONNES_BLANK + x;
    else
        pixel += (NB_LIGNES_2 * NB_COLONNES_TOTAL) + (y / 2 * NB_COLONNES_TOTAL) + NB_COLONNES_BLANK + x;

    *pixel = color;
}

void line(unsigned int * image, int xa, int ya, int xb, int yb, int couleur){
	int alpha = 0, unit = 4096, dec = 12;
	int dx = 0, dy = 0, x = 0, y =0;
	int xadec = 0, xbdec = 0, yadec = 0, ybdec = 0;

	xadec = xa << dec; xbdec = xb << dec; yadec = ya << dec; ybdec = yb << dec;

	if( (xbdec - xadec) >= 0) dx = +unit; else dx = -unit;
	if( (ybdec - yadec) >= 0) dy = +unit; else dy = -unit;

	if(xadec != xbdec){
		alpha = (yb - ya) << dec;
		alpha /= (xb - xa);
	}
	y = yadec;

	if( xadec > 0 && xbdec > 0) {
		if(xadec == xbdec) {
			y = yadec;
			while(y != ybdec ){
				putPixel(image, xadec>>dec, y>>dec, couleur);
				y = y + dy;
			}
		}
		else if(yadec == ybdec) {
			x = xadec;
			while(x != xbdec ){
				putPixel(image, x>>dec, yadec>>dec, couleur);
				x = x + dx;
			}
		}
		else if (abs(alpha) <= unit){
			if (dx > 0) {
				for( x =xadec ; x <= xbdec ; x = x + dx){
					putPixel(image, x>>dec, y>>dec, couleur);
					y = y + (alpha);
				}
			}
			else {
				for( x = xadec ; x >= xbdec ; x = x + dx){
					putPixel(image, x>>dec, y>>dec, couleur);
					y = y - (alpha);
				}
			}
		}
		else {
			if(ya != yb){
				alpha = (xb - xa) << dec;
				alpha /= (yb - ya);
			}
			x = xadec;
			if (dy > 0) {
				for( y = yadec ; y <= ybdec ; y = y + dy){
					putPixel(image, x>>dec, y>>dec, couleur);
					x = x + (alpha);
				}
			}
			else {
				for( y =yadec ; y >= ybdec ; y = y + dy){
					putPixel(image, x>>dec, y>>dec, couleur);
					x = x - (alpha);
				}
			}
		}
	}
}

void horLine(unsigned int *image, short startx, short starty, short length, unsigned int color)
{
    unsigned int *pixel = image;
    short current;

//    if (starty % 2 == 0)
	if((starty&1) == 0)
		pixel += (NB_LIGNES_1 * NB_COLONNES_TOTAL) + ((starty/2) * NB_COLONNES_TOTAL) + NB_COLONNES_BLANK + startx;
    else
        pixel += (NB_LIGNES_2 * NB_COLONNES_TOTAL) + (((starty + 1)/2) * NB_COLONNES_TOTAL) + NB_COLONNES_BLANK + startx;

    for(current = 0; current < length; current++)
    {
        *pixel = color;
        pixel++;
    }
}

void vertLine(unsigned int *image, short startx, short starty, short length, unsigned int color)
{
    unsigned int *pixel1 = image + (NB_LIGNES_1 * NB_COLONNES_TOTAL) + ((starty/2) * NB_COLONNES_TOTAL) + NB_COLONNES_BLANK + startx;
    unsigned int *pixel2 = image + (NB_LIGNES_2 * NB_COLONNES_TOTAL) + ((starty/2) * NB_COLONNES_TOTAL) + NB_COLONNES_BLANK + startx;
    short tramme_paire;
    short current;

    if (starty % 2 == 1)
        tramme_paire = 0;
    else
        tramme_paire = 1;

    for(current = 0; current < length; current++)
    {
        if(tramme_paire == 1)
        {
            *pixel1 = color;
            pixel1 += NB_COLONNES_TOTAL;
        }
        else
        {
            *pixel2 = color;
            pixel2 += NB_COLONNES_TOTAL;
        }
        tramme_paire = !tramme_paire;
    }
}

void rectangle(unsigned int *image, short startx, short starty, short lenghtx, short lenghty, unsigned int color)
{
    horLine(image, startx, starty, lenghtx, color);
    horLine(image, startx, starty + lenghty - 1, lenghtx, color);
    vertLine(image, startx, starty + 1, lenghty - 2, color);
    vertLine(image, startx + lenghtx - 1, starty + 1, lenghty - 2, color);
}

void rectangleFill(unsigned int *image, short startx, short starty, short lenghtx, short lenghty, unsigned int color)
{
    short ligne;

    for(ligne = 0; ligne < lenghty; ligne++)
        line(image, startx, starty + ligne, startx + lenghtx, starty + ligne, color);
}

void printChar(unsigned int *image, short x, short y, short lx, short ly, char car, unsigned int color)
{
    short i,j,k,l,a;
    char *matrice = charset[car - 32];
    char ligne;
    unsigned int *pixel1 = image + (NB_LIGNES_1 * NB_COLONNES_TOTAL) + ((y/2) * NB_COLONNES_TOTAL) + NB_COLONNES_BLANK + x;
    unsigned int *pixel2 = image + (NB_LIGNES_2 * NB_COLONNES_TOTAL) + ((y/2) * NB_COLONNES_TOTAL) + NB_COLONNES_BLANK + x;
    short tramme_paire;

    if (y % 2 == 1)
        tramme_paire = 0;
    else
        tramme_paire = 1;

    for(i = 0; i < 8; i++)
    {
       for(k = 0; k < ly; k++)
       {
       	ligne = *matrice;

       	for(j = 0; j < 8; j++)
        {
         for(l = 0; l < lx; l++)
         {
         	if(tramme_paire == 1)
        	{
           		if(ligne & 0x80)
           			*pixel1 = color;
           	 	pixel1++;
			}
          	else
        	{
        	 	if(ligne & 0x80)
           	 		*pixel2 = color;
           	 	pixel2++;
        	}
         }
		for (a=0;a<15;a++);
          ligne = ligne << 1;
		}

        if(tramme_paire == 1)
           	pixel1 += NB_COLONNES_TOTAL - 8 * lx;
        else
           	pixel2 += NB_COLONNES_TOTAL - 8 * lx;

        tramme_paire = !tramme_paire;
        }
       matrice++;
	}
}

/* *  printString with automatic line feed * * *\
 While debugging we found out that printing large
 strings caused major screen dysfunctions because
   the function printed chars beyond the screen
       space and thus disturbed blanking
 We made this versin which goes to the next if
            the string is too large
\* * * * * * * * * * * * * * * * * * * * * * * */



void printString(unsigned int *image, short x, short y, short lx, short ly, char *str, unsigned int color)
{
    char *strPos = str;
    short posx = x, posy = y, i;
    int limit = (351-x)/(6*lx);
    while(1){
		for(i=0; i<limit; i++){
		  if(*str != '\0')
		  {
			  printChar(image, posx, posy, lx, ly, *str, color);
			  posx += 6 * lx;
			   str++;
		   }
		  else return;
		}
	posy+=35;
	posx = x;
	}
}


