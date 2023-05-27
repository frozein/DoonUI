#define GLFW_DLL

#include <iostream>
#include <GLAD/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include "DoonUI/doonui.hpp"
#include "QuickMath/quickmath.h"

//--------------------------------------------------------------------------------------------------------------------------------//

unsigned int windowW = 1280, windowH = 720;
dnui::Element baseElement = dnui::Element(); //create fullscreen element to serve as the "base"

float deltaTime = 0.0f;

float sliderVal = 5.0f;

//--------------------------------------------------------------------------------------------------------------------------------//

void window_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	DNUI_set_window_size(width, height);
	windowW = width;
	windowH = height;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
		baseElement.handle_event(dnui::Event(dnui::Event::MOUSE_RELEASE));
}

void scroll_callback(GLFWwindow* window, double offsetX, double offsetY)
{
	dnui::Event scrollEvent = dnui::Event(dnui::Event::SCROLL);
	scrollEvent.scroll.dir = (float)offsetY * deltaTime / 20.0f;
	baseElement.handle_event(scrollEvent);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if(action != GLFW_PRESS && action != GLFW_REPEAT)
		return;

	if(key == GLFW_KEY_RIGHT || key == GLFW_KEY_LEFT || key == GLFW_KEY_DOWN || key == GLFW_KEY_UP)
	{
		dnui::Event arrowEvent = dnui::Event(dnui::Event::ARROW_KEY_PRESS);
		arrowEvent.arrowKey.dir = key - GLFW_KEY_RIGHT;
		baseElement.handle_event(arrowEvent);
	}

	if(key == GLFW_KEY_BACKSPACE || key == GLFW_KEY_DELETE)
	{
		dnui::Event deleteEvent = dnui::Event(dnui::Event::DELETE_KEY_PRESS);
		deleteEvent.del.backspace = key == GLFW_KEY_BACKSPACE;
		baseElement.handle_event(deleteEvent);
	}
}

void character_callback(GLFWwindow* window, unsigned int character)
{
	if(character > 128) //non-ASCII not supported
		return;

	dnui::Event charEvent = dnui::Event(dnui::Event::CHARACTER);
	charEvent.character.character = character;
	baseElement.handle_event(charEvent);
}

void button_callback(int callbackID, void* userData)
{
	std::cout << "Button #" << callbackID << " has been pressed!" << std::endl << "Slider Value: " << sliderVal << std::endl;
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
	GLFWwindow* window = glfwCreateWindow(windowW, windowH, "DoonUI", NULL, NULL);
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
	glViewport(0, 0, windowW, windowH);
	glfwSetWindowSizeCallback(window, window_size_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCharCallback(window, character_callback);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
	glEnable(GL_MULTISAMPLE);

	//initialize DNUI:
	//---------------------------------
	DNUI_init(windowW, windowH);
	DNUIfont* arialFont = DNUI_load_font("arial.ttf", 72);

	//create test layout:
	//---------------------------------
	dnui::Transition buttonBaseTransition = dnui::Transition(500.0f, dnui::Transition::EXPONENTIAL);
	buttonBaseTransition.set_target_w(dnui::Dimension(dnui::Dimension::RELATIVE, 0.1f));
	buttonBaseTransition.set_target_vec4({1.0f, 0.0f, 0.0f, 1.0f}, offsetof(dnui::Button, m_color));
	buttonBaseTransition.set_target_float(0.0f, offsetof(dnui::Button, m_angle));

	dnui::Transition buttonHoverTransition = dnui::Transition(500.0f, dnui::Transition::EXPONENTIAL);
	buttonHoverTransition.set_target_w(dnui::Dimension(dnui::Dimension::RELATIVE, 0.11f));
	buttonHoverTransition.set_target_vec4({1.0f, 1.0f, 0.0f, 1.0f}, offsetof(dnui::Button, m_color));
	buttonHoverTransition.set_target_float(5.0f, offsetof(dnui::Button, m_angle));

	dnui::Transition buttonHoldTransition = dnui::Transition(500.0f, dnui::Transition::EXPONENTIAL);
	buttonHoldTransition.set_target_w(dnui::Dimension(dnui::Dimension::RELATIVE, 0.105f));
	buttonHoldTransition.set_target_vec4({0.0f, 1.0f, 0.0f, 1.0f}, offsetof(dnui::Button, m_color));
	buttonHoldTransition.set_target_float(2.5f, offsetof(dnui::Button, m_angle));

	dnui::Button baseButton = dnui::Button(dnui::Coordinate(dnui::Coordinate::RELATIVE, 0.5f , dnui::Coordinate::CENTER_CENTER), 
	                                       dnui::Coordinate(dnui::Coordinate::PIXELS  , 20.0f, dnui::Coordinate::CENTER_MIN),
	                                       dnui::Dimension(dnui::Dimension::RELATIVE, 0.1f), dnui::Dimension(dnui::Dimension::ASPECT, 1.0f),
	                                       button_callback, 0, nullptr, -1, {1.0f, 0.0f, 0.0f, 1.0f}, 20.0f, 0.0f, 
	                                       {0.0f, 1.0f, 1.0f, 1.0f}, 5.0f,
	                                       buttonBaseTransition, buttonHoverTransition, buttonHoldTransition);

	dnui::Text baseText = dnui::Text(dnui::Coordinate(dnui::Coordinate::RELATIVE, 0.5f , dnui::Coordinate::CENTER_CENTER), 
	                                 dnui::Coordinate(dnui::Coordinate::RELATIVE, 0.5f, dnui::Coordinate::CENTER_CENTER),
	                                 dnui::Dimension(dnui::Dimension::RELATIVE, 0.4f),
	                                 "0", arialFont);

	for(int i = 1; i <= 5; i++)
	{
		dnui::Button* button = new dnui::Button(baseButton);
		button->m_callbackID = i;
		button->m_xPos.relativePos = 0.2f + i / 7.0f;

		dnui::Text* text = new dnui::Text(baseText);
		text->m_text = std::to_string(i);
		button->m_children.push_back((dnui::Element*)text);

		baseElement.m_children.push_back((dnui::Element*)button);
	}

	dnui::Box* sidePanel = new dnui::Box(dnui::Coordinate(dnui::Coordinate::PIXELS, 20.0f, dnui::Coordinate::CENTER_MIN),
	                                     dnui::Coordinate(dnui::Coordinate::RELATIVE, 0.5f, dnui::Coordinate::CENTER_CENTER),
	                                     dnui::Dimension(dnui::Dimension::RELATIVE, 0.2f), dnui::Dimension(dnui::Dimension::SPACE, 40.0f),
	                                     -1, {0.0f, 0.0f, 1.0f, 1.0f}, 40.0f, 0.0f);

	dnui::Text* sidePanelText = new dnui::Text(dnui::Coordinate(dnui::Coordinate::PIXELS, 20.0f, dnui::Coordinate::CENTER_MIN), 
	                                           dnui::Coordinate(dnui::Coordinate::PIXELS, 20.0f, dnui::Coordinate::CENTER_MAX), 
	                                           dnui::Dimension(dnui::Dimension::SPACE, 40.0f), 
	                                           "A simple and efficient method is presented which allows improved rendering of glyphs composed of curved and linear elements. A distance field is generated from a high resolution image, and then stored into a channel of a lower-resolution texture. In the simplest case, this texture can then be rendered simply by using the alphatesting and alpha-thresholding feature of modern GPUs, without a custom shader.",
	                                           arialFont, {1.0f, 1.0f, 1.0f, 1.0f}, 0.5f, 300.0f);

	sidePanel->m_children.push_back(sidePanelText);
	baseElement.m_children.push_back(sidePanel);

	dnui::Text* fancyText = new dnui::Text(dnui::Coordinate(dnui::Coordinate::RELATIVE, 0.6f , dnui::Coordinate::CENTER_CENTER),
	                                       dnui::Coordinate(dnui::Coordinate::RELATIVE, 0.55f, dnui::Coordinate::CENTER_CENTER),
	                                       dnui::Dimension(dnui::Dimension::RELATIVE, 0.6f),
	                                       "DoonUI", arialFont, {1.0f, 1.0f, 1.0f, 1.0f}, 0.0f, 0.0f, 0, 0.65f, 0.5f, {1.0f, 0.2f, 1.0f, 1.0f}, 0.5f, 0.05f);

	baseElement.m_children.push_back(fancyText);

	dnui::Slider* slider = new dnui::Slider(dnui::Coordinate(dnui::Coordinate::PIXELS, 20.0f, dnui::Coordinate::CENTER_MAX), 
	                                      dnui::Coordinate(dnui::Coordinate::RELATIVE, 0.55f, dnui::Coordinate::CENTER_CENTER), 
										  dnui::Dimension(dnui::Dimension::PIXELS, 50.0f), dnui::Dimension(dnui::Dimension::RELATIVE, 0.5f), 
										  dnui::Slider::FLOAT, &sliderVal, 0.0f, 10.0f, true, {0.0f, 0.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f},
										  3.0f, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 1.0f, 1.0f}, 3.0f, 10.0f);
	baseElement.m_children.push_back(slider);

	dnui::TextBox* textBox = new dnui::TextBox(dnui::Coordinate(dnui::Coordinate::RELATIVE, 0.6f , dnui::Coordinate::CENTER_CENTER),
	                                           dnui::Coordinate(dnui::Coordinate::PIXELS, 20.0f, dnui::Coordinate::CENTER_MAX), 
											   dnui::Dimension(dnui::Dimension::RELATIVE, 0.5f), dnui::Dimension(dnui::Dimension::ASPECT, 0.15f), "edit me", arialFont, {1.0f, 1.0f, 1.0f, 1.0f},
											   0.7f, {0.0f, 0.0f, 0.0f, 1.0f}, 0.5f, 10.0f, {0.5f, 0.5f, 0.5f, 1.0f}, 0.0f);
	baseElement.m_children.push_back(textBox);

	//main loop:
	//---------------------------------
	float oldTime = glfwGetTime() * 1000.0f;

	while(!glfwWindowShouldClose(window))
	{
		//calculate dt:
		//---------------------------------
		float newTime = glfwGetTime() * 1000.0f;
		deltaTime = newTime - oldTime;
		oldTime = newTime;

		//update mouse state:
		//---------------------------------
		double mouseX, mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);
		dnui::Button::set_mouse_state({(float)mouseX - windowW * 0.5f, (float)mouseY - windowH * 0.5f}, glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
		dnui::TextBox::set_control_key_state(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT  ) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT  ) == GLFW_PRESS, 
		                                     glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS);

		//clear rendering buffer:
		//---------------------------------
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		//render + draw ui:
		//---------------------------------
		baseElement.update(deltaTime, {0.0f, 0.0f}, {(float)windowW, (float)windowH});
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