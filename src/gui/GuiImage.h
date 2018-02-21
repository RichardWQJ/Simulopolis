#pragma once

#include "SFML/Graphics.hpp"
#include "gui/GuiWidget.h"

class GuiImage : public GuiWidget
{
public:
    GuiImage(const sf::Texture& texture);
    virtual ~GuiImage();

    virtual void setPosition(sf::Vector2f position) override;
    virtual void setSize(sf::Vector2f size) override;

protected:
    virtual void render(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    sf::Sprite mSprite;
};
