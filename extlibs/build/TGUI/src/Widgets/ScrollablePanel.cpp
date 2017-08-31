/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2017 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <TGUI/Widgets/ScrollablePanel.hpp>
#include <TGUI/Clipping.hpp>
#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    static std::map<std::string, ObjectConverter> defaultRendererValues =
            {
                {"bordercolor", sf::Color::Black},
                {"backgroundcolor", sf::Color::Transparent}
            };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ScrollablePanel::ScrollablePanel(const Layout2d& size, sf::Vector2f contentSize) :
        Panel{size}
    {
        m_type = "ScrollablePanel";

        m_renderer = aurora::makeCopied<ScrollablePanelRenderer>();
        setRenderer(RendererData::create(defaultRendererValues));

        // Rotate the horizontal scrollbar
        m_horizontalScrollbar.setSize(m_horizontalScrollbar.getSize().y, m_horizontalScrollbar.getSize().x);

        setContentSize(contentSize);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ScrollablePanel::Ptr ScrollablePanel::create(Layout2d size, sf::Vector2f contentSize)
    {
        return std::make_shared<ScrollablePanel>(size, contentSize);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ScrollablePanel::Ptr ScrollablePanel::copy(ScrollablePanel::ConstPtr panel)
    {
        if (panel)
            return std::static_pointer_cast<ScrollablePanel>(panel->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::setSize(const Layout2d& size)
    {
        Panel::setSize(size);
        updateScrollbars();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::add(const tgui::Widget::Ptr& widget, const sf::String& widgetName)
    {
        Panel::add(widget, widgetName);

        const sf::Vector2f bottomRight = widget->getPosition() + widget->getFullSize();
        if (m_contentSize == sf::Vector2f{0, 0})
        {
            if (bottomRight.x > m_mostBottomRightPosition.x)
                m_mostBottomRightPosition.x = bottomRight.x;
            if (bottomRight.y > m_mostBottomRightPosition.y)
                m_mostBottomRightPosition.y = bottomRight.y;

            updateScrollbars();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ScrollablePanel::remove(const Widget::Ptr& widget)
    {
        const sf::Vector2f bottomRight = widget->getPosition() + widget->getFullSize();

        const bool ret = Panel::remove(widget);

        if (m_contentSize == sf::Vector2f{0, 0})
        {
            if ((bottomRight.x == m_mostBottomRightPosition.x) || (bottomRight.y == m_mostBottomRightPosition.y))
            {
                recalculateMostBottomRightPosition();
                updateScrollbars();
            }
        }

        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::removeAllWidgets()
    {
        Panel::removeAllWidgets();

        if (m_contentSize == sf::Vector2f{0, 0})
            updateScrollbars();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::setContentSize(sf::Vector2f size)
    {
        m_contentSize = size;

        if (m_contentSize == sf::Vector2f{0, 0})
            recalculateMostBottomRightPosition();

        updateScrollbars();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f ScrollablePanel::getContentSize() const
    {
        if (m_contentSize != sf::Vector2f{0, 0})
            return m_contentSize;
        else if (m_widgets.empty())
            return getInnerSize();
        else
            return m_mostBottomRightPosition;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::leftMousePressed(sf::Vector2f pos)
    {
        m_mouseDown = true;

        if (m_verticalScrollbar.mouseOnWidget(pos - getPosition()))
            m_verticalScrollbar.leftMousePressed(pos - getPosition());
        else if (m_horizontalScrollbar.mouseOnWidget(pos - getPosition()))
            m_horizontalScrollbar.leftMousePressed(pos - getPosition());
        else if (sf::FloatRect{getPosition().x + getChildWidgetsOffset().x, getPosition().y + getChildWidgetsOffset().y, getInnerSize().x, getInnerSize().y}.contains(pos))
        {
            Panel::leftMousePressed({pos.x + static_cast<float>(m_horizontalScrollbar.getValue()),
                                     pos.y + static_cast<float>(m_verticalScrollbar.getValue())});
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::leftMouseReleased(sf::Vector2f pos)
    {
        if (m_verticalScrollbar.mouseOnWidget(pos - getPosition()))
            m_verticalScrollbar.leftMouseReleased(pos - getPosition());
        else if (m_horizontalScrollbar.mouseOnWidget(pos - getPosition()))
            m_horizontalScrollbar.leftMouseReleased(pos - getPosition());
        else if (sf::FloatRect{getPosition().x + getChildWidgetsOffset().x, getPosition().y + getChildWidgetsOffset().y, getInnerSize().x, getInnerSize().y}.contains(pos))
        {
            Panel::leftMouseReleased({pos.x + static_cast<float>(m_horizontalScrollbar.getValue()),
                                      pos.y + static_cast<float>(m_verticalScrollbar.getValue())});
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::mouseMoved(sf::Vector2f pos)
    {
        // Check if the mouse event should go to the scrollbar
        if ((m_verticalScrollbar.isMouseDown() && m_verticalScrollbar.isMouseDownOnThumb()) || m_verticalScrollbar.mouseOnWidget(pos - getPosition()))
        {
            m_verticalScrollbar.mouseMoved(pos - getPosition());
        }
        else if ((m_horizontalScrollbar.isMouseDown() && m_horizontalScrollbar.isMouseDownOnThumb()) || m_horizontalScrollbar.mouseOnWidget(pos - getPosition()))
        {
            m_horizontalScrollbar.mouseMoved(pos - getPosition());
        }
        else // Mouse not on scrollbar or dragging the scrollbar thumb
        {
            if (sf::FloatRect{getPosition().x + getChildWidgetsOffset().x, getPosition().y + getChildWidgetsOffset().y, getInnerSize().x, getInnerSize().y}.contains(pos))
            {
                Panel::mouseMoved({pos.x + static_cast<float>(m_horizontalScrollbar.getValue()),
                                   pos.y + static_cast<float>(m_verticalScrollbar.getValue())});
            }

            m_verticalScrollbar.mouseNoLongerOnWidget();
            m_horizontalScrollbar.mouseNoLongerOnWidget();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::mouseWheelScrolled(float delta, sf::Vector2f pos)
    {
        if (m_horizontalScrollbar.isShown() && m_horizontalScrollbar.mouseOnWidget(pos - getPosition()))
        {
            m_horizontalScrollbar.mouseWheelScrolled(delta, pos - getPosition());
            mouseMoved(pos);
        }
        else if (m_verticalScrollbar.isShown())
        {
            m_verticalScrollbar.mouseWheelScrolled(delta, pos - getPosition());
            mouseMoved(pos);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::mouseNoLongerOnWidget()
    {
        Panel::mouseNoLongerOnWidget();
        m_verticalScrollbar.mouseNoLongerOnWidget();
        m_horizontalScrollbar.mouseNoLongerOnWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::mouseNoLongerDown()
    {
        Panel::mouseNoLongerDown();
        m_verticalScrollbar.mouseNoLongerDown();
        m_horizontalScrollbar.mouseNoLongerDown();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

        const auto oldStates = states;

        // Draw the borders
        if (m_bordersCached != Borders{0})
        {
            drawBorders(target, states, m_bordersCached, getSize(), m_borderColorCached);
            states.transform.translate({m_bordersCached.getLeft(), m_bordersCached.getTop()});
        }

        // Draw the background
        const sf::Vector2f innerSize = {getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight(),
                                        getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom()};
        drawRectangleShape(target, states, innerSize, m_backgroundColorCached);

        states.transform.translate(m_paddingCached.getLeft(), m_paddingCached.getTop());
        sf::Vector2f contentSize = {innerSize.x - m_paddingCached.getLeft() - m_paddingCached.getRight(),
                                    innerSize.y - m_paddingCached.getTop() - m_paddingCached.getBottom()};

        if (m_verticalScrollbar.getMaximum() > m_verticalScrollbar.getLowValue())
            contentSize.x -= m_verticalScrollbar.getSize().x;
        if (m_horizontalScrollbar.getMaximum() > m_horizontalScrollbar.getLowValue())
            contentSize.y -= m_horizontalScrollbar.getSize().y;

        // Draw the child widgets
        {
            const Clipping clipping{target, states, {}, contentSize};

            states.transform.translate(-static_cast<float>(m_horizontalScrollbar.getValue()),
                                       -static_cast<float>(m_verticalScrollbar.getValue()));

            drawWidgetContainer(&target, states);
        }

        m_verticalScrollbar.draw(target, oldStates);
        m_horizontalScrollbar.draw(target, oldStates);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::rendererChanged(const std::string& property)
    {
        if (property == "scrollbar")
        {
            m_verticalScrollbar.setRenderer(getRenderer()->getScrollbar());
            m_horizontalScrollbar.setRenderer(getRenderer()->getScrollbar());
        }
        else
            Panel::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::updateScrollbars()
    {
        const sf::Vector2f visibleSize = getInnerSize();
        m_horizontalScrollbar.setLowValue(visibleSize.x);
        m_verticalScrollbar.setLowValue(visibleSize.y);

        const sf::Vector2f contentSize = getContentSize();
        m_horizontalScrollbar.setMaximum(contentSize.x);
        m_verticalScrollbar.setMaximum(contentSize.y);

        const bool horizontalScrollbarVisible = m_horizontalScrollbar.getMaximum() > m_horizontalScrollbar.getLowValue();
        if (horizontalScrollbarVisible)
        {
            m_verticalScrollbar.setSize(m_verticalScrollbar.getSize().x, getInnerSize().y - m_horizontalScrollbar.getSize().y);
            m_verticalScrollbar.setLowValue(m_verticalScrollbar.getLowValue() - m_horizontalScrollbar.getSize().y);

            const bool verticalScrollbarVisible = m_verticalScrollbar.getMaximum() > m_verticalScrollbar.getLowValue();
            if (verticalScrollbarVisible)
                m_horizontalScrollbar.setSize(getInnerSize().x - m_verticalScrollbar.getSize().x, m_horizontalScrollbar.getSize().y);
            else
                m_horizontalScrollbar.setSize(getInnerSize().x, m_horizontalScrollbar.getSize().y);
        }
        else
        {
            m_verticalScrollbar.setSize(m_verticalScrollbar.getSize().x, getInnerSize().y);

            const bool verticalScrollbarVisible = m_verticalScrollbar.getMaximum() > m_verticalScrollbar.getLowValue();
            if (verticalScrollbarVisible)
            {
                m_horizontalScrollbar.setSize(getInnerSize().x - m_verticalScrollbar.getSize().x, m_horizontalScrollbar.getSize().y);
                m_horizontalScrollbar.setLowValue(m_horizontalScrollbar.getLowValue() - m_verticalScrollbar.getSize().x);

                if (m_horizontalScrollbar.getMaximum() > m_horizontalScrollbar.getLowValue())
                    m_verticalScrollbar.setSize(m_verticalScrollbar.getSize().x, getInnerSize().y - m_horizontalScrollbar.getSize().y);
            }
            else
                m_horizontalScrollbar.setSize(getInnerSize().x, m_horizontalScrollbar.getSize().y);
        }

        m_verticalScrollbar.setPosition(getChildWidgetsOffset().x + getInnerSize().x - m_verticalScrollbar.getSize().x, getChildWidgetsOffset().y);
        m_horizontalScrollbar.setPosition(getChildWidgetsOffset().x, getChildWidgetsOffset().y + getInnerSize().y - m_horizontalScrollbar.getSize().y);

        m_verticalScrollbar.setScrollAmount(static_cast<unsigned int>(std::ceil((m_verticalScrollbar.getMaximum() - m_verticalScrollbar.getLowValue()) / 50.f)));
        m_horizontalScrollbar.setScrollAmount(static_cast<unsigned int>(std::ceil((m_verticalScrollbar.getMaximum() - m_verticalScrollbar.getLowValue()) / 50.f)));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ScrollablePanel::recalculateMostBottomRightPosition()
    {
        for (const auto& widget : m_widgets)
        {
            const sf::Vector2f bottomRight = widget->getPosition() + widget->getFullSize();
            if (bottomRight.x > m_mostBottomRightPosition.x)
                m_mostBottomRightPosition.x = bottomRight.x;
            if (bottomRight.y > m_mostBottomRightPosition.y)
                m_mostBottomRightPosition.y = bottomRight.y;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
