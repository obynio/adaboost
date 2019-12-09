#include "pca.h"

Matrix* Add (Matrix* m1, Matrix* m2) //m1 is destination
{
	for (int i = 0; i < m1->x; i++)
	{
		for (int j = 0; j < m1->y; j++)
		{
			m1->array[i][j] = m1->array[i][j] + m2->array[i][j];
		}
	}
	return m1;
}

Matrix* Substract (Matrix* m1, Matrix* m2) //m1 is destination
{
	for (int i = 0; i < m1->x; i++)
	{
		for (int j = 0; j < m1->y; j++)
		{
			m1->array[i][j] = m1->array[i][j] - m2->array[i][j];
		}
	}
	return m1;
}

Matrix* Multiply (Matrix* m1, Matrix* m2)
{
	Matrix* newm = malloc(sizeof(Matrix));
	newm->x = m1->y;
	newm->y = m2->x;
	mallocMatrix(newm);

	for (int i = 0; i < newm->x; i++)
	{
		for (int j = 0; j < newm->y; j++)
		{
			newm->array[i][j] = 0;
			for (int k = 0; k < m1->x; k++)
			{
				newm->array[i][j] += m1->array[k][j] * m2->array[i][k];
			}
		}
	}
	return newm;
}

Matrix* Transpose (Matrix* m)
{
	Matrix* newm = malloc(sizeof(Matrix));
	newm->x = m->y;
	newm->y = m->x;
	mallocMatrix(newm);
	for (int i = 0; i < m->x; i++)
	{
		for (int j = 0; j < m->y; j++)
		{
			newm->array[j][i] = m->array[i][j];
		}
	}
	return newm;
}

Matrix* MeanMatrix (Matrix** m, int len)
{
	Matrix* newm = malloc(sizeof(Matrix));
	newm->x = m[0]->x;
	newm->y = m[0]->y;
	mallocMatrix(newm);
	for (int i = 0; i < newm->x; i++) //set to zero
	{
		for (int j = 0; j < newm->y; j++)
		{
			newm->array[i][j] = 0;
		}
	}

	for (int i = 0; i < len; i++) //addition of every matrices
	{
		Add(newm, m[i]);
	}

	for (int i = 0; i < newm->x; i++) //division for getting mean matrix
	{
		for (int j = 0; j < newm->y; j++)
		{
			newm->array[i][j] =  newm->array[i][j] / len;
		}
	}
	return newm;
}

Matrix** Normalize (Matrix** m, int len, Matrix* mean) //m matrices are destination
{
	for (int i = 0; i < len; i++)
	{
		Substract(m[i], mean);
	}
	return m;
}

Matrix* ToGlobalMeanMatrix (Matrix** m, int len) //here we put the list of mean matrices into one
{
	Matrix* newm = malloc(sizeof(Matrix));
	newm->x = len;
	newm->y = m[0]->x * m[0]->y; //each column is a matrix
	mallocMatrix(newm);
	for (int i = 0; i < len; i++)
	{
		for (int j = 0; j < m[0]->y; j++)
		{
			for (int k = 0; k < m[0]->x; k++)
			{
				newm->array[i][j*k + k] = m[i]->array[k][j];
			}
		}
	}
	return newm;
}

Matrix* Covariance (Matrix** m, int len)
{
	Matrix* newm = malloc(sizeof(Matrix));
	newm->x = len;
	newm->y = m[0]->x * m[0]->y; //each column is a matrix
	mallocMatrix(newm);

	Matrix* meanMatrix = MeanMatrix (m, len);
	Normalize (m, len, meanMatrix);

	Matrix* globalMatrix = ToGlobalMeanMatrix(m, len);
	Matrix* transposedGlobalMatrix = Transpose(globalMatrix);

	Matrix* covarianceMatrix = Multiply(transposedGlobalMatrix, globalMatrix);

	freeMatrix(meanMatrix);
	free(meanMatrix);
	freeMatrix(globalMatrix);
	free(globalMatrix);
	freeMatrix(transposedGlobalMatrix);
	free(transposedGlobalMatrix);
	for(int i = 0; i < len; i++)
	{
		freeMatrix(m[i]);
		free(m[i]);
	}
	return covarianceMatrix;
}

int Distance (unsigned long* vec1, unsigned long* vec2, int len)
{
	double dist = 0;
	for(int i=0; i<len; i++)
	{
		double sub = (double)vec1[i] - (double)vec2[i];
		dist = dist + pow(sub, 2);
	}
	dist = pow(dist, 0.5);
	return dist;
}

int getDirCount(char *folder) 
{
	DIR *dir;
	struct dirent *dirent;
	int count = 0;

	dir = opendir(folder);

	if (!dir)
	{
		printf("error: can not open directory\n");
		exit(0);
	}

	while ((dirent = readdir(dir)) != NULL) 
	{
		if (strstr(dirent->d_name, ".pgm") || strstr(dirent->d_name, ".png") || strstr(dirent->d_name, ".jpg"))
			count++;
	}

	closedir(dir);
	return count;
}

void dirToMatrix(char *folder, Matrix *m) 
{
	DIR *dir;
	struct dirent *dirent;
	int count = 0;
	char buffer[200];

	dir = opendir(folder);

	if (!dir)
	{
		printf("error: can not open directory\n");
		exit(0);
	}


	while ((dirent = readdir(dir)) != NULL) 
	{
		// fucking . and ..
		if (!strstr(dirent->d_name, ".pgm") && !strstr(dirent->d_name, ".png") && !strstr(dirent->d_name, ".jpg"))
			continue;

		sprintf(buffer, "%s%s", folder, dirent->d_name);

		if (!count)
		{
			// Allocating a new matrix. 
			// One collumn contain an entire image.
			// Each collum represent an image, pixel by pixel

			// This is img1
			//
			// |--------|--------|
			// | pixel1 | pixel3 |
			// |--------|--------|
			// | pixel2 | pixel4 |
			// |--------|--------|

			// This is our result matrix
			// 
			// |==============|==============|=============|
			// | img1 pixel 1 | img2 pixel 1 | img3 pixel1 |
			// |==============|==============|=============|
			// | img1 pixel 2 | img2 pixel 2 | img3 pixel2 |
			// |==============|==============|=============|
			// | img1 pixel 3 | img2 pixel 3 | img3 pixel3 |
			// |==============|==============|=============|

			// WARNING : All training images must be exactly the same size !

			SDL_Surface* img = IMG_Load(buffer);

			int fileNumber = getDirCount(folder);

			m->x = fileNumber;
			m->y = img->w * img->h;

			mallocMatrix(m);

			SDL_FreeSurface(img);
		}

		Matrix bis; // Square matrix here, need to convert it to my new dir matrix
		getGreyImage(buffer, &bis);

		for (int my = 0; my < m->y; my++)
		{
			m->array[count][my] = bis.array[my / bis.y][my % bis.y];	
		}

		// Next file
		count++;
	}

	/*
	   for (int i = 0; i < m->y; i++)
	   {
	   printf("%d\n", m->array[0][i]);
	   }
	   */

	closedir(dir);
}
