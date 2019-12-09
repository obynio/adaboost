#include "database.h"

void writeToFile()
{
	FILE *f = fopen("file.txt", "w");
	personList* list = database;
	int nb = 1;
	while(list != NULL)
	{
		fprintf(f, "Name number %d: %s\n", nb, (*list).name);
		list = list->next;
		nb++;
	}
}

GtkWidget* addPerson (const char* name)
{

	personList* newPerson = malloc(sizeof(personList));
	newPerson->name =  malloc(strlen(name) * sizeof(char));
	strcpy(newPerson->name, name);
	newPerson->imagePath =  malloc(11 * sizeof(char));
	strcpy(newPerson->imagePath, "default.png");
	newPerson->label = gtk_label_new(name);
	newPerson->next = database;

	database = newPerson;

	return newPerson->label;

}



void modifyName (GtkWidget* label, const char* name)
{
	personList* list = database;
	while(list != NULL)
	{
		if(list->label == label)
		{
			free(list->name);
			list->name = malloc(strlen(name) * sizeof(char *));
			strcpy(list->name, name);
			break;
		}
		list = list->next;
	}
}

void modifyImg (GtkWidget* label, const char* imagePath)
{
	personList* list = database;
	while(list != NULL)
	{
		if(list->label == label)
		{
			free(list->imagePath);
			list->imagePath = malloc(strlen(imagePath) * sizeof(char *));
			strcpy(list->imagePath, imagePath);
			break;
		}
		list = list->next;
	}
}

void deletePerson (GtkWidget* label)
{
	personList* list = database;
	while(list != NULL)
	{
		if(list->label == label)
		{
			personList* tmp = database;
			database = list->next;
			free(tmp);
			break;
		}
		if(list->next->label == label)
		{
			personList* tmp = list->next;
			list->next = list->next->next;
			free(tmp);
			break;
		}

		list = list->next;
	}
}

char* GetImgPath (GtkWidget* label)
{
	personList* list = database;
	while(list != NULL)
	{
		if(list->label == label)
		{
			return list->imagePath;
		}
		list = list->next;
	}
	return "";
}
