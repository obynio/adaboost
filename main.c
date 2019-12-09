#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <time.h>

#include "database.h"
#include "interface.h"
#include "adaboost.h"
#include "json.h"
#include "cascading.h"
#include "utils.h"
#include "pca.h"

int main(int argc, char *argv[])
{
	if (argc < 2)
		startinterface (argc, argv);
	else
		printf("error: invalid option '%s'\n", argv[1]);

	return 0;
}


