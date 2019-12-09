#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef _SDLTOMATRIX_H_
#define _SDLTOMATRIX_H_

typedef struct
{
	unsigned long** array;
	int x;
	int y;

	int xmin;
	int ymin;
	int xmax;
	int ymax;
} Matrix;




void imgToMatrix(SDL_Surface* image, Matrix* m);
void toIntegral(Matrix* m);

void getIntegralImage(char* str, Matrix* m);
void getGreyImage(char* str, Matrix* m);
void printMatrix(Matrix* m);

void mallocMatrix(Matrix* m);
void freeMatrix(Matrix* m);

#endif
