#ifndef LIST_H
#define LIST_H

#include "../element.hpp"
#include <utility>

namespace dnui
{

//a container that positions a list of elements in a grid pattern, has no rendering of its own
class List : public Element
{
public:
	Dimension m_xPadding     = Dimension(dnui::Dimension::PIXELS, 0.0f); //the amount of space in between each list element on the x-axis
	Dimension m_yPadding     = Dimension(dnui::Dimension::PIXELS, 0.0f); //the amount of space in between each list element on the y-axis
	Dimension m_xEdgePadding = Dimension(dnui::Dimension::PIXELS, 0.0f); //the amount of space between the edge of the list and the elements on its ends on the x-axis
	Dimension m_yEdgePadding = Dimension(dnui::Dimension::PIXELS, 0.0f); //the amount of space between the edge of the list and the elements on its ends on the y-axis

	int m_itemsPerLine = -1;     //the number of items per line, or -1 to determine this automatically
	bool m_smoothScroll = false; //whether or not to smooth out the scrolling of the list

	List() = default;
	List(Coordinate x, Coordinate y, Dimension w, Dimension h, 
	     Dimension xPadding, Dimension yPadding, Dimension xEdgePadding, Dimension yEdgePadding,
		 int itemsPerLine = -1, bool smoothScroll = false);
	~List();

	/* Adds an item to the list with an identifier. NOTE: added items are automatically freed when the list's destructor is called, do NOT free them yourself
	 * @param item the ui element to be added
	 * @param id the identifier of the item, does not need to be unique
	 */
	void add_item(Element* item, int id);
	/* Gets the first element in the list with a given identifier
	 * @param id the identifier to search for
	 * @returns the element with the given identifier, or nullptr if none was found
	 */
	Element* get_item(int id);
	/* Removes the first element in the list with a given identifier. NOTE: removed items are automatically freed, do NOT free them yourself
	 * @param id the identifier to remove
	 */
	void remove_item(int id);

	void update(float dt, DNvec2 parentPos, DNvec2 parentSize);
	void render(float parentAlphaMult);
	void handle_event(Event event);

protected:
	std::vector<std::pair<int, Element*>> m_listItems; //all of the list's items

	float m_scrollPos = 0.0f;       //the current scroll position
	float m_scrollTargetPos = 0.0f; //the target scroll position
};

}; //namespace dnui

#endif