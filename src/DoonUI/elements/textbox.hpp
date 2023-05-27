#ifndef TEXTBOX_H
#define TEXTBOX_H

#include "../element.hpp"
#include "box.hpp"
#include <string>

namespace dnui
{

//a string of text
class TextBox : public Element
{
public:
	//elements:
	Box* m_background; //the background box, the text gets rendered over this
	Box* m_cursor;     //the cursor box
	Box* m_highlight;  //the highlighted text box

	//text parameters:
	std::string m_text;                                   //the text to render
	DNUIfont* m_font = nullptr;                           //the handle to the font to render with
	DNvec4 m_textColor = {1.0f, 1.0f, 1.0f, 1.0f};        //the text's color
	float m_textThickness = 0.5f;                         //the thickness of the text
	DNvec4 m_textOutlineColor = {0.0f, 0.0f, 0.0f, 1.0f}; //the color of the text's outline
	float m_textOutlineThickness = 1.0f;                  //the thickness at which the text's outline begins
	float m_textPadding = 10.0f;                          //space between edge of background and start of text, in pixels

	TextBox() = default;
	TextBox(Coordinate x, Coordinate y, Dimension w, Dimension h,
		    std::string text, DNUIfont* font, DNvec4 textColor = {1.0f, 1.0f, 1.0f, 1.0f}, 
		    float textThickness = 0.5f, DNvec4 textOutlineColor = {0.0f, 0.0f, 0.0f, 0.0f}, 
		    float textOutlineThickness = 1.0f, float textPadding = 10.0f,
			DNvec4 backgroundColor = {1.0f, 1.0f, 1.0f, 1.0f}, float backgroundCornerRad = 0.0f, 
			DNvec4 backgroundOutlineColor = {0.0f, 0.0f, 0.0f, 1.0f}, 
			float backgroundOutlineThickness = 0.0f, DNvec4 cursorColor = {1.0f, 1.0f, 1.0f, 0.5f});
	~TextBox();

	/* Call whenever the state of the control keys changes
	 * @param shiftHeld whether or not the shfit key is currently held
	 * @param ctrlHeld whether or not the ctrl key is currently held
	 */
	static void set_control_key_state(bool shiftHeld, bool ctrlHeld);

	//returns the highlighted portion of the text, or the entire string if none is highlighted
	std::string get_highlighted();

	void update(float dt, DNvec2 parentPos, DNvec2 parentSize);
	void render(float parentAlphaMult);
	void handle_event(Event event);

protected:
	bool m_hasFocus = false; //whether or not the textbox has keyboard focus
	int m_cursorPos = 0;     //the index of the character the cursor is on
	int m_highlightLen = 0;  //the number of characters highlighted

	DNvec2 m_textSize;   //the rendered size of the text at 1.0 scale
	float m_renderScale; //the final scale of text

private:
	inline static bool s_shiftHeld; //whether the shift key is currently held
	inline static bool s_ctrlHeld;  //whether the ctrl key is currently held

	const float CURSOR_SIZE = 7.5f; //the rendered size of the cursor, in pixels
	Transition m_cursorTransition; //the transition used to animate the cursor's position
	//returns the rendered position of the cursor, given its position in the text
	float get_cursor_render_pos(int cursorPos);
	//returns the position of the cursor in the text, given its rendered position
	int get_cursor_pos(float cursorRenderPos); 

	int m_lastCursorPos = 0;     //the cursor position last frame, used to determine if an animation update is needed
	int m_orgCursorPos = 0;      //original cursor position when highlighting started
	bool m_highlighting = false; //whether the user us currently dragging their mouse to highlight next

	std::vector<DNvec2> m_charPositons; //the starting and ending x position of each chararcter when rendered

	float m_time = 0.0f; //used for cursor animation
	float m_cursorAlpha; //the alpha value of the cursor, used to keep track when it is set to 0
};

}; //namespace dnui

#endif