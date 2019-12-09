#include <gtk/gtk.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "database.h"
#include "utils.h"
#include "interface.h"

GtkWidget *window; //Window
GtkWidget *notebook; //Notebook

//// First tab /////
GtkWidget *database_tab;
GtkWidget *listbox;
GtkWidget *image;
GtkWidget *entry;
GtkWidget *buttons_box;
GtkWidget *add_button;
GtkWidget *modifyName_button;
GtkWidget *modifyImg_button;
GtkWidget *delete_button;
GtkWidget *dialog;
///////////////////

//// Second tab /////
GtkWidget *facerecognition_tab;
GtkWidget *widgets_box;
GtkWidget *changeImg_button;
GtkWidget *evaluate_button;
GtkWidget *image2;
GtkWidget *result_label;
///////////////////

SDL_Surface* surface;
SDL_Surface* surface2;


static void LoadImage (const char* path, int nb, GtkWidget* img)
{
	SDL_Surface* holder;
	if(nb)
	{
		SDL_FreeSurface(surface2);
		surface2 = IMG_Load(path);
		holder = surface2;
	}
	else
	{
		SDL_FreeSurface(surface);
		surface = IMG_Load(path);
		holder = surface;
	}
	if (holder != NULL)
	{
		GdkPixbuf* pixbuffer;
		if (holder->format->BitsPerPixel == 24)
		{
			pixbuffer = gdk_pixbuf_new_from_data(holder->pixels,
					GDK_COLORSPACE_RGB, FALSE, 8, holder->w, holder->h,
					holder->pitch, NULL, NULL);
		}
		else
		{
			pixbuffer = gdk_pixbuf_new_from_data(holder->pixels,
					GDK_COLORSPACE_RGB, TRUE, 8, holder->w, holder->h,
					holder->pitch, NULL, NULL);
		}
		gtk_image_set_from_pixbuf(GTK_IMAGE(img), pixbuffer);
	}
}

static void selected()
{
	g_print ("\nSelecting an other person");
	GtkListBoxRow *row = gtk_list_box_get_selected_row
		(GTK_LIST_BOX(listbox));
	GtkWidget * lab = gtk_container_get_children
		(GTK_CONTAINER (row))->data;
	gtk_entry_set_text(GTK_ENTRY(entry),
		gtk_label_get_text(GTK_LABEL(lab)));
	LoadImage(GetImgPath(lab), 0, image);
}

static void add_name()
{
	if (gtk_entry_get_text(GTK_ENTRY(entry))[0] != '\0')
	{
		g_print ("\nAdding name");
		GtkWidget* lab = addPerson(gtk_entry_get_text(GTK_ENTRY(entry)));
		GtkWidget* row = gtk_list_box_row_new();
		gtk_container_add (GTK_CONTAINER (row), lab);
		gtk_list_box_prepend (GTK_LIST_BOX (listbox), row);
		gtk_widget_show_all (window);
	}
}

static void modify_name()
{
	GtkListBoxRow *row = gtk_list_box_get_selected_row (GTK_LIST_BOX(listbox));
	if(row != NULL)
	{
		g_print ("\nModifying name");
		GtkWidget * lab = gtk_container_get_children (GTK_CONTAINER (row))->
			data;
		modifyName (lab, gtk_entry_get_text(GTK_ENTRY(entry)));
		gtk_label_set_text(GTK_LABEL(lab), gtk_entry_get_text
				(GTK_ENTRY(entry)));
	}
}

static void modify_image()
{
	GtkListBoxRow *row = gtk_list_box_get_selected_row
		(GTK_LIST_BOX(listbox));
	if (row != NULL)
	{
	g_print ("\nModifying image");
	dialog = gtk_file_chooser_dialog_new ("Open File",
			GTK_WINDOW(window),
			GTK_FILE_CHOOSER_ACTION_OPEN,
			"_Cancel",
			GTK_RESPONSE_CANCEL,
			"_Open",
			GTK_RESPONSE_ACCEPT,
			NULL);
	gint res = gtk_dialog_run (GTK_DIALOG (dialog));
	if (res == GTK_RESPONSE_ACCEPT)
	{
		char *filename;
		GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
		filename = gtk_file_chooser_get_filename (chooser);
		g_print ("\nImage selected at : %s", filename);
		GtkWidget * lab = gtk_container_get_children
			(GTK_CONTAINER (row))->data;
		modifyImg(lab, filename);
		LoadImage(filename, 0, image);
	}
	else
	{
		g_print ("\nSelectection failed");
	}
	gtk_widget_destroy (dialog);
	}
}

static void delete_image()
{
	GtkListBoxRow *row = gtk_list_box_get_selected_row (GTK_LIST_BOX(listbox));
	if(row != NULL)
	{
		g_print ("\nDeleting name");
		GtkWidget* lab = gtk_container_get_children (GTK_CONTAINER (row))->data;
		deletePerson (lab);
		gtk_container_remove(GTK_CONTAINER (listbox), GTK_WIDGET(row));
	}
}

static void change_image()
{
	g_print ("\nChoosing image");
	dialog = gtk_file_chooser_dialog_new ("Open File",
			GTK_WINDOW(window),
			GTK_FILE_CHOOSER_ACTION_OPEN,
			"_Cancel",
			GTK_RESPONSE_CANCEL,
			"_Open",
			GTK_RESPONSE_ACCEPT,
			NULL);
	gint res = gtk_dialog_run (GTK_DIALOG (dialog));
	if (res == GTK_RESPONSE_ACCEPT)
	{
		char *filename;
		GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
		filename = gtk_file_chooser_get_filename (chooser);
		g_print ("\nImage selected at : %s", filename);
		LoadImage(filename, 1, image2);
	}
	else
	{
		g_print ("\nSelectection failed");
	}
	gtk_widget_destroy (dialog);
	gtk_label_set_text(GTK_LABEL(result_label), "En attente d'évaluation ...");
}

static void evaluate()
{
	if (surface2 != NULL)
	{
		int result = detect2(surface2);

		GdkPixbuf* pixbuffer;
		if (surface2->format->BitsPerPixel == 24)
		{
			pixbuffer = gdk_pixbuf_new_from_data(surface2->pixels,
					GDK_COLORSPACE_RGB, FALSE, 8, surface2->w, surface2->h,
					surface2->pitch, NULL, NULL);
		}
		else
		{
			pixbuffer = gdk_pixbuf_new_from_data(surface2->pixels,
					GDK_COLORSPACE_RGB, TRUE, 8, surface2->w, surface2->h,
					surface2->pitch, NULL, NULL);
		}
		gtk_image_set_from_pixbuf(GTK_IMAGE(image2), pixbuffer);
		if(result)
		{
			gtk_label_set_text(GTK_LABEL(result_label), "Visage détecté");
		}
		else
		{
			gtk_label_set_text(GTK_LABEL(result_label), "Aucun visage détecté");
		}
	}
}

static void activate (GtkApplication *app)
{
	image = gtk_image_new();
	image2 = gtk_image_new();

	///// Set window ///////////////////////////////////////
	window = gtk_application_window_new (app);
	gtk_window_set_title (GTK_WINDOW (window), "HTCPCP by 2b|!2b");
	gtk_window_set_default_size (GTK_WINDOW (window), 200, 200);
	////////////////////////////////////////////////////////

	///// Set notebook (holding tabs) //////////////////////
	notebook = gtk_notebook_new ();
	////////////////////////////////////////////////////////

	///// Set database tab //////////////////////////////////
	database_tab = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 10);
	gtk_widget_set_halign(database_tab, GTK_ALIGN_CENTER);
	gtk_widget_set_valign(database_tab, GTK_ALIGN_CENTER);
	////////////////////////////////////////////////////////

	///// Set list of people //////////////////////////////////
	listbox = gtk_list_box_new ();
	gtk_widget_set_valign(listbox, GTK_ALIGN_CENTER);
	gtk_widget_set_margin_start(listbox, 20);
	g_signal_connect (listbox, "row-selected", G_CALLBACK (selected), NULL);
	gtk_container_add(GTK_CONTAINER (database_tab), listbox);
	///////////////////////////////////////////////////////

	////// Set image /////////////////////////////////////
	LoadImage("default.png", 0, image);
	gtk_widget_set_margin_start(image, 20);
	gtk_widget_set_margin_end(image, 20);
	gtk_container_add(GTK_CONTAINER (database_tab), image);
	///////////////////////////////////////////////////////

	////// Set entry /////////////////////////////////////
	entry = gtk_entry_new();
	gtk_container_add(GTK_CONTAINER (database_tab), entry);
	///////////////////////////////////////////////////////

	///// Set add button //////////////////////////////////
	add_button = gtk_button_new_with_label ("Ajouter");
	g_signal_connect (add_button, "clicked", G_CALLBACK (add_name), NULL);
	///////////////////////////////////////////////////////

	///// Set modify name button //////////////////////////////////
	modifyName_button = gtk_button_new_with_label ("Modifier le nom");
	g_signal_connect (modifyName_button, "clicked",
			G_CALLBACK (modify_name), NULL);
	///////////////////////////////////////////////////////

	///// Set modify image button //////////////////////////////////
	modifyImg_button = gtk_button_new_with_label ("Modifier l'image");
	g_signal_connect (modifyImg_button, "clicked",
			G_CALLBACK (modify_image), NULL);
	///////////////////////////////////////////////////////

	///// Set delete button //////////////////////////////////
	delete_button = gtk_button_new_with_label ("Supprimer");
	g_signal_connect (delete_button, "clicked",
			G_CALLBACK (delete_image), NULL);
	///////////////////////////////////////////////////////

	///// Set box of buttons ///////////////////////////////
	buttons_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 15);
	gtk_widget_set_valign(buttons_box, GTK_ALIGN_CENTER);
	gtk_widget_set_margin_start(buttons_box, 40);
	gtk_widget_set_margin_end(buttons_box, 40);

	gtk_container_add (GTK_CONTAINER (buttons_box), add_button);
	gtk_container_add (GTK_CONTAINER (buttons_box), modifyName_button);
	gtk_container_add (GTK_CONTAINER (buttons_box), modifyImg_button);
	gtk_container_add (GTK_CONTAINER (buttons_box), delete_button);
	gtk_container_add(GTK_CONTAINER (database_tab), buttons_box);
	///////////////////////////////////////////////////////

	///// Set facial recognition tab ///////////////////////
	facerecognition_tab = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_set_halign(facerecognition_tab, GTK_ALIGN_CENTER);
	gtk_widget_set_valign(facerecognition_tab, GTK_ALIGN_CENTER);
	/////////////////////////////////////////////////////////

	////// Set image2 ///////////////////////////////////////
	LoadImage("default.png", 1, image2);
	gtk_widget_set_margin_top(image2, 30);
	gtk_widget_set_margin_bottom(image2, 100);
	/////////////////////////////////////////////////////////

	///// Set change image button ///////////////////////////
	changeImg_button = gtk_button_new_with_label ("Changer d'image");
	g_signal_connect (changeImg_button, "clicked",
			G_CALLBACK (change_image), NULL);
	////////////////////////////////////////////////////////

	///// Set evaluate button //////////////////////////////
	evaluate_button = gtk_button_new_with_label ("Démarrer évaluation");
	g_signal_connect (evaluate_button, "clicked", G_CALLBACK (evaluate), NULL);
	gtk_widget_set_margin_bottom(evaluate_button, 50);
	////////////////////////////////////////////////////////

	///// Set result label //////////////////////////////
	result_label = gtk_label_new("En attente d'évaluation ...");
	gtk_widget_set_margin_bottom(result_label, 15);
	////////////////////////////////////////////////////////

	///// Set box of widgets ///////////////////////////////
	widgets_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 15);
	gtk_container_add (GTK_CONTAINER (widgets_box), image2);
	gtk_container_add (GTK_CONTAINER (widgets_box), changeImg_button);
	gtk_container_add (GTK_CONTAINER (widgets_box), evaluate_button);
	gtk_container_add (GTK_CONTAINER (widgets_box), result_label);
	gtk_container_add(GTK_CONTAINER (facerecognition_tab), widgets_box);
	///////////////////////////////////////////////////////

	///// Enter tabs in notebook //////////////////////////
	gtk_notebook_append_page(GTK_NOTEBOOK (notebook), database_tab,
			gtk_label_new ("Bibliothèque d'images"));
	gtk_notebook_append_page(GTK_NOTEBOOK (notebook), facerecognition_tab,
			gtk_label_new ("Reconnaissance faciale"));
	///////////////////////////////////////////////////////

	///// Enter notebook in window //////////////////////////
	gtk_container_add(GTK_CONTAINER (window), notebook);
	/////////////////////////////////////////////////////////

	///// Display everything ////////////////////////////////
	gtk_widget_show_all (window);
	/////////////////////////////////////////////////////////

}

int startinterface (int argc, char *argv[])
{
	GtkApplication *app;
	int status;

	app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
	g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
	status = g_application_run (G_APPLICATION (app),argc, argv);
	g_object_unref (app);

	return status;
}
