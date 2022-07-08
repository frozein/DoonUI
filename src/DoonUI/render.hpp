#ifndef DNUI_RENDER_H
#define DNUI_RENDER_H

#include "math/common.hpp"

bool DNUI_init();
void DNUI_close();

void DNUI_drawrect(DNvec2 center, DNvec2 size, float angle);

#endif