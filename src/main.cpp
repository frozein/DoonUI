#define GLFW_DLL

#include <iostream>
#include <GLAD/glad.h>
#include <GLFW/glfw3.h>
#include "DoonUI/element.hpp"

extern "C" //idk why but this works
{
	#include "DoonUI/render.h"
	#include "DoonUI/math/vector.h"
}

DNivec2 windowSize = {1280, 720};
DNUIelement* baseElement;

void window_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	DNUI_set_window_size(width, height);
	windowSize = {width, height};
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
		baseElement->handle_event(DNUIevent(DNUIevent::MOUSE_RELEASE));
}

int main()
{
	//init GLFW:
	//---------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

	DNUI_init(windowSize.x, windowSize.y);
	int arialFont = DNUI_load_font("arial.ttf", 72);

	DNUIbutton testBox = DNUIbutton(DNUIbutton::AnimState(DNUIcoordinate(DNUIcoordinate::RELATIVE, 0.5f, DNUIcoordinate::CENTER_CENTER), DNUIcoordinate(DNUIcoordinate::RELATIVE, 0.5f, DNUIcoordinate::CENTER_CENTER), DNUIdimension(DNUIdimension::RELATIVE, 0.5f), DNUIdimension(DNUIdimension::ASPECT, 1.0f), {1.0f, 0.0f, 0.0f, 1.0f}, 20.0f, 0.0f), nullptr, 0);
	
	DNUItext* testText = new DNUItext(DNUIcoordinate(DNUIcoordinate::PIXELS, 20.0f, DNUIcoordinate::CENTER_MIN), DNUIcoordinate(DNUIcoordinate::PIXELS, 20.0f, DNUIcoordinate::CENTER_MAX), 
	DNUIdimension(DNUIdimension::RELATIVE, 0.45f), "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Fusce placerat congue sollicitudin. Class aptent taciti sociosqu ad litora torquent per conubia nostra, per inceptos himenaeos.", arialFont, {1.0f, 1.0f, 1.0f, 1.0f}, 0.5f, 300.0f);

	DNUItext* testText2 = new DNUItext(DNUIcoordinate(DNUIcoordinate::PIXELS, 20.0f, DNUIcoordinate::CENTER_MAX), DNUIcoordinate(DNUIcoordinate::PIXELS, 20.0f, DNUIcoordinate::CENTER_MAX), 
	DNUIdimension(DNUIdimension::RELATIVE, 0.45f), "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Fusce placerat congue sollicitudin. Class aptent taciti sociosqu ad litora torquent per conubia nostra, per inceptos himenaeos.", arialFont, {1.0f, 1.0f, 1.0f, 1.0f}, -1.0f, 600.0f, 1);

	testBox.children.push_back((DNUIelement*)testText);
	testBox.children.push_back((DNUIelement*)testText2);

	testBox.start_anim(DNUIanimData(1.0f, true, DNUIanimData::EXPONENTIAL), DNUIbutton::AnimState(DNUIcoordinate(DNUIcoordinate::RELATIVE, 0.0f, DNUIcoordinate::CENTER_CENTER), DNUIcoordinate(DNUIcoordinate::RELATIVE, 0.5f, DNUIcoordinate::CENTER_CENTER), DNUIdimension(DNUIdimension::RELATIVE, 0.25f), DNUIdimension(DNUIdimension::ASPECT, 1.0f), {1.0f, 0.0f, 0.0f, 1.0f}, 20.0f, 0.0f));

	baseElement = &testBox;

	//main loop:
	//---------------------------------
	float oldTime = glfwGetTime();

	while(!glfwWindowShouldClose(window))
	{
		float newTime = glfwGetTime();
		float deltaTime = newTime - oldTime;
		oldTime = newTime;

		double mouseX, mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);
		DNUIbutton::set_mouse_state({(float)mouseX - windowSize.x * 0.5f, (float)mouseY - windowSize.y * 0.5f}, glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS);

		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		testBox.update(deltaTime, {0.0f, 0.0f}, {(float)windowSize.x, (float)windowSize.y});
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