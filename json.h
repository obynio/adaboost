#include <stdlib.h>
#include <stdio.h>
#include <json.h>
#include "adaboost.h"

#ifndef JSON_H_
#define JSON_H_

// jsonw.c

int writeJson(json_object* json);
void dbErase();
void dbInsert(StrongClassifier *strongClass);

json_object* weakClassifierToJson(WeakClassifier *weakClass);
json_object* classifierListToJson(ClassifierList *classList);
json_object* strongClassifierToJson(StrongClassifier *strongClass);
json_object* strongClassifierListToJson(SCList *classList);

// jsonr.c

char* readJson();
SCList* dbExtract();
SCList* jsonToStrongClassifierList(json_object* jStrongList);
StrongClassifier jsonToStrongClassifier(json_object *jStrongClass);
ClassifierList* jsonToWeakClassifierList(json_object *jWeakList);
WeakClassifier jsonToWeakClassifier(json_object *jWeakClass);

#endif
