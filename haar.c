#include "haar.h"

float accessPixel24(Matrix* m, float x, float y)
{

	//printf("(%d,%d)  ",x,y);
	float newx=((float)(x*(m->xmax-m->xmin)))/24.0  -1 + m->xmin;
	float newy=((float)(y*(m->ymax-m->ymin)))/24.0  -1 + m->ymin;

	//printf("%f,%f ",newx,newy);

	int intx=newx;
	int inty=newy;
	int decimalx=newx-intx;
	int decimaly=newy-inty;

	if(intx<0||inty<0||intx>=m->x||inty>=m->y)
		return 0;

	if(intx>=m->x-1||inty>=m->y-1)
		return ((float)m->array[intx][inty])/((m->xmax-m->xmin)*(m->ymax-m->ymin));

	float r = m->array[intx][inty] +
		decimalx*(m->array[intx+1][inty]) +
		decimaly*(m->array[intx][inty+1]);
	//printf("%f ",((float)r)/(m->x*m->y));
	return r/((m->xmax-m->xmin)*(m->ymax-m->ymin));
}


float rectSum(Matrix* m, float xmin, float ymin, float xmax, float ymax)
{
	float s=accessPixel24(m, xmax, ymax)
		-accessPixel24(m, xmax, ymin)
		-accessPixel24(m, xmin, ymax)
		+accessPixel24(m, xmin, ymin);
	return s;

}


float haar(Matrix* m, int type, int xmin, int ymin, int xmax, int ymax)
{
	float ymed=(ymin+ymax)/2;
	float xmed=(xmin+xmax)/2;
	float xthird=(xmin+xmax)/3;
	switch(type)
	{
		case 1:
			return rectSum(m, xmed, ymin, xmax, ymax)
				-rectSum(m, xmin, ymin, xmed, ymax);

		case 2:
			return rectSum(m, xmin, ymin, xmax, ymed)
				-rectSum(m, xmin, ymed, xmax, ymax);

		case 3:
			return (2*rectSum(m, xmin+xthird, ymin, xmin+2*xthird, ymax))
				-rectSum(m, xmin, ymin, xmax, ymax);

		case 4:
			return rectSum(m, xmin, ymin, xmax, ymax)
				-(2*rectSum(m, xmed, ymin, xmax, ymed))
				-(2*rectSum(m, xmin, ymed, xmed, ymax));
	}
	return 0;
}


float relativeHaar(Matrix *m, int type, int xmin, int ymin, int xmax, int ymax)
{
	double intVal = haar(m, type, xmin, ymin, xmax, ymax);
	//double max = (((long)(xmax-xmin)*(ymax-ymin)*255) *
	//	((long)(m->xmax-m->xmin)*(m->ymax-m->ymin)))/576;
	//double max=(m->xmax-m->xmin)*(m->ymax-m->ymin);
	double max=1;
	//printf("(%d %d)(%d %d) %d =%f/%f=%f\n",xmin, ymin, xmax, ymax, type, intVal, max, intVal/max);
	return intVal / max;
}
