#include "render.h"
#include "math/matrix.h"
#include "math/vector.h"

#include <stdio.h>
#include <malloc.h>
#include <math.h>
#include <string.h>
#include <GLAD/glad.h>
#include <FreeType/ft2build.h>
#include FT_FREETYPE_H

//--------------------------------------------------------------------------------------------------------------------------------//

static bool _DNUI_load_into_buffer(const char* path, char** buffer);
static bool _DNUI_load_shader_program(const char* vertPath, const char* fragPath, GLuint* program);

//--------------------------------------------------------------------------------------------------------------------------------//
//for rendering text:

static GLuint textProgram;
static GLuint textBuffer;
static GLuint textArray;

static FT_Library freetypeLib;

typedef struct DNUIfont
{
	int textureAtlas;
	DNivec2 atlasSize;
	float maxBearing;

	struct
	{
		float advance;
		float bmpW;
		float bmpH;
		float bmpL;
		float bmpT;
		float texOffset;
	} glyphInfo[128];
} DNUIfont;

#define DNUI_MAX_FONTS 16
int curFont = 0;
DNUIfont fonts[DNUI_MAX_FONTS];

//--------------------------------------------------------------------------------------------------------------------------------//
//for rendering rectangles:

static GLuint rectProgram;
static GLuint rectBuffer;
static GLuint rectArray;

//--------------------------------------------------------------------------------------------------------------------------------//

static DNvec2 windowSize;
static DNmat3 projectionMat;

//--------------------------------------------------------------------------------------------------------------------------------//

bool DNUI_init(unsigned int windowW, unsigned int windowH)
{
	//load shader programs:
	//---------------------------------
	if(!_DNUI_load_shader_program("shaders/vertex.vert", "shaders/rect.frag", &rectProgram))
		return false;
	
	if(!_DNUI_load_shader_program("shaders/vertex.vert", "shaders/text.frag", &textProgram))
		return false;

	//create rect vertex buffer:
	//---------------------------------
	float quadVertices[] = {
     	 1.0f,  1.0f, 1.0f, 1.0f,
     	 1.0f, -1.0f, 1.0f, 0.0f,
    	-1.0f, -1.0f, 0.0f, 0.0f,
    	-1.0f, -1.0f, 0.0f, 0.0f,
    	 1.0f,  1.0f, 1.0f, 1.0f,
    	-1.0f,  1.0f, 0.0f, 1.0f
	};

	glGenVertexArrays(1, &rectArray);
	glGenBuffers(1, &rectBuffer);

	glBindVertexArray(rectArray);
	glBindBuffer(GL_ARRAY_BUFFER, rectBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, (void*)(sizeof(GLfloat) * 2));
	glEnableVertexAttribArray(1);

	//create text vertex buffer:
	//---------------------------------
	glGenVertexArrays(1, &textArray);
	glGenBuffers(1, &textBuffer);

	glBindVertexArray(textArray);
	glBindBuffer(GL_ARRAY_BUFFER, textBuffer);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, (void*)(sizeof(GLfloat) * 2));
	glEnableVertexAttribArray(1);

	//set projection matrix:
	//---------------------------------
	DNUI_set_window_size(windowW, windowH);

	//initialize freetype:
	//---------------------------------
	if(FT_Init_FreeType(&freetypeLib))
	{
		printf("DNUI ERROR - FAILED TO INITIALIZE FREETYPE\n");
		return false;
	}

	return true;
}

void DNUI_close()
{
	glDeleteProgram(textProgram);
	glDeleteBuffers(1, &textBuffer);
	glDeleteVertexArrays(1, &textArray);

	glDeleteProgram(rectProgram);
	glDeleteBuffers(1, &rectBuffer);
	glDeleteVertexArrays(1, &rectArray);

	FT_Done_FreeType(freetypeLib);
}

DNvec2 DNUI_get_window_size()
{
	return windowSize;
}

void DNUI_set_window_size(unsigned int w, unsigned int h)
{
	//set vars:
	//---------------------------------
	windowSize.x = w;
	windowSize.y = h;

	//generate new projection matrix:
	//---------------------------------
	projectionMat = DN_MAT3_IDENTITY;
	projectionMat.m[0][0] = 2.0f / w;
	projectionMat.m[1][1] = 2.0f / h;
}

//--------------------------------------------------------------------------------------------------------------------------------//

int DNUI_load_font(const char* path, int size)
{	
	//find new font index:
	//---------------------------------
	int newIndex = curFont;
	for(int i = 0; i < curFont; i++)
		if(fonts[i].textureAtlas < 0)
		{
			newIndex = i;
			break;
		}
	
	if(newIndex == curFont)
	{
		if(curFont >= DNUI_MAX_FONTS)
		{
			printf("DNUI ERROR - MAX FONTS EXCEEDED\n");
			return -1;
		}

		curFont++;
	}

	//load freetype face:
	//---------------------------------
	FT_Face font;
	if(FT_New_Face(freetypeLib, path, 0, &font))
	{
		printf("DNUI ERROR - FAILED TO LOAD FONT \"%s\"\n", path);
		return -1;
	}

	FT_Set_Pixel_Sizes(font, 0, size);

	//compute texture dimensions:
	//---------------------------------
	const int texturePadding = 1;

	int w = 0;
	int h = 0;
	for(int i = 32; i < 128; i++)
	{
		if(FT_Load_Char(font, i, FT_LOAD_RENDER))
		{
			printf("DNUI ERROR - FAILED TO LOAD CHARACTER \"%c\"", i);
			continue;
		}

		FT_Render_Glyph(font->glyph, FT_RENDER_MODE_SDF); //TODO: find a way to not render the text twice

		fonts[newIndex].maxBearing = font->glyph->bitmap_top > fonts[newIndex].maxBearing ? font->glyph->bitmap_top : fonts[newIndex].maxBearing;

		w += font->glyph->bitmap.width + texturePadding;
		h = font->glyph->bitmap.rows > h ? font->glyph->bitmap.rows : h;
	}

	//create texture:
	//---------------------------------
	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //since the bitmaps generated by freetype have an alignment of 1 byte
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int x = 0;
	for(int i = 32; i < 128; i++)
	{
		if(FT_Load_Char(font, i, FT_LOAD_RENDER))
		{
			printf("DNUI ERROR - FAILED TO LOAD CHARACTER \"%c\"", i);
			continue;
		}

		FT_Render_Glyph(font->glyph, FT_RENDER_MODE_SDF);

		//set data:
		fonts[newIndex].glyphInfo[i].advance = font->glyph->advance.x / 64.0f;
		fonts[newIndex].glyphInfo[i].bmpW = font->glyph->bitmap.width;
		fonts[newIndex].glyphInfo[i].bmpH = font->glyph->bitmap.rows;
		fonts[newIndex].glyphInfo[i].bmpL = font->glyph->bitmap_left;
		fonts[newIndex].glyphInfo[i].bmpT = font->glyph->bitmap_top;
		fonts[newIndex].glyphInfo[i].texOffset = (float)x / w;

		//set texture:
		glTexSubImage2D(GL_TEXTURE_2D, 0, x, 0, font->glyph->bitmap.width, font->glyph->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, font->glyph->bitmap.buffer);

		//advance:
		x += font->glyph->bitmap.width + texturePadding;
	}

	fonts[newIndex].textureAtlas = tex;
	fonts[newIndex].atlasSize.x = w;
	fonts[newIndex].atlasSize.y = h;

	return newIndex;
}

void DNUI_free_font(int font)
{
	glDeleteTextures(1, &fonts[font].textureAtlas);
	fonts[font].textureAtlas = -1;
}

float _DNUI_line_render_size(const char* text, int font, float scale)
{
	float w = 0.0;

	for(char* c = (char*)text; *c != '\0'; c++)
	{
		if(*(c + 1) == '\0')
			w += fonts[font].glyphInfo[*c].bmpL + fonts[font].glyphInfo[*c].bmpW;
		else
			w += fonts[font].glyphInfo[*c].advance;
	}

	return w * scale;
}

DNvec2 DNUI_string_render_size(const char* text, int font, float scale, float maxW)
{
	DNvec2 res;
	if(maxW <= 0.0f)
		return (DNvec2){_DNUI_line_render_size(text, font, scale), fonts[font].atlasSize.y * scale};
	else
		res.x = maxW;

	int len = strlen(text);
	int numLines = 0;
	int startPos = 0;
	int lastSpace = -1;
	float curWidth = 0.0;

	int i;
	for(i = 0; i < len; i++)
	{
		char test = text[i];

		if(text[i] == ' ')
		{
			lastSpace = i;
			curWidth += fonts[font].glyphInfo[text[i]].advance * scale;
		}
		else if(curWidth + (fonts[font].glyphInfo[text[i]].bmpL + fonts[font].glyphInfo[text[i]].bmpW) * scale > maxW)
		{
			int endPos = lastSpace <= startPos ? i : lastSpace + 1;
			numLines++;
			startPos = endPos;
			i = startPos;
			curWidth = 0.0;
		}
		else
			curWidth += fonts[font].glyphInfo[text[i]].advance * scale;
	}

	if(i > startPos)
		numLines++;

	res.y = numLines * fonts[font].atlasSize.y * scale;
	return res;
}

//draws a single line of text
void _DNUI_draw_string_line(const char* text, int font, DNvec2 pos, float scale, DNvec4 color, float thickness, float softness, DNvec4 outlineColor, float outlineThickness, float outlineSoftness)
{
	//create vertex array:
	//---------------------------------
	struct Vertex
	{
		GLfloat x;
		GLfloat y;
		GLfloat texX;
		GLfloat texY;
	}* vertices;

	int len = strlen(text);
	int numVertices = 6 * len;
	vertices = malloc(4 * sizeof(GLfloat) * numVertices);

	//generate vertex data:
	//---------------------------------
	int i = 0;
	for(char* c = (char*)text; *c != '\0'; c++)
	{
		float texOffset = fonts[font].glyphInfo[*c].texOffset;
		float bmpW = fonts[font].glyphInfo[*c].bmpW / fonts[font].atlasSize.x;
		float bmpH = fonts[font].glyphInfo[*c].bmpH / fonts[font].atlasSize.y;

		float x =  pos.x + fonts[font].glyphInfo[*c].bmpL * scale;
		float y = -pos.y - (fonts[font].glyphInfo[*c].bmpT - fonts[font].maxBearing) * scale;
		float w = fonts[font].glyphInfo[*c].bmpW * scale;
		float h = fonts[font].glyphInfo[*c].bmpH * scale;

		pos.x += fonts[font].glyphInfo[*c].advance * scale;

		//don't render spaces
		if(w <= 0.0 || h <= 0.0)
			continue;

		vertices[i++] = (struct Vertex){x	 , -y	 , texOffset		, 0.0 };
		vertices[i++] = (struct Vertex){x + w, -y	 , texOffset + bmpW, 0.0 };
		vertices[i++] = (struct Vertex){x	 , -y - h, texOffset		, bmpH};
		vertices[i++] = (struct Vertex){x + w, -y	 , texOffset + bmpW, 0.0 };
		vertices[i++] = (struct Vertex){x	 , -y - h, texOffset		, bmpH};
		vertices[i++] = (struct Vertex){x + w, -y - h, texOffset + bmpW, bmpH};
	}

	//send to GPU:
	//---------------------------------
	glBindBuffer(GL_ARRAY_BUFFER, textBuffer);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(GLfloat) * i, vertices, GL_DYNAMIC_DRAW);

	glUseProgram(textProgram);
	glUniform4fv(glGetUniformLocation(textProgram, "color"), 1, (GLfloat*)&color);
	glUniform4fv(glGetUniformLocation(textProgram, "outlineColor"), 1, (GLfloat*)&outlineColor);
	glUniformMatrix3fv(glGetUniformLocation(textProgram, "modelProjection"), 1, GL_FALSE, (GLfloat*)&projectionMat);
	glUniform1i(glGetUniformLocation(textProgram, "textureAtlas"), 0);
	glUniform1f(glGetUniformLocation(textProgram, "scale"), scale);
	glUniform1f(glGetUniformLocation(textProgram, "thickness"), 1.0 - thickness);
	glUniform1f(glGetUniformLocation(textProgram, "softness"), softness);
	glUniform1f(glGetUniformLocation(textProgram, "outlineThickness"), 1.0 - outlineThickness);
	glUniform1f(glGetUniformLocation(textProgram, "outlineSoftness"), outlineSoftness);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fonts[font].textureAtlas);

	glBindVertexArray(textArray);
	glDrawArrays(GL_TRIANGLES, 0, i);

	free(vertices);
}

void DNUI_draw_string(const char* text, int font, DNvec2 pos, float scale, float maxW, int align, DNvec4 color, float thickness, float softness, DNvec4 outlineColor, float outlineThickness, float outlineSoftness)
{
	DNvec2 size = DNUI_string_render_size(text, font, scale, maxW);
	pos.x -= size.x * 0.5f;
	pos.y += size.y * 0.5f;

	if(maxW <= 0.0)
	{
		_DNUI_draw_string_line(text, font, pos, scale, color, thickness, softness, outlineColor, outlineThickness, outlineSoftness);
		return;
	}

	int len = strlen(text);
	int numLines = 0;
	int startPos = 0;
	int lastSpace = -1;
	float curWidth = 0.0;

	int i;
	for(i = 0; i < len; i++)
	{
		char test = text[i];

		if(text[i] == ' ')
		{
			lastSpace = i;
			curWidth += fonts[font].glyphInfo[text[i]].advance * scale;
		}
		else if(curWidth + (fonts[font].glyphInfo[text[i]].bmpL + fonts[font].glyphInfo[text[i]].bmpW) * scale > maxW)
		{
			char* line;

			int endPos = lastSpace <= startPos ? i : lastSpace + 1;
			line = malloc(endPos - startPos + 1);
			memcpy(line, &text[startPos], endPos - startPos);
			line[endPos - startPos] = '\0';

			float x = pos.x;
			if(align == 1)
				x += size.x - _DNUI_line_render_size(line, font, scale);
			else if(align == 2)
				x += (size.x - _DNUI_line_render_size(line, font, scale)) * 0.5f;

			_DNUI_draw_string_line(line, font, (DNvec2){x, pos.y - fonts[font].atlasSize.y * scale * numLines}, scale, color, thickness, softness, outlineColor, outlineThickness, outlineSoftness);

			free(line);

			numLines++;
			startPos = endPos;
			i = startPos;
			curWidth = 0.0;
		}
		else
			curWidth += fonts[font].glyphInfo[text[i]].advance * scale;
	}

	if(i > startPos)
	{
		char* line;

		line = malloc(i - startPos + 1);
		memcpy(line, &text[startPos], i - startPos);
		line[i - startPos] = '\0';

		float x = pos.x;
		if(align == 1)
			x += size.x - _DNUI_line_render_size(line, font, scale);
		else if(align == 2)
			x += (size.x - _DNUI_line_render_size(line, font, scale)) * 0.5f;

		_DNUI_draw_string_line(line, font, (DNvec2){x, pos.y - fonts[font].atlasSize.y * scale * numLines}, scale, color, thickness, softness, outlineColor, outlineThickness, outlineSoftness);

		free(line);
	}
}

void DNUI_draw_string_simple(const char* text, int font, DNvec2 pos, float scale, float wrap, int align, DNvec4 color)
{
	DNUI_draw_string(text, font, pos, scale, wrap, align, color, 0.5f, 0.05f, (DNvec4){0.0f, 0.0f, 0.0f, 0.0f}, 1.0f, 0.05f);
}

//--------------------------------------------------------------------------------------------------------------------------------//

void DNUI_draw_rect(int textureHandle, DNvec2 center, DNvec2 size, float angle, DNvec4 color, float cornerRad)
{
	DNmat3 model = DN_mat3_translate(DN_MAT3_IDENTITY, (DNvec2){center.x, center.y});
	model = DN_mat3_rotate(model, angle);
	model = DN_mat3_scale(model, (DNvec2){size.x * 0.5f, size.y * 0.5f});

	model = DN_mat3_mult(projectionMat, model);

	glUseProgram(rectProgram);

	glUniformMatrix3fv(glGetUniformLocation(rectProgram, "modelProjection"), 1, GL_FALSE, (GLfloat*)&model);
	glUniform4fv(glGetUniformLocation(rectProgram, "color"), 1, (GLfloat*)&color);
	glUniform2fv(glGetUniformLocation(rectProgram, "size"), 1, (GLfloat*)&size);
	glUniform1f(glGetUniformLocation(rectProgram, "cornerRad"), cornerRad);

	if(textureHandle >= 0)
	{
		glUniform1ui(glGetUniformLocation(rectProgram, "useTex"), true);
		glUniform1i(glGetUniformLocation(rectProgram, "tex"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureHandle);
	}
	else
		glUniform1ui(glGetUniformLocation(rectProgram, "useTex"), false);

	glBindVertexArray(rectArray);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

//--------------------------------------------------------------------------------------------------------------------------------//

static bool _DNUI_load_into_buffer(const char* path, char** buffer)
{
	*buffer = 0;
	long length;
	FILE* file = fopen(path, "rb");

	if(file)
	{
		bool result = false;

		fseek(file, 0, SEEK_END);
		length = ftell(file);
		fseek(file, 0, SEEK_SET);
		*buffer = (char*)calloc(1, length + 1);

		if(*buffer)
		{
			if(fread(*buffer, length, 1, file) == 1)
				result = true;
			else
			{
				printf("DNUI ERROR - COULD NOT READ FROM FILE %s\n", path);
				result = false;
				free(*buffer);
			}
		}
		else
			printf("DNUI ERROR - COULD NOT ALLOCATE MEMORY FOR SHADER SOURCE CODE");

		fclose(file);
		return result;
	}
	else
	{
		printf("DNUI ERROR - COULD NOT OPEN FILE %s\n", path);
		return false;
	}
}

bool _DNUI_load_shader_program(const char* vertPath, const char* fragPath, GLuint* program)
{
	//load from files:
	char* vertexSource = 0;
	if(!_DNUI_load_into_buffer(vertPath, &vertexSource))
		return false;

	char* fragmentSource = 0;
	if(!_DNUI_load_into_buffer(fragPath, &fragmentSource))
		return false;

	unsigned int vertex, fragment;
	int success;
	char infoLog[512];

	//compile vertex shader:
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertexSource, NULL);
	glCompileShader(vertex);
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if(!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		printf("vertex.vert - %s\n DNUI ERROR - FAILED TO COMPILE VERTEX SHADER\n", infoLog);
		return false;
	}

	//compile fragment shader:
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragmentSource, NULL);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if(!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		printf("fragment.frag - %s\n DNUI ERROR - FAILED TO COMPILE FRAGMENT SHADER", infoLog);
		return false;
	}

	//link shaders:
	unsigned int prog = glCreateProgram();
	glAttachShader(prog, vertex);
	glAttachShader(prog, fragment);
	glLinkProgram(prog);
	glGetProgramiv(prog, GL_LINK_STATUS, &success);
	if(!success)
	{
		glGetProgramInfoLog(prog, 512, NULL, infoLog);
		printf("%s\n DNUI ERROR - FAILED TO LINK SHADER PROGRAM", infoLog);
		return false;
	}

	*program = prog;

	//delete shaders:
	glDeleteShader(vertex);
	glDeleteShader(fragment);

	//free memory:
	free(vertexSource);
	free(fragmentSource);

	return true;
}