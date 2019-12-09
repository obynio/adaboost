#ifndef _CASCADING_H_
#define _CASCADING_H_

#include "SDLToMatrix.h"
#include "adaboost.h"

typedef struct PositionList
{
	struct PositionList *next;
	int x;
	int y;
	int size;
} PositionList;


int casc(struct SCList *list, Matrix *m);
PositionList *casc_pos(SCList *l, Matrix *m);
void freePositionList(PositionList *l);
#endif
