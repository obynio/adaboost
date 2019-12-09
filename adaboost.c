#include "adaboost.h"
#include "json.h"

#define MAX 5000


TrainingList* loadSingleFolder(char * folder, int positive, int *nb)
{
	char buffer[200];
	int i=1;
	SDL_Surface *img = NULL;
	TrainingList *l=NULL, *next=NULL;

	DIR           *d;
	struct dirent *dir;
	d = opendir(folder);

	if(!d)
		return l;

	while(i<=MAX && (dir = readdir(d))!=NULL)
	{
		sprintf(buffer, "%s%s", folder, dir->d_name);
		//printf("%s\n",buffer);
		img=IMG_Load(buffer);
		if(img==NULL)
		{
			continue;
		}

		*nb=1+*nb;
		i++;

		next=l;
		l=malloc(sizeof(TrainingList));
		l->next=next;
		l->positive=positive;

		imgToMatrix(img, &l->m);
		toIntegral(&l->m);
		SDL_FreeSurface(img);
	}
	closedir(d);
	return l;
}


TrainingList* loadExamples(char *positiveFolder, char* negativeFolder)
{
	int nb=0;
	TrainingList *l2=loadSingleFolder(negativeFolder, 0, &nb);
	TrainingList *l1=loadSingleFolder(positiveFolder, 1, &nb);
	TrainingList *l=NULL;

	printf("loaded %d images\n", nb);

	if(l1)
	{
		l=l1;
		while(l->next)
		{
			l->weight=1.0/nb;
			l=l->next;
		}
		l->weight=1.0/nb;
		l->next=l2;
	}
	else
	{
		l1=l2;
	}
	while(l2)
	{
		l2->weight=1.0/nb;
		l2=l2->next;
	}

	return l1;

}


void freeTrainingList(TrainingList *l)
{
	TrainingList *next;
	while(l)
	{
		next=l->next;
		freeMatrix(&l->m);
		free(l);
		l=next;
	}
}

void freeStrongClassifier(StrongClassifier *sc)
{
	ClassifierList* current = sc->weak, *next;
	while(current)
	{
		next=current->next;
		free(current);
		current=next;
	}
}


int twc(WeakClassifier wc, Matrix *m, int *x, int *y)
{
	float haarVal = relativeHaar(m, wc.type, wc.xmin + *x, wc.ymin + *y,
			wc.xmax + *x, wc.ymax + *y);
	return haarVal*wc.parity>=wc.threshold;
}

int testWeakClassifier(WeakClassifier wc, Matrix *m)
{
	float haarVal = relativeHaar(m, wc.type, wc.xmin, wc.ymin, wc.xmax,
			wc.ymax);
	//printf("wc=%f/%f\n",haarVal*wc.parity, wc.threshold);
	return haarVal*wc.parity>=wc.threshold;
}

double testAllExamples(TrainingList *l, WeakClassifier wc)
{
	double r=0.0, t=0.0;
	while(l!=NULL)
	{
		r += testWeakClassifier(wc, &l->m) == (l->positive!=0) ?
			l->weight : 0.0;
		t += l->weight;
		l=l->next;
	}
	return r/t;
}



static inline void swap(struct TrainingElement *a, struct TrainingElement *b)
{
	struct TrainingElement c=*a;
	*a=*b;
	*b=c;
}
struct TrainingElement* partition(struct TrainingElement* begin,
								  struct TrainingElement *end, struct TrainingElement *pivot)
{
	float pval=pivot->val;
    swap(pivot, end-1);
    pivot=begin;
    for(struct TrainingElement* i=begin; i<=end-2; i++)
    {
		if(i->val<pval)
		{
			swap(pivot, i);
			pivot=pivot+1;
		}
    }
    swap(pivot, end-1);
    return pivot;	
}
void quickSort(struct TrainingElement* begin, struct TrainingElement *end)
{
    if(end-begin>1)
    {
		struct TrainingElement *pivot = begin;
		pivot = partition(begin, end, pivot);
		quickSort(begin, pivot);
		quickSort(pivot+1, end);
    }
}


static inline float min(float x, float y)
{
	return x>y ? y : x;
}

double getBestThreshold(WeakClassifier* wc, TrainingList* l)
{
	struct TrainingElement t[2*MAX];
	int total=0;
	float totalP=0, totalN=0;
	wc->parity=1;
	while(l)
	{
		t[total].positive=l->positive;
		t[total].weight=l->weight;
		t[total].val=relativeHaar(&l->m, wc->type, wc->xmin, wc->ymin,
								   wc->xmax, wc->ymax);
		if(l->positive)
			totalP+=l->weight;
		else
			totalN+=l->weight;
		l=l->next;
		total++;
	}
	quickSort(t,t+total);
   
	
	double bestScore=0.5;
	int bestPosition=0;
	double score=totalP;
	for(int i=0; i<total; i++)
	{
		score+=t[i].weight * (t[i].positive?-1:1);

		if(fabs(0.5-score)>fabs(0.5-bestScore))
		{
			bestScore=score;
			bestPosition=i;
		}
	}

	if(bestPosition==total-1)
		wc->threshold=t[bestPosition].val;
	else
		wc->threshold=(t[bestPosition].val+t[bestPosition+1].val)/2;
	//printf("%f ",wc->threshold);
	
	
	if(bestScore<0.5)
	{
		bestScore=1-bestScore;
		wc->threshold*=-1;
		wc->parity=-1;
	}

	return bestScore;
}

void normalizeTrainingWeight(TrainingList* l)
{
	double total=0.0;
	for(TrainingList* i=l; i; i=i->next)
	{
		total+=i->weight;
	}
	for(TrainingList* i=l; i; i=i->next)
	{
		i->weight/=total;
	}
}


void setTrainingWeight(WeakClassifier wc, TrainingList* l, double reweight)
{
	for(TrainingList* i=l; i; i=i->next)
	{
		double incorrect= testWeakClassifier(wc, &i->m)
			== (i->positive!=0) ? -1.0 : 1.0;
		i->weight*=exp(reweight*incorrect);
		//if(i->weight<=0.0001)
		//	i->weight=0.0001;
	}
	normalizeTrainingWeight(l);
	//printf("%a\n",l->weight);
}




void *thread(void* d)
{
	struct ThreadData *data = (struct ThreadData*)d;
	TrainingList *l=data->l;
	float newScore=0, bestScore=0;
	WeakClassifier best;

	WeakClassifier new;
	new.xmin=data->x;
	
	for(new.type=1; new.type<=4; new.type++)
	{
		for(new.ymin=0; new.ymin<24; new.ymin++)
		{
			for(new.ymax=new.ymin+2; new.ymax<24; new.ymax++)
			{
				for(new.xmax=new.xmin+2; new.xmax<24; new.xmax++)
				{
					newScore=getBestThreshold(&new, l);
					if(newScore>bestScore)
					{
						bestScore=newScore;
						best=new;
					}
				}
			}
		}
		
	}
	while(*data->lock);
	*data->lock=1;
	if(bestScore>*data->bestScore)
	{
		*data->wc=best;
		*data->bestScore=bestScore;
	}
	*data->lock=0;
	(*data->nbThread)--;
	free(d);
	return NULL;
}











WeakClassifier getBestClassifier(TrainingList *l, float *score)
{
	WeakClassifier currentBest;
	float bestScore=0;
	int lock=0, nbThread=0;
	

	for(int x=0; x<23; x++)
	{
		nbThread++;


		struct ThreadData *data = malloc(sizeof(struct ThreadData));
		data->l=l;
		data->lock=&lock;
		data->nbThread=&nbThread;
		data->wc=&currentBest;
		data->bestScore=&bestScore;
		data->x=x;

		pthread_t th;
		pthread_create(&th, NULL, thread, data);
		
	}
	while(nbThread>0)
	{
		//printf("%d\n",nbThread);
		SDL_Delay(10);
	}

	*score=bestScore;
	return currentBest;
}


int testStrong(Matrix *m, StrongClassifier *sc)
{
	double total=0.0;
	ClassifierList* l=sc->weak;
	while(l)
	{
		total+=testWeakClassifier(l->class, m)?l->weight:0.0;
		l=l->next;
	}
	return total>=sc->totalThreshold;
}


double testFalseNegative(StrongClassifier *sc, TrainingList *l)
{
	int found=0, total=0;
	while(l)
	{
		if(l->positive!=0)
		{
			if(!testStrong(&l->m, sc))
				found++;
			total++;
		}
		l=l->next;
	}
	//printf("False nefative = %d/%d\n", found, total);
	return (float)found/(float)total;
}


void setThreshold(StrongClassifier *sc, TrainingList* l, float falseNegative, FILE* logfile)
{
	printf("finding best total threshold\n");
	float totalWeight=0.0;
	ClassifierList *class=sc->weak;
	while(class)
	{
		totalWeight+=class->weight;
		class=class->next;
	}



	sc->totalThreshold=0.0;

	
	int total=0, correct=0;
	int totalP=0, correctP=0;
	int totalN=0, correctN=0;
	float fneg=0, fpos=0;
	float bestThreshold=0;
	float bestScore=0;
	do
	{
		sc->totalThreshold+=totalWeight/50.0;

		for(TrainingList *i=l; i; i=i->next)
		{
			if(testStrong(&i->m, sc)==i->positive)
				correct++;
			total++;
			if(i->positive)
			{
				if(testStrong(&i->m, sc))
					correctP++;
				totalP++;
			}
			else
			{
				if(!testStrong(&i->m, sc))
					correctN++;
				totalN++;
			}
		}
		fneg = (float)(totalP-correctP)/(float)totalP;
		fpos = (float)(totalN-correctN)/(float)totalN;
		float score=(1-fneg)+(1-fpos);
		if(score>bestScore)
		{
			bestScore=score;
			bestThreshold=sc->totalThreshold;
		}
		
		//printf("%f\n",fneg);
	}
	while(fneg<=falseNegative
		  && sc->totalThreshold<=totalWeight
		  && correctN<totalN);
	sc->totalThreshold=bestThreshold;
	printf("best threshold=%f/%f\n", sc->totalThreshold, totalWeight);
	//sc->falseNegative=testFalseNegative(sc, l);

	for(TrainingList *i=l; i; i=i->next)
	{
		if(testStrong(&i->m, sc)==i->positive)
			correct++;
		total++;
		if(i->positive)
		{
			if(testStrong(&i->m, sc))
				correctP++;
			totalP++;
		}
		else
		{
			if(!testStrong(&i->m, sc))
				correctN++;
			totalN++;
		}
	}
		
	printf("Overall negative success = %f\n", (float)correctN/(float)totalN);
	printf("Overall positive success = %f\n\n", (float)correctP/(float)totalP);
	printf("Overall success = %f\n", (float)correct/(float)total);


	fprintf(logfile, "Overall negative success = %f\n", (float)correctN/(float)totalN);
	fprintf(logfile, "Overall positive success = %f\n\n", (float)correctP/(float)totalP);
	fprintf(logfile, "Overall success = %f\n", (float)correct/(float)total);

}


SCList *adaboost(char *positiveFolder, char* negativeFolder, int nb)
{
	
	FILE *logfile=fopen("log.txt","w");

	SCList *sclist = NULL;
	float jump=0.2/nb;
	TrainingList *training = loadExamples(positiveFolder, negativeFolder);
	StrongClassifier strong;
	strong.weakNumber=0;
	float score=0.0;
	strong.weak=NULL;

	for(int j=0; j<nb; j++)
	{
		
		unsigned long totalStart=SDL_GetTicks();

		for(int i=0; i<10*j+5; i++)
		{
			strong.weakNumber++;
			unsigned long start=SDL_GetTicks();
			ClassifierList *new=malloc(sizeof(ClassifierList));
			WeakClassifier wc = getBestClassifier(training, &score);
			new->class=wc;
		
			printf("(%2d,%2d) (%2d,%2d)", wc.xmin, wc.ymin, wc.xmax, wc.ymax);
			printf(" %d %2d %f = %f\t\t|  ", wc.type, wc.parity, wc.threshold, score);
			printf("found in %fs\n", ((double)(SDL_GetTicks()-start))/1000);

			fprintf(logfile, "(%d,%d) (%d,%d)", wc.xmin, wc.ymin, wc.xmax, wc.ymax);
			fprintf(logfile, " %d %d %f = %f\t\t|  ", wc.type, wc.parity, wc.threshold, score);
			fprintf(logfile, "found in %fs\n", ((double)(SDL_GetTicks()-start))/1000);

			new->weight=score>=0.999f?1:0.5f*log(score/(1.0-score));
			setTrainingWeight(new->class, training, new->weight);

		

			new->next=strong.weak;
			strong.weak=new;
		}
		setThreshold(&strong, training, ((float)j)*jump, logfile);
		
		printf("\nsc found in %fs\n\n\n",
			((double)(SDL_GetTicks()-totalStart))/1000);

		dbInsert(&strong);
		SCList *next=sclist;
		sclist=malloc(sizeof(SCList));
		sclist->next=next;
		sclist->sc=strong;

		
	}

	

	freeTrainingList(training);
	fclose(logfile);

	return sclist;
}



void freeSCList(SCList* l)
{
	return;
	while(l!=NULL)
	{
		//freeStrongClassifier(&l->sc);
		SCList* aux=l->next;
		free(l);
		l=aux;
	}
}
