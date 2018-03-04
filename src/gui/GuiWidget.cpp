#include "gui/GuiWidget.h"
#include "gui/GuiLayout.h"

GuiWidget::GuiWidget() : mParent(nullptr), mVisible(true), mDirty(false)
{
    //ctor
}

GuiWidget::~GuiWidget()
{

}

void GuiWidget::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (mVisible)
    {
        render(target, states);
        for (const GuiWidget* widget : mChildren)
            target.draw(*widget);
    }
}

void GuiWidget::update()
{
    if (mLayout != nullptr)
    {
        mLayout->align();
        for (GuiWidget* widget : mChildren)
            widget->update();
    }
    mDirty = false;
}

void GuiWidget::add(GuiWidget* widget)
{
    widget->setParent(this);
    mChildren.push_back(widget);
    setDirty();
}

std::vector<GuiWidget*>& GuiWidget::getChildren()
{
    return mChildren;
}

const std::vector<GuiWidget*>& GuiWidget::getChildren() const
{
    return mChildren;
}

void GuiWidget::fitSizeToContent()
{
    setSize(mLayout->computeSize());
}

void GuiWidget::setParent(GuiWidget* parent)
{
    mParent = parent;
}

void GuiWidget::setLayout(std::unique_ptr<GuiLayout> layout)
{
    mLayout = std::move(layout);
    mLayout->setOwner(this);
    setDirty();
}

sf::Vector2f GuiWidget::getPosition() const
{
    return mPosition;
}

void GuiWidget::setPosition(sf::Vector2f position)
{
    mPosition = position;
    setDirty();
}

sf::Vector2f GuiWidget::getSize() const
{
    return mSize;
}

void GuiWidget::setSize(sf::Vector2f size)
{
    mSize = size;
    setDirty();
}

sf::FloatRect GuiWidget::getRect() const
{
    return sf::FloatRect(mPosition, mSize);
}

bool GuiWidget::isVisible() const
{
    return mVisible;
}

void GuiWidget::setVisible(bool visible)
{
    mVisible = visible;
}

bool GuiWidget::isDirty() const
{
    return mDirty;
}

void GuiWidget::updateMouseMoved(sf::Vector2f position)
{
    if (mVisible)
    {
        onHover(position);
        for (GuiWidget* widget : mChildren)
            widget->updateMouseMoved(position);
    }
}

void GuiWidget::updateMouseButtonPressed(sf::Vector2f position)
{
    if (mVisible)
    {
        onPress(position);
        for (GuiWidget* widget : mChildren)
            widget->updateMouseButtonPressed(position);
    }
}

void GuiWidget::updateMouseButtonReleased(sf::Vector2f position)
{
    if (mVisible)
    {
        onRelease(position);
        for (GuiWidget* widget : mChildren)
            widget->updateMouseButtonReleased(position);
    }
}

void GuiWidget::setDirty()
{
    mDirty = true;
    if (mParent != nullptr)
        mParent->setDirty();
}

void GuiWidget::render(sf::RenderTarget& target, sf::RenderStates states) const
{

}

void GuiWidget::onHover(sf::Vector2f position)
{

}

void GuiWidget::onPress(sf::Vector2f position)
{

}

void GuiWidget::onRelease(sf::Vector2f position)
{

}
