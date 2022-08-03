#define GLFW_DLL

#include <iostream>
#include <GLAD/glad.h>
#include <GLFW/glfw3.h>
#include "DoonUI/element.hpp"
extern "C"
{
	#include "DoonUI/render.h"
	#include "DoonUI/math/vector.h"
}

//--------------------------------------------------------------------------------------------------------------------------------//

DNivec2 windowSize = {1280, 720};
DNUIelement baseElement = DNUIelement(); //create fullscreen element to serve as the "base"

//--------------------------------------------------------------------------------------------------------------------------------//

void window_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	DNUI_set_window_size(width, height);
	windowSize = {width, height};
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
		baseElement.handle_event(DNUIevent(DNUIevent::MOUSE_RELEASE));
}

void button_callback(int callbackID)
{
	std::cout << "Button #" << callbackID << " has been pressed!" << std::endl;
}

//--------------------------------------------------------------------------------------------------------------------------------//

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
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
	glEnable(GL_MULTISAMPLE);

	//initialize DNUI:
	//---------------------------------
	DNUI_init(windowSize.x, windowSize.y);
	DNUIfont* arialFont = DNUI_load_font("arial.ttf", 72);

	//create test layout:
	//---------------------------------
	DNUItransition buttonBaseTransition = DNUItransition(500.0f, DNUItransition::EXPONENTIAL);
	buttonBaseTransition.set_target_w(DNUIdimension(DNUIdimension::RELATIVE, 0.1f));
	buttonBaseTransition.set_target_vec4({1.0f, 0.0f, 0.0f, 1.0f}, offsetof(DNUIbutton, color));
	buttonBaseTransition.set_target_float(0.0f, offsetof(DNUIbutton, angle));

	DNUItransition buttonHoverTransition = DNUItransition(500.0f, DNUItransition::EXPONENTIAL);
	buttonHoverTransition.set_target_w(DNUIdimension(DNUIdimension::RELATIVE, 0.11f));
	buttonHoverTransition.set_target_vec4({1.0f, 1.0f, 0.0f, 1.0f}, offsetof(DNUIbutton, color));
	buttonHoverTransition.set_target_float(5.0f, offsetof(DNUIbutton, angle));

	DNUItransition buttonHoldTransition = DNUItransition(500.0f, DNUItransition::EXPONENTIAL);
	buttonHoldTransition.set_target_w(DNUIdimension(DNUIdimension::RELATIVE, 0.105f));
	buttonHoldTransition.set_target_vec4({0.0f, 1.0f, 0.0f, 1.0f}, offsetof(DNUIbutton, color));
	buttonHoldTransition.set_target_float(2.5f, offsetof(DNUIbutton, angle));

	DNUIbutton baseButton = DNUIbutton(DNUIcoordinate(DNUIcoordinate::RELATIVE, 0.5f , DNUIcoordinate::CENTER_CENTER), 
	                                   DNUIcoordinate(DNUIcoordinate::PIXELS  , 20.0f, DNUIcoordinate::CENTER_MIN),
	                                   DNUIdimension(DNUIdimension::RELATIVE, 0.1f), DNUIdimension(DNUIdimension::ASPECT, 1.0f),
	                                   button_callback, 0, -1, {1.0f, 0.0f, 0.0f, 1.0f}, 20.0f, 0.0f, 
	                                   {0.0f, 1.0f, 1.0f, 1.0f}, 5.0f,
	                                   buttonBaseTransition, buttonHoverTransition, buttonHoldTransition);

	DNUItext baseText = DNUItext(DNUIcoordinate(DNUIcoordinate::RELATIVE, 0.5f , DNUIcoordinate::CENTER_CENTER), 
	                             DNUIcoordinate(DNUIcoordinate::RELATIVE, 0.5f, DNUIcoordinate::CENTER_CENTER),
	                             DNUIdimension(DNUIdimension::RELATIVE, 0.4f),
	                             "0", arialFont);

	for(int i = 1; i <= 5; i++)
	{
		DNUIbutton* button = new DNUIbutton(baseButton);
		button->callbackID = i;
		button->xPos.relativePos = 0.2f + i / 7.0f;

		DNUItext* text = new DNUItext(baseText);
		text->text = std::to_string(i);
		button->children.push_back((DNUIelement*)text);

		baseElement.children.push_back((DNUIelement*)button);
	}

	DNUIbox* sidePanel = new DNUIbox(DNUIcoordinate(DNUIcoordinate::PIXELS, 20.0f, DNUIcoordinate::CENTER_MIN),
	                                 DNUIcoordinate(DNUIcoordinate::RELATIVE, 0.5f, DNUIcoordinate::CENTER_CENTER),
	                                 DNUIdimension(DNUIdimension::RELATIVE, 0.2f), DNUIdimension(DNUIdimension::SPACE, 40.0f),
	                                 -1, {0.0f, 0.0f, 1.0f, 1.0f}, 40.0f, 0.0f);

	DNUItext* sidePanelText = new DNUItext(DNUIcoordinate(DNUIcoordinate::PIXELS, 20.0f, DNUIcoordinate::CENTER_MIN), 
	                                       DNUIcoordinate(DNUIcoordinate::PIXELS, 20.0f, DNUIcoordinate::CENTER_MAX), 
	                                       DNUIdimension(DNUIdimension::SPACE, 40.0f), 
	                                       "A simple and efficient method is presented which allows improved rendering of glyphs composed of curved and linear elements. A distance field is generated from a high resolution image, and then stored into a channel of a lower-resolution texture. In the simplest case, this texture can then be rendered simply by using the alphatesting and alpha-thresholding feature of modern GPUs, without a custom shader.",
	                                       arialFont, {1.0f, 1.0f, 1.0f, 1.0f}, 0.5f, 300.0f);

	sidePanel->children.push_back(sidePanelText);
	baseElement.children.push_back(sidePanel);

	DNUItext* fancyText = new DNUItext(DNUIcoordinate(DNUIcoordinate::RELATIVE, 0.6f , DNUIcoordinate::CENTER_CENTER),
	                                   DNUIcoordinate(DNUIcoordinate::RELATIVE, 0.55f, DNUIcoordinate::CENTER_CENTER),
	                                   DNUIdimension(DNUIdimension::RELATIVE, 0.6f),
	                                   "DoonUI", arialFont, {1.0f, 1.0f, 1.0f, 1.0f}, 1.0f, 0.0f, 0, 0.65f, 0.5f, {1.0f, 0.2f, 1.0f, 1.0f}, 0.5f, 0.05f);

	baseElement.children.push_back(fancyText);

	//main loop:
	//---------------------------------
	float oldTime = glfwGetTime() * 1000.0f;

	while(!glfwWindowShouldClose(window))
	{
		//calculate dt:
		//---------------------------------
		float newTime = glfwGetTime() * 1000.0f;
		float deltaTime = newTime - oldTime;
		oldTime = newTime;

		//update mouse state:
		//---------------------------------
		double mouseX, mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);
		DNUIbutton::set_mouse_state({(float)mouseX - windowSize.x * 0.5f, (float)mouseY - windowSize.y * 0.5f}, glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);

		//clear rendering buffer:
		//---------------------------------
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		//render + draw ui:
		//---------------------------------
		baseElement.update(deltaTime, {0.0f, 0.0f}, {(float)windowSize.x, (float)windowSize.y});
		baseElement.render(1.0f);

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