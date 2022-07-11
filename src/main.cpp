#define GLFW_DLL

#include <iostream>
#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

extern "C" //idk why but this works
{
	#include "DoonUI/render.h"
}

void window_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	DNUI_set_window_size(width, height);
}

int main()
{
	//init GLFW:
	//---------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_SAMPLES, 4);

	//create and init window:
	//---------------------------------
	GLFWwindow* window = glfwCreateWindow(1280, 720, "DoonUI", NULL, NULL);
	if(window == NULL)
	{
		printf("Failed to create GLFW window\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	//load opengl functions:
	//---------------------------------
	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		printf("Failed to initialize GLAD\n");
		return -1;
	}

	//set gl viewport:
	//---------------------------------
	glViewport(0, 0, 1280, 720);
	glfwSetWindowSizeCallback(window, window_size_callback);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

	glEnable(GL_MULTISAMPLE);

	DNUI_init(1280, 720);
	DNUI_load_font("arial.ttf", 72);
	//DNUI_drawstring("t", 0, {0.0, 0.0}, {1.0, 1.0});

	//main loop:
	//---------------------------------
	while(!glfwWindowShouldClose(window))
	{
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//DNUI_drawrect({1280 / 2, 720 / 2}, {720, 300}, 0.0f, {0.0, 1.0, 0.0, 1.0}, sinf(glfwGetTime() * 3.0f) * 60 + 70);
		DNUI_drawstring("t", 0, {100, 100}, {1.0, 1.0});

		//finish rendering and swap:
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//clean up and close:
	//---------------------------------
	DNUI_close();
	glfwTerminate();
	return 0;
}