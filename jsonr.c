#include "json.h"
#include "adaboost.h"

char* readJson()
{
	FILE* file = NULL;
	file = fopen("training.json", "r");

	if (file == NULL)
	{
		return NULL;
	}
	else
	{
		fseek(file, 0, SEEK_END);
		long fsize = ftell(file);
		fseek(file, 0, SEEK_SET);

		char *string = malloc(fsize + 1);
		fread(string, fsize, 1, file);
		fclose(file);

		string[fsize] = 0;

		return string;
	}
}


SCList* dbExtract()
{
	char* str = readJson();

	if (str == NULL)
	{
		printf("error: can not read training database\n");
		// HERE IS A EXIT(0) JUST HERE !!! IN CASE OF BUG, REMOVE IT !!!
		exit(0);
	}
	else
	{
		json_object *jobj = json_tokener_parse(str);
		return jsonToStrongClassifierList(jobj);
	}
}


SCList* jsonToStrongClassifierList(json_object* jStrongList)
{
	int len = json_object_array_length(jStrongList);

	SCList *old = NULL;

	for (int i = len - 1; i >= 0; i--)
	{
		json_object *jStrongElt;
		jStrongElt = json_object_array_get_idx(jStrongList, i);

		SCList *strongList = malloc(sizeof(SCList));;

		strongList->sc = jsonToStrongClassifier(jStrongElt);
		strongList->next = old;

		old = strongList;
	}

	return old;

}


StrongClassifier jsonToStrongClassifier(json_object *jStrongClass)
{
	StrongClassifier strongClass;

	json_object *jWeakL, *jTotalT, *jFalseNegative, *jWeakNumber;
	json_object_object_get_ex(jStrongClass, "weak_classifier_list", &jWeakL);
	json_object_object_get_ex(jStrongClass, "total_threshold", &jTotalT);
	json_object_object_get_ex(jStrongClass, "false_negative", &jFalseNegative);
	json_object_object_get_ex(jStrongClass, "weak_number", &jWeakNumber);

	strongClass.weak = jsonToWeakClassifierList(jWeakL);
	strongClass.totalThreshold = json_object_get_double(jTotalT);
	strongClass.falseNegative = json_object_get_double(jFalseNegative);
	strongClass.weakNumber = json_object_get_int(jWeakNumber);

	return strongClass;
}


ClassifierList* jsonToWeakClassifierList(json_object *jWeakList)
{
	int len = json_object_array_length(jWeakList);

	ClassifierList *old = NULL;

	for (int i = len - 1; i >= 0; i--)
	{
		json_object *jWeakElt, *jWeight, *jWeakClass;
		jWeakElt = json_object_array_get_idx(jWeakList, i);

		json_object_object_get_ex(jWeakElt, "weight", &jWeight);
		json_object_object_get_ex(jWeakElt, "weak_classifier", &jWeakClass);



		ClassifierList *weakList = malloc(sizeof(ClassifierList));;
		weakList->weight = json_object_get_double(jWeight);

		weakList->class = jsonToWeakClassifier(jWeakClass);
		weakList->next = old;

		old = weakList;
	}

	return old;
}


WeakClassifier jsonToWeakClassifier(json_object *jWeakClass)
{
	WeakClassifier weakClass;

	json_object *jXmin, *jYmin, *jXmax, *jYmax, *jType, *jParity, *jThreshold;
	json_object_object_get_ex(jWeakClass, "xmin", &jXmin);
	json_object_object_get_ex(jWeakClass, "ymin", &jYmin);
	json_object_object_get_ex(jWeakClass, "xmax", &jXmax);
	json_object_object_get_ex(jWeakClass, "ymax", &jYmax);

	json_object_object_get_ex(jWeakClass, "type", &jType);
	json_object_object_get_ex(jWeakClass, "parity", &jParity);
	json_object_object_get_ex(jWeakClass, "threshold", &jThreshold);


	weakClass.xmin = json_object_get_int(jXmin);
	weakClass.ymin = json_object_get_int(jYmin);
	weakClass.xmax = json_object_get_int(jXmax);
	weakClass.ymax = json_object_get_int(jYmax);

	weakClass.type = json_object_get_int(jType);
	weakClass.parity = json_object_get_int(jParity);
	weakClass.threshold = json_object_get_double(jThreshold);

	return weakClass;
}

