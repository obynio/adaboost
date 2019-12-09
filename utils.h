#ifndef _UTILS_H_
#define _UTILS_H_

#include <gtk/gtk.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "cascading.h"
#include "json.h"

void showResults (SDL_Surface* img, PositionList* result);
void GtkAdaptation (SDL_Surface* img);
int detect2(SDL_Surface* surface);

#endif
