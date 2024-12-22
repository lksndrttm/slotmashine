#include "button.h"

void Button::alignText_()
{
    text_.setOrigin(text_.getLocalBounds().width/2.0f, text_.getLocalBounds().height/2.0f);
    text_.setPosition(shape_.getPosition().x + shape_.getSize().x/2.0f, shape_.getPosition().y + shape_.getSize().y/2.0f);
}

Button::Button(float x, float y, float width, float height)
{
    if (!font_.loadFromFile("data/Monocraft.otf"))
    {
        throw std::runtime_error("Button creation error. Cant load font.");
    }
    shape_ = sf::RectangleShape({width, height});
    shape_.setPosition(x,y);

    text_.setFont(font_);
    text_.setCharacterSize(32);

    alignText_();
}

Button::Button()
{
    if (!font_.loadFromFile("data/Monocraft.otf"))
    {
        throw std::runtime_error("Button creation error. Cant load font.");
    }
    shape_ = sf::RectangleShape({200, 100});
    shape_.setPosition(0,0);

    text_.setFont(font_);
    text_.setCharacterSize(32);

    alignText_();
}

void Button::setText(const std::wstring& text)
{
    text_.setString(text);
    alignText_();
}

void Button::setPosition(const sf::Vector2f& position)
{
    shape_.setPosition(position);
    alignText_();
}

void Button::setSize(const sf::Vector2f& size)
{
    shape_.setSize(size);
    alignText_();
}

void Button::draw(sf::RenderWindow& window)
{
    window.draw(shape_);
    window.draw(text_);
}

bool Button::isPressed(float mouseX, float mouseY)
{
    float bx = shape_.getPosition().x;
    float by = shape_.getPosition().y;
    sf::FloatRect bRect (bx, by, shape_.getSize().x, shape_.getSize().y);
    bool buttonPressed = bRect.contains(mouseX, mouseY);

    return buttonPressed;
    
}


void Button::setFontColor(const sf::Color& color)
{
    text_.setFillColor(color);
}

void Button::setColor(const sf::Color& color)
{
    shape_.setFillColor(color);
}

void Button::setFontSize(int fontSize)
{
    text_.setCharacterSize(fontSize);
    alignText_();
}

