#include "list.hpp"

dnui::List::List(Coordinate x, Coordinate y, Dimension w, Dimension h, 
                 Dimension xPadding, Dimension yPadding, Dimension xEdgePadding, Dimension yEdgePadding,
				 int itemsPerLine, bool smoothScroll) : dnui::Element(x, y, w, h)
{
	m_xPadding = xPadding;
	m_yPadding = yPadding;
	m_xEdgePadding = xEdgePadding;
	m_yEdgePadding = yEdgePadding;
	m_itemsPerLine = itemsPerLine;
	m_smoothScroll = smoothScroll;
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
			delete m_listItems[i].second;
			m_listItems.erase(m_listItems.begin() + i);
			break;
		}
}

void dnui::List::update(float dt, DNvec2 parentPos, DNvec2 parentSize)
{
	//calculate padding:
	DNvec2 padding;
	if(m_xPadding.type == Dimension::ASPECT)
	{
		float h = m_yPadding.calc_render_size(m_renderSize.y);
		padding = {h * m_xPadding.aspectRatio, h};
	}
	else if(m_yPadding.type == Dimension::ASPECT)
	{
		float w = m_xPadding.calc_render_size(m_renderSize.x);
		padding = {w, w * m_yPadding.aspectRatio};
	}
	else
		padding = {m_xPadding.calc_render_size(m_renderSize.x), m_yPadding.calc_render_size(m_renderSize.y)};

	//calculate edge padding:
	DNvec2 edgePadding;
	if(m_xEdgePadding.type == Dimension::ASPECT)
	{
		float h = m_yEdgePadding.calc_render_size(m_renderSize.y);
		edgePadding = {h * m_xEdgePadding.aspectRatio, h};
	}
	else if(m_yEdgePadding.type == Dimension::ASPECT)
	{
		float w = m_xEdgePadding.calc_render_size(m_renderSize.x);
		edgePadding = {w, w * m_yEdgePadding.aspectRatio};
	}
	else
		edgePadding = {m_xEdgePadding.calc_render_size(m_renderSize.x), m_yEdgePadding.calc_render_size(m_renderSize.y)};

	//calculate items per line:
	float itemSpace = m_renderSize.x - edgePadding.x * 2.0f;
	int itemsPerLine;
	if(m_itemsPerLine < 0)
		itemsPerLine = (int)floorf(itemSpace / padding.x);
	else
		itemsPerLine = m_itemsPerLine;

	//calculate padding:
	if(itemsPerLine == 1)
		edgePadding.x += itemSpace * 0.5f;
	else
		padding.x = itemSpace / (itemsPerLine - 1);

	//update scrolling:
	if(m_renderSize.y > 0.0f)
	{
		float listHeight = 2.0f * edgePadding.y + padding.y * ((m_listItems.size() - 1) / itemsPerLine);	
		
		if(listHeight < m_renderSize.y)
			m_scrollTargetPos = 0.0f;
		else
		{
			m_scrollTargetPos = fmaxf(m_scrollTargetPos, 1.0f - listHeight / m_renderSize.y);
			m_scrollTargetPos = fminf(m_scrollTargetPos, 0.0f);
		}
	}

	if(m_smoothScroll)
		m_scrollPos += (m_scrollTargetPos - m_scrollPos) * (1.0f - powf(0.99f, dt));
	else
		m_scrollPos = m_scrollTargetPos;

	//set list item positions and update:
	for(int i = 0; i < m_listItems.size(); i++)
	{
		float xPos = edgePadding.x + padding.x * (i % itemsPerLine);	
		float yPos = edgePadding.y + padding.y * (i / itemsPerLine) + m_scrollPos * m_renderSize.y;
		xPos -= m_renderSize.x * 0.5f;
		yPos -= m_renderSize.y * 0.5f;

		m_listItems[i].second->m_xPos = Coordinate(Coordinate::PIXELS,  xPos, Coordinate::CENTER_CENTER);
		m_listItems[i].second->m_yPos = Coordinate(Coordinate::PIXELS, -yPos, Coordinate::CENTER_CENTER);

		m_listItems[i].second->m_active = m_active;

		m_listItems[i].second->update(dt, m_renderPos, m_renderSize);
	}

	dnui::Element::update(dt, parentPos, parentSize);
}

void dnui::List::render(float parentAlphaMult)
{
	for(int i = 0; i < m_listItems.size(); i++)
		m_listItems[i].second->render(m_alphaMult * parentAlphaMult);
	
	dnui::Element::render(parentAlphaMult);
}

void dnui::List::handle_event(Event event)
{
	if(event.type == Event::SCROLL && m_active && is_hovered())
		m_scrollTargetPos += event.scroll.dir;

	for(int i = 0; i < m_listItems.size(); i++)
		m_listItems[i].second->handle_event(event);

	dnui::Element::handle_event(event);
}