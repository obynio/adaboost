#include "utils.h"
#include "cascading.h"


void putpixel(SDL_Surface *surface, int x, int y)
{
	int bpp = surface->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to set */
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

	switch(bpp) {
		case 1:
			p[0] = 255;
			break;

		case 2:
			p[0] = 255;
			p[1] = 255;
			break;

		case 3:
			p[0] = 0;
			p[1] = 255;
			p[2] = 0;
			break;

		case 4:
			p[0] = 0;
			p[1] = 255;
			p[2] = 0;
			p[3] = 255;
			break;
	}
}

void GtkAdaptation (SDL_Surface* img)
{
	int bpp = img->format->BytesPerPixel;

	// For loading BMP (swap red and blue)
	if(bpp == 3 && SDL_BYTEORDER != SDL_BIG_ENDIAN) {
		for (int x = 0; x < img->w; x++)
		{
			for (int y = 0; y < img->h; y++)
			{
				Uint8 *p = (Uint8 *)img->pixels + y * img->pitch + x * bpp;
				Uint8 tmp = p[0];
				p[0] = p[2];
				p[2] = tmp;
			}
		}
	}
}


void showResult (SDL_Surface* img, int xmin, int ymin, int xmax, int ymax)
{
	if(xmin < 0) { xmin = 0;}
	if(xmax > img->w-1) { xmax = img->w-1;}
	if(ymin < 0) { ymin = 0;}
	if(ymax > img->h-1) { ymax = img->h-1;}

	for(int x = xmin; x < xmax; x++)
	{
		putpixel(img, x, ymin);
		putpixel(img, x, ymax);
	}

	for(int y = ymin; y < ymax; y++)
	{
		putpixel(img, xmin, y);
		putpixel(img, xmax, y);
	}
}

void showResults (SDL_Surface* img, PositionList* result)
{
	while(result != NULL)
	{
	   showResult(img, result->x, result->y, result->x + result->size, result->y + result->size);
	   result = result->next;
	}
}

int detect2(SDL_Surface* surface)
{
	printf("\nReading database");
	SCList* l = dbExtract();

	Matrix m;
	imgToMatrix(surface, &m);
	toIntegral(&m);

	printf("\nStarting cascading\n");

	PositionList* result = casc_pos(l, &m);
	showResults(surface, result);

	freeSCList(l);
	freeMatrix(&m);
	
	return result != NULL;
}

