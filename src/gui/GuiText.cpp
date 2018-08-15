#include "gui/GuiText.h"
#include "resource/XmlDocument.h"

GuiText::GuiText(const sf::String& text, unsigned int characterSize, const XmlDocument* style) :
    GuiWidget(style),
    mText(text, mStyle->getFirstChildByName("text").getAttributes().get<const sf::Font&>("font"), characterSize)
{
    mText.setFillColor(mStyle->getFirstChildByName("text").getAttributes().get<sf::Color>("color"));
    computeSize();
    mFixedSize = true;
}

GuiText::GuiText(const PropertyList& properties) : GuiWidget(properties)
{
    const XmlDocument* style = properties.get<const XmlDocument*>("style");
    mText.setString(properties.get<sf::String>("text", ""));
    mText.setFont(style->getFirstChildByName("text").getAttributes().get<const sf::Font&>("font"));
    mText.setCharacterSize(properties.get<unsigned int>("characterSize", 0));
    mText.setFillColor(style->getFirstChildByName("text").getAttributes().get<sf::Color>("color"));
    computeSize();
    mFixedSize = true;
}

GuiText::~GuiText()
{
    //dtor
}

void GuiText::setPosition(sf::Vector2f position)
{
    GuiWidget::setPosition(position);
    mText.setPosition(sf::Vector2f(sf::Vector2i(position)));
}

void GuiText::setCharacterSize(unsigned int characterSize)
{
    mText.setCharacterSize(characterSize);
    computeSize();
}

void GuiText::setText(const sf::String& text)
{
    if (text != mText.getString())
    {
        mText.setString(text);
        computeSize();
        setDirty();
    }
}

void GuiText::setColor(sf::Color color)
{
    mText.setFillColor(color);
}

void GuiText::render(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(mText);
}

void GuiText::computeSize()
{
    mSize = sf::Vector2f(mText.getGlobalBounds().width, mText.getCharacterSize() * 5 / 4);
}
