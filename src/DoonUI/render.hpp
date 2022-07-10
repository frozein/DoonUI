#ifndef DNUI_RENDER_H
#define DNUI_RENDER_H

#include "math/common.hpp"

bool DNUI_init(unsigned int windowW, unsigned int windowH);
void DNUI_close();

void DNUI_set_window_size(unsigned int w, unsigned int h);

void DNUI_drawrect(DNvec2 center, DNvec2 size, float angle, DNvec4 color, float cornerRad);

#endif