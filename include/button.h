#pragma once

#include <SFML/Graphics.hpp>

#include <string>


class Button
{
  public:
    Button(float x, float y, float width, float height);
    Button();
    void setText(const std::wstring& text);

    void draw(sf::RenderWindow& window);
    bool isPressed(float mouseX, float mouseY);

    void setFontColor(const sf::Color& color);
    void setColor(const sf::Color& color);
    void setFontSize(int fontSize);
    void setPosition(const sf::Vector2f& position);
    void setSize(const sf::Vector2f& size);

  private:
    sf::Font font_;
    sf::Text text_;
    sf::RectangleShape shape_;
    void alignText_();
};

