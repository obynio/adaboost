#include "json.h"

int writeJson(json_object* json)
{
	FILE* file = NULL;
	file = fopen("training.json", "w");

	if (file == NULL)
	{
		return 0;
	}

	//fseek(file, 0, SEEK_END);
	fprintf(file, "%s\n", json_object_to_json_string(json));

	fclose(file);
	return 1;
}


void dbErase()
{
	FILE* file = NULL;
	file = fopen("training.json", "r");

	if (file != NULL)
	{
		fclose(file);
		printf("removing previous database\n");
		remove("training.json");
	}
}


void dbInsert(StrongClassifier *strongClass)
{
	char *str = readJson();

	if (str == NULL)
	{
		// No existing database

		SCList list;
		list.sc = *strongClass;
		list.next = NULL;

		writeJson(strongClassifierListToJson(&list));
	}
	else
	{
		// Existing database

		json_object *jobj = json_tokener_parse(str);
		json_object_array_add(jobj, strongClassifierToJson(strongClass));

		if(!writeJson(jobj))
			printf("error: can not write training database\n");
	}
}


json_object* weakClassifierToJson(WeakClassifier *weakClass)
{
	json_object *jnode = json_object_new_object();

	json_object *jxmin = json_object_new_int(weakClass->xmin);
	json_object *jymin = json_object_new_int(weakClass->ymin);
	json_object *jxmax = json_object_new_int(weakClass->xmax);
	json_object *jymax = json_object_new_int(weakClass->ymax);

	json_object *jtype = json_object_new_int(weakClass->type);
	json_object *jparity = json_object_new_int(weakClass->parity);
	json_object *jthreshold = json_object_new_double(weakClass->threshold);

	json_object_object_add(jnode, "xmin", jxmin);
	json_object_object_add(jnode, "ymin", jymin);
	json_object_object_add(jnode, "xmax", jxmax);
	json_object_object_add(jnode, "ymax", jymax);

	json_object_object_add(jnode, "type", jtype);
	json_object_object_add(jnode, "parity", jparity);
	json_object_object_add(jnode, "threshold", jthreshold);

	return jnode;
}


json_object* classifierListToJson(ClassifierList *classList)
{
	json_object *jweakList = json_object_new_array();

	while (classList != NULL)
	{
		// New children for jweakList
		json_object *jweakListChildren = json_object_new_object();

		// Adding the weak classifier to the child
		json_object *jweakC = weakClassifierToJson(&classList->class);
		json_object_object_add(jweakListChildren, "weak_classifier", jweakC);

		// Adding the weight to the child
		json_object *jweight = json_object_new_double(classList->weight);
		json_object_object_add(jweakListChildren, "weight", jweight);

		// Add the children to the array of trees
		json_object_array_add(jweakList, jweakListChildren);

		// Move forward the next ClassifierList
		classList = classList->next;
	}

	return jweakList;
}


json_object* strongClassifierToJson(StrongClassifier *strongClass)
{
	json_object *jnode = json_object_new_object();

	json_object *jweakList = classifierListToJson(strongClass->weak);
	json_object *jtotalT = json_object_new_double(strongClass->totalThreshold);
	json_object *jfalseN = json_object_new_double(strongClass->falseNegative);
	json_object *jweakNumber = json_object_new_int(strongClass->weakNumber);

	json_object_object_add(jnode, "weak_classifier_list", jweakList);
	json_object_object_add(jnode, "total_threshold", jtotalT);
	json_object_object_add(jnode, "false_negative", jfalseN);
	json_object_object_add(jnode, "weak_number", jweakNumber);

	return jnode;
}


json_object* strongClassifierListToJson(SCList *classList)
{
	json_object *jstrongList = json_object_new_array();

	while (classList != NULL)
	{
		// Add the children to the array of trees
		json_object *jstrongClass = strongClassifierToJson(&classList->sc);
		json_object_array_add(jstrongList, jstrongClass);

		// Move forward the next ClassifierList
		classList = classList->next;
	}

	return jstrongList;
}

