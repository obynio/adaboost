#ifndef INTERFACE
#define INTERFACE

#if GTK_MINOR_VERSION < 12
#define gtk_widget_set_margin_start(x, y) gtk_widget_set_margin_left(x, y)
#define gtk_widget_set_margin_end(x, y) gtk_widget_set_margin_right(x, y)
#endif

int startinterface (int argc, char *argv[]);

#endif
