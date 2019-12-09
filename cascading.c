#include "adaboost.h"
#include "cascading.h"
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>





int casc(struct SCList *list, Matrix *m)
{
    //*list = sorted_list(*list);
    while (list != NULL)
    {
        if (!testStrong(m, &list->sc))
			return 0;
		list = list->next;
		//printf(".");
    }
    return 1;
}

PositionList* insert(PositionList* list, int x, int y, int size)
{
	PositionList* newResult = malloc(sizeof(PositionList));
	newResult->x = x;
	newResult->y = y;
	newResult->size = size;
	newResult->next = list;
	list = newResult;
	return list;
}



static inline int diff(int a, int b)
{
	return a>b ? a-b : b-a;
}

int overlap(int x1, int x2, int y1, int y2, int size1, int size2)
{
	/*if(x1 > x2+size2 || x2 > x1+size1)
		return 0;
	if(y1 > y2+size2 || y2 > y1+size1)
		return 0;
		return 1;*/
	int error = size1/2;
	return diff(x1,x2)<=error && diff(y1,y2)<=error && diff(size1,size2)<2*error;
}




PositionList *removeDoubles(PositionList *l)
{
	PositionList **it=&l;
	while(*it)
	{
		int doubleNumber=1;
		int xsum=(*it)->x, ysum=(*it)->y, sizeSum=(*it)->size;
		for(PositionList *i=*it; i->next;)
		{
			if(overlap((*it)->x, i->next->x, (*it)->y, i->next->y, (*it)->size, i->next->size))
			{
				doubleNumber++;
				xsum+=i->next->x;
				ysum+=i->next->y;
				sizeSum+=i->next->size;
				
				PositionList *aux=i->next->next;
				free(i->next);
				i->next=aux;
			}
			else
			{
				i=i->next;
			}
		}
		if(doubleNumber<3)
		{
			PositionList *aux=(*it)->next;
			free(*it);
			*it=aux;
			continue;
		}
		(*it)->x=xsum/doubleNumber;
		(*it)->y=ysum/doubleNumber;
		(*it)->size=sizeSum/doubleNumber;
		it=&(*it)->next;
	}	
	return l;
}





static inline int min(int x, int y)
{
	return x>y?y:x;
}



PositionList *casc_pos(SCList *l, Matrix *m)
{
	//int tmp = 0;

	PositionList *returnList = NULL;
	for (int size = 24; size <=m->x && size <=m->y ; size+=size/15)
	{
		for (m->xmin = 0; m->xmin + size <= m->x; m->xmin += size/24)
		{
			for(m->ymin = 0; m->ymin + size <= m->y; m->ymin += size/24)
			{
				m->xmax = m->xmin + size;
				m->ymax = m->ymin + size;
				
				//tmp++;

				if(casc(l, m))
				{
					// BUG HERE, CASC RETURN ALWAYS FALSE
					// MAYBE A BUG WITH PARAMETERS OF CASC
					// MAYBE THE MATRIX GIVEN IS WRONG ?
					// SHEPARD

					PositionList *aux=returnList;
					returnList=malloc(sizeof(PositionList));
					returnList->x=m->xmin;
					returnList->y=m->ymin;
					returnList->size=size;
					returnList->next=aux;
					//printf("\nFace found at: (%d,%d)(%d,%d)",
					//	   m->xmin, m->ymin,
					//	   m->xmax, m->ymax);
					//tmp++;

					returnList = insert(returnList, m->xmin, m->ymin, size);
				}
			}
		}
	}

	//printf("%d\n", tmp);

	returnList=removeDoubles(returnList);
	//returnList=removeDoubles(returnList);
	return returnList;
}


void freePositionList(PositionList *l)
{
	while(l)
	{
		PositionList *aux=l->next;
		free(l);
		l=aux;
	}
}



//int *pos(struct SCList *list, Matrix *m)
//{
//    int *pos = malloc(sizeof(int));
//    Sc_ite(list, m, pos);
//    return pos; 
//}
