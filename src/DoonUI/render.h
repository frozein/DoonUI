#ifndef DNUI_RENDER_H
#define DNUI_RENDER_H

#include "math/common.h"
#include <stdbool.h>

//--------------------------------------------------------------------------------------------------------------------------------//
//INITIALIZATION:

/* Initializes the DoonUI library, must be called before any other DNUI functions are called
 * @param windowW the width of the window, in pixels
 * @param windowH the height of the window, in pixels
 * @returns true on success, false on failure 
 */
bool DNUI_init(unsigned int windowW, unsigned int windowH);
/* De-initializes the DoonUI library, must be called to avoid memory leaks
 */
void DNUI_close();

/* @returns the size of the window as is known to DNUI, in pixels
 */
DNvec2 DNUI_get_window_size();
/* Call whenever the window size changes so that proper coordinates can be calculated
 * @param w the new width of the window, in pixels
 * @param h the new height of the window, in pixels
 */
void DNUI_set_window_size(unsigned int w, unsigned int h);

//--------------------------------------------------------------------------------------------------------------------------------//
//TEXT RENDERING:

/* Loads a font from a TrueType font file
 * @param path the file path to the .ttf file
 * @param size the height of each glyph, in pixels, larger values may take significantly longer to load
 * @returns a handle to the loaded font, or -1 on failure
 */
int DNUI_load_font(const char* path, int size);
/* Frees a font from memory, must be called to avoid memory leaks
 * @param font the handle to the font to free
 */
void DNUI_free_font(int font);

/* Calculates the size of a string when rendered to the screen
 * @param text the string to calculate
 * @param font the handle to the font to use
 * @param scale the scale of the text, a scale of 1.0 means that the font will be rendered at its actual resolution
 * @param wrap the maximum number of pixels the string can extend horizontally before wrapping to a new line. Set to 0 if no wrapping is desired
 * @returns the size of the string when rendered, in pixels
 */
DNvec2 DNUI_string_render_size(const char* text, int font, float scale, float wrap);
/* Renders a string to the screen
 * @param text the string to render
 * @param font the handle to the font to use
 * @param pos the position of the center of the string, in pixels
 * @param scale the scale of the text, a scale of 1.0 means that the font will be rendered at its actual resolution
 * @param wrap the maximum number of pixels the string can extend horizontally before wrapping to a new line. Set to 0 if no wrapping is desired
 * @param align how to align the text when wrapping: 0 = align left (all lines start at left side), 1 = align right (all lines end at right side), 2 = align center (all lines are individually centered)
 * @param color the color of the text, in rgba format
 * @param thickness the thickness of the text, 0.5 is the default value
 * @param softness the softness of the text's edges, 0.05 is the default value
 * @param outlineColor the color of the text's outline, in rgba format
 * @param outlineThickness the thickness at which the text's outline begins. Set to 1.0 if no outline is desired
 * @param outlineSoftness the softness of the outline's edges, 0.05 is the default value
 */
void DNUI_draw_string(const char* text, int font, DNvec2 pos, float scale, float wrap, int align, DNvec4 color, float thickness, float softness, DNvec4 outlineColor, float outlineThickness, float outlineSoftness);
/* Renders a string to the screen, same as DNUI_draw_string() but with fewer parameters
 * @param text the string to render
 * @param font the handle to the font to use
 * @param pos the position of the center of the string, in pixels
 * @param scale the scale of the text, a scale of 1.0 means that the font will be rendered at its actual resolution
 * @param wrap the maximum number of pixels the string can extend horizontally before wrapping to a new line. Set to 0 if no wrapping is desired
 * @param align how to align the text when wrapping: 0 = align left (all lines start at left side), 1 = align right (all lines end at right side), 2 = align center (all lines are individually centered)
 * @param color the color of the text, in rgba format
 */
void DNUI_draw_string_simple(const char* text, int font, DNvec2 pos, float scale, float wrap, int align, DNvec4 color);

//--------------------------------------------------------------------------------------------------------------------------------//
//RECT RENDERING:

/* Renders a rectangle to the screen
 * @param textureHandle a handle to an openGL texture to render, set to -1 if no texture is desired
 * @param center the position of the rectangle's center, in pixels. {0, 0} denotes the center of the screen
 * @param size the size, in pixels, of the rectangle
 * @param angle the angle, in degrees, to rotate the rectangle
 * @param color the color of the rectangle, in rgba format. If a texture is used, the final color will be the texture's color multiplied by this
 * @param cornerRad used to add rounded corners. denotes the radius of the rectangles corners, in pixels
 * @param outlineColor the color of the rectangle's outline, if one is desired
 * @param outlineThickness the thickness of the rectangle's outline, in pixels, or 0 if no outline is desired
 */
void DNUI_draw_rect(int textureHandle, DNvec2 center, DNvec2 size, float angle, DNvec4 color, float cornerRad, DNvec4 outlineColor, float outlineThickness);

//--------------------------------------------------------------------------------------------------------------------------------//

#endif