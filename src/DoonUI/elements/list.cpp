#include "list.hpp"

dnui::List::List(Coordinate x, Coordinate y, Dimension w, Dimension h, 
                 Dimension xPadding, Dimension yPadding, Dimension xEdgePadding, Dimension yEdgePadding,
				 int itemsPerLine = -1, bool vertical = true) : dnui::Element(x, y, w, h)
{
	m_xPadding = xPadding;
	m_yPadding = yPadding;
	m_xEdgePadding = xEdgePadding;
	m_yEdgePadding = yEdgePadding;
	m_itemsPerLine = itemsPerLine;
	m_vertical = vertical;
}

dnui::List::~List()
{
	for(int i = 0; i < m_listItems.size(); i++)
		delete m_listItems[i].second;
}

void dnui::List::add_item(Element* item, int id)
{
	m_listItems.push_back(std::make_pair(id, item));
}

dnui::Element* dnui::List::get_item(int id)
{
	for(int i = 0; i < m_listItems.size(); i++)
		if(m_listItems[i].first == id)
			return m_listItems[i].second;

	return nullptr;
}

void dnui::List::remove_item(int id)
{
	for(int i = 0; i < m_listItems.size(); i++)
		if(m_listItems[i].first == id)
		{
			m_listItems.erase(m_listItems.begin() + i);
			break;
		}
}

void dnui::List::update(float dt, DNvec2 parentPos, DNvec2 parentSize)
{
	float lineSize = m_renderSize.x;
	float itemEdgePadding = m_xEdgePadding.calc_render_size(m_renderSize.y);
	float lineEdgePadding = m_yEdgePadding.calc_render_size(m_renderSize.y);

	int itemsPerLine;
	float itemPadding;
	float linePadding;

	if(m_itemsPerLine < 0)
	{
		float freeItemSpace = lineSize - itemEdgePadding * 2.0f;
		itemsPerLine = (int)floorf(freeItemSpace / m_xEdgePadding.calc_render_size(lineSize));
	}
	else
		itemsPerLine = m_itemsPerLine;

	

	for(int i = 0; i < m_listItems.size(); i++)
	{

	}

	dnui::Element::update(dt, parentPos, parentSize);
}