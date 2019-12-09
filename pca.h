#ifndef _EIGENOPS_H_
#define _EIGENOPS_H_

#include <stdio.h>
#include <math.h>
#include <dirent.h>

#include "SDLToMatrix.h"

Matrix* Add (Matrix* m1, Matrix* m2);
Matrix* Substract (Matrix* m1, Matrix* m2);
Matrix* Multiply (Matrix* m1, Matrix* m2);
Matrix* Transpose (Matrix* m);

void dirToMatrix(char *folder, Matrix *m);

#endif
