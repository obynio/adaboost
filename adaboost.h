#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <math.h>
#include <time.h>
#include <pthread.h>
#include <dirent.h> 
#include "haar.h"

#ifndef _ADABOOST_H_
#define _ADABOOST_H_

//One weak classifier (One haar + threshold)
typedef struct WeakClassifier
{
	int xmin;
	int ymin;
	int xmax;
	int ymax;
	int type;

	int parity;        //-1 or 1, if the haar value should be negative
	float threshold;
} WeakClassifier;


//Weak classifier list with weight, part of a strong classifier
typedef struct ClassifierList
{
	WeakClassifier class;
	double weight;
	struct ClassifierList *next;
} ClassifierList;


//strong classifier, used in cascading
typedef struct StrongClassifier
{
	ClassifierList *weak;
	float totalThreshold;
	float falseNegative;
	int weakNumber;
} StrongClassifier;




typedef struct SCList
{
	StrongClassifier sc;
	struct SCList *next;
} SCList;




//only used in the training part
typedef struct TrainingList
{
	struct TrainingList *next;
	Matrix m;
	int positive;
	float weight;
} TrainingList;


struct ThreadData
{
	struct TrainingList *l;
	int *lock;
	int *nbThread;
	float *bestScore;
	WeakClassifier *wc;
	int x;
};


struct TrainingElement
{
	float val;
	int positive;
	float weight;
};


TrainingList* loadExamples(char *positiveFolder, char* negativeFolder);

int twc(WeakClassifier wc, Matrix *m, int *x, int *y);
int testWeakClassifier(WeakClassifier wc, Matrix *m);
int testStrong(Matrix *m, StrongClassifier *sc);
double testAllExamples(TrainingList *l, WeakClassifier wc);

WeakClassifier getBestClassifier(TrainingList *l, float *score);

SCList *adaboost(char* positiveFolder, char* negativeFolder,
		int nb);

//SCList* learning(char *positiveFolder, char *negativeFolder, int nb);

void freeStrongClassifier(StrongClassifier* sc);
void freeSCList(SCList* l);

#endif
