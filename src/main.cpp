#define GLFW_DLL

#include <iostream>
#include <GLAD/glad.h>
#include <GLFW/glfw3.h>
#include "DoonUI/render.hpp"

void window_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
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

	DNUI_init();

	//main loop:
	//---------------------------------
	while(!glfwWindowShouldClose(window))
	{
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		DNUI_drawrect({1280 / 2, 720 / 2}, {720, 300}, glfwGetTime() * 30.0f);

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