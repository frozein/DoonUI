#include "render.hpp"
#include <stdio.h>
#include <malloc.h>
#include <math.h>
#include <GLAD/glad.h>
#include "math/matrix.hpp"
#include "math/vector.hpp"

static bool _DNUI_load_into_buffer(const char* path, char** buffer);

//--------------------------------------------------------------------------------------------------------------------------------//

static GLuint shaderProgram;
static GLuint vertexArray;

//--------------------------------------------------------------------------------------------------------------------------------//

bool DNUI_init()
{
	//load shader:
	//---------------------------------

	//load from files:
	char* vertexSource = 0;
	if(!_DNUI_load_into_buffer("shaders/vertex.vert", &vertexSource))
		return false;

	char* fragmentSource = 0;
	if(!_DNUI_load_into_buffer("shaders/fragment.frag", &fragmentSource))
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
	unsigned int program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if(!success)
	{
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		printf("%s\n DNUI ERROR - FAILED TO LINK SHADER PROGRAM", infoLog);
		return false;
	}

	shaderProgram = program;

	//delete shaders:
	glDeleteShader(vertex);
	glDeleteShader(fragment);

	//free memory:
	free(vertexSource);
	free(fragmentSource);

	//create vertex buffer:
	//---------------------------------

	float quadVertices[] = {
     	 1.0f,  1.0f,
     	 1.0f, -1.0f,
    	-1.0f, -1.0f,
    	-1.0f, -1.0f,
    	 1.0f,  1.0f,
    	-1.0f,  1.0f
	};

	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 2, (void*)0);
	glEnableVertexAttribArray(0);

	vertexArray = VAO;

	return true;
}

void DNUI_close()
{
	glDeleteProgram(shaderProgram);
	glDeleteVertexArrays(1, &vertexArray);
}

void DNUI_drawrect(DNvec2 center, DNvec2 size, float angle)
{
	//get window width/height
	DNivec4 windowParams; //x, y, w, h
	glGetIntegerv(GL_VIEWPORT, (int*)&windowParams);

	//generate orthagonal projection:
	DNmat3 projection = DN_MAT3_IDENTITY;
	projection.m[0][0] = 2.0f / windowParams.z;
	projection.m[1][1] = 2.0f / windowParams.w;
	projection.m[2][0] = -1.0f;
	projection.m[2][1] = -1.0f;

	DNmat3 model = DN_mat3_translate(DN_MAT3_IDENTITY, {center.x, center.y});
	model = DN_mat3_rotate(model, angle);
	model = DN_mat3_scale(model, {size.x * 0.5f, size.y * 0.5f});

	model = DN_mat3_mult(projection, model);

	glUseProgram(shaderProgram);
	glUniformMatrix3fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, (GLfloat*)&model);
	glUniform4f(glGetUniformLocation(shaderProgram, "color"), 1.0, 0.0, 0.0, 1.0);
	glUniform2f(glGetUniformLocation(shaderProgram, "size"), size.x, size.y);
	glUniform1f(glGetUniformLocation(shaderProgram, "cornerRad"), 20.0f);
	glUniform1ui(glGetUniformLocation(shaderProgram, "useTex"), false);
	glBindVertexArray(vertexArray);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

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