#ifndef DATABASE
#define DATABASE

#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct personList
{
	char* name;
	char* imagePath;
	GtkWidget* label;
	struct personList* next;
} personList;

personList* database;


GtkWidget* addPerson (const char* name);
void modifyName (GtkWidget* label, const char* name);
void modifyImg (GtkWidget* label, const char* imagePath);
void deletePerson (GtkWidget* label);

char* GetImgPath (GtkWidget* label);

#endif
