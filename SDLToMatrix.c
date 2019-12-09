#include "SDLToMatrix.h"

void mallocMatrix(Matrix* m)
{
	m->array = malloc(m->x*sizeof(long*));
	for(int i=0; i<m->x; i++)
	{
		m->array[i]=malloc(m->y*sizeof(long));
	}
}


void freeMatrix(Matrix* m)
{
	for(int i=0; i<m->x; i++)
	{
		free(m->array[i]);
	}
	free(m->array);
}


void imgToMatrix(SDL_Surface* image, Matrix* m)
{
	m->x=image->w;
	m->y=image->h;


	mallocMatrix(m);

	m->xmin=0;
	m->xmax=m->x;
	m->ymin=0;
	m->ymax=m->y;

	int bpp=image->format->BytesPerPixel;
	Uint8 r=0, g=0, b=0;

	for(int i=0; i<image->w; i++)
	{
		for(int j=0; j<image->h; j++)
		{
			Uint8* pixel =
				(Uint8*)image->pixels+j*image->pitch+i*bpp;
			Uint32 color=0;

			switch(bpp)
			{
				case 1:
					color=*pixel;
					break;
				case 2:
					color=*(Uint16*)pixel;
					break;
				case 3:

					if(SDL_BYTEORDER==SDL_BIG_ENDIAN)

						color=pixel[0]<<16|pixel[1]<<8|pixel[2];
					else

						color=pixel[0]|pixel[1]<<8|pixel[2]<<16;
				case 4:
					color=*(Uint32*)pixel;
			}

			SDL_GetRGB(color, image->format, &r, &g, &b);
			m->array[i][j] = (r+g+b)/3;
		}
	}
}


void toIntegral(Matrix* m)
{

	for(int i=1; i<m->x; i++)
	{
		m->array[i][0]+=m->array[i-1][0];
	}

	for(int i=1; i<m->y; i++)
	{
		m->array[0][i]+=m->array[0][i-1];
	}
	for(int i=1; i<m->x; i++)
	{
		for(int j=1; j<m->y; j++)
		{
			m->array[i][j]+=m->array[i-1][j]+m->array[i][j-1]
			-m->array[i-1][j-1];
		}
	}
}


void printMatrix(Matrix* m)
{
	for (int i=0; i < m->y; i++)
	{
		for (int j=0; j < m->x; j++)
		{
			printf("%lu ", m->array[j][i]);
		}

		printf("\n");
	}
}


void getIntegralImage(char* str, Matrix* m)
{
	SDL_Surface* img = IMG_Load(str);
	if(!img)
	{
		m=NULL;
		printf("error: can not load image\n");
		exit(0);
	}
	printf("image loaded\n");

	imgToMatrix(img, m);

	toIntegral(m);

	SDL_FreeSurface(img);
}

void getGreyImage(char* str, Matrix* m)
{
	SDL_Surface* img = IMG_Load(str);
	
	if (!img)
	{
		m = NULL;
		printf("error: can not load image\n");
		exit(0);
	}

	printf("image loaded\n");

	imgToMatrix(img, m);

	SDL_FreeSurface(img);
}

