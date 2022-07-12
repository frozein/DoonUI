#ifndef DNUI_RENDER_H
#define DNUI_RENDER_H

#include "math/common.h"
#include <stdbool.h>

bool DNUI_init(unsigned int windowW, unsigned int windowH);
void DNUI_close();

void DNUI_set_window_size(unsigned int w, unsigned int h);

void DNUI_drawrect(DNvec2 center, DNvec2 size, float angle, DNvec4 color, float cornerRad);
void DNUI_drawstring(const char* text, unsigned int font, DNvec2 pos, float scale, DNvec4 color, float thickness, float softness, DNvec4 outlineColor, float outlineThickness, float outlineSoftness);

int DNUI_load_font(const char* path, int size);
void DNUI_free_font(int font);

#endif