#define GLFW_DLL

#include <iostream>
#include <GLAD/glad.h>
#include <GLFW/glfw3.h>
#include "DoonUI/element.hpp"

extern "C" //idk why but this works
{
	#include "DoonUI/render.h"
}

DNivec2 windowSize = {1280, 720};

void window_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	DNUI_set_window_size(width, height);
	windowSize = {width, height};
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
	GLFWwindow* window = glfwCreateWindow(windowSize.x, windowSize.y, "DoonUI", NULL, NULL);
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
	glViewport(0, 0, windowSize.x, windowSize.y);
	glfwSetWindowSizeCallback(window, window_size_callback);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

	glEnable(GL_MULTISAMPLE);

	DNUI_init(windowSize.x, windowSize.y);
	int arialFont = DNUI_load_font("arial.ttf", 72);

	DNUIbox testBox = DNUIbox(DNUIcoordinate(DNUIcoordinate::PIXELS, 20.0f, DNUIcoordinate::CENTER_MAX), DNUIcoordinate(DNUIcoordinate::RELATIVE, 0.5f, DNUIcoordinate::CENTER_CENTER), DNUIdimension(DNUIdimension::RELATIVE, 0.5f), DNUIdimension(DNUIdimension::ASPECT, 1.0f), {1.0f, 0.0f, 0.0f, 1.0f}, 20.0f, -1);
	
	DNUItext* testText = new DNUItext(DNUIcoordinate(DNUIcoordinate::PIXELS, 20.0f, DNUIcoordinate::CENTER_MIN), DNUIcoordinate(DNUIcoordinate::PIXELS, 20.0f, DNUIcoordinate::CENTER_MIN), 
	DNUIdimension(DNUIdimension::RELATIVE, 0.25f), "Lorem ipsum dolor sit amet, consectetur adipiscing elit.", arialFont, {1.0f, 1.0f, 1.0f, 1.0f}, 0.5f, 300.0f);

	DNUIbox* testBox3 = new DNUIbox(DNUIcoordinate(DNUIcoordinate::RELATIVE, 0.5f, DNUIcoordinate::CENTER_CENTER), DNUIcoordinate(DNUIcoordinate::RELATIVE, 0.5f, DNUIcoordinate::CENTER_CENTER), DNUIdimension(DNUIdimension::RELATIVE, 0.5f), DNUIdimension(DNUIdimension::ASPECT, 1.0f), {0.0f, 0.0f, 1.0f, 1.0f}, 20.0f, -1);
	testText->children.push_back((DNUIelement*)testBox3);

	DNUIbox* testBox2 = new DNUIbox(DNUIcoordinate(DNUIcoordinate::PIXELS, 20.0f, DNUIcoordinate::CENTER_MIN), DNUIcoordinate(DNUIcoordinate::RELATIVE, 0.5f, DNUIcoordinate::CENTER_CENTER), DNUIdimension(DNUIdimension::RELATIVE, 0.5f), DNUIdimension(DNUIdimension::ASPECT, 1.0f), {0.0f, 1.0f, 0.0f, 1.0f}, 20.0f, -1);
	testBox.children.push_back((DNUIelement*)testBox2);
	testBox.children.push_back((DNUIelement*)testText);

	//main loop:
	//---------------------------------
	while(!glfwWindowShouldClose(window))
	{
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		testBox.update(1.0f, {0.0f, 0.0f}, {(float)windowSize.x, (float)windowSize.y});
		testBox.render();
		//DNUI_draw_rect(-1, {0, 720 / 2}, {w * 2, 600}, 0.0f, {1.0, 1.0, 0.0, 1.0}, 30);
		//DNUI_draw_string("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Aenean et arcu metus. Fusce placerat congue sollicitudin. Class aptent taciti sociosqu ad litora torquent per conubia nostra, per inceptos himenaeos. Etiam vitae nulla vitae neque lacinia sollicitudin ac ut ipsum. Phasellus leo quam, lobortis ac tincidunt a, molestie non eros. Nulla ultrices fermentum justo, a porta nisl. Vivamus bibendum tempus augue, non aliquam quam dapibus in.", arialFont, {0, 0}, 0.5f, 500, 2, {1.0f, 1.0f, 1.0f, 1.0f}, 0.7f, 0.05f, {0.0f, 0.0f, 0.0f, 1.0f}, 0.5f, 0.05f);
		//DNUI_draw_string("Hello world!", arialFont, {100, 350}, 3.0f, {1.0f, 1.0f, 1.0f, 1.0f}, 0.55f, 0.05f, {1.0f, 0.0f, 0.0f, 1.0f}, 0.45f, 0.05f);
		//DNUI_draw_string("Hello world!", arialFont, {100, 150}, 3.0f, {1.0f, 1.0f, 1.0f, 1.0f}, 0.5f, 0.05f, {0.0f, 1.0f, 0.0f, 1.0f}, 0.45f, 0.5f);

		//finish rendering and swap:
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//clean up and close:
	//---------------------------------
	DNUI_free_font(arialFont);
	DNUI_close();
	glfwTerminate();
	return 0;
}