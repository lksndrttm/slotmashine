#pragma once
#include <SFML/Graphics.hpp>

class MessageBox
{
  public:
    MessageBox();
    void draw(sf::RenderWindow& window);

    sf::Vector2f size() const { return background_.getSize(); };
    void setSize(const sf::Vector2f& size);

    sf::Vector2f position() const { return background_.getPosition(); }
    void setPosition(const sf::Vector2f& position);

    void setMessageTextColor(const sf::Color& color) { messageText_.setFillColor(color); }
    void setMessageText(const std::wstring& message);
    void setFillColor(const sf::Color& color) { background_.setFillColor(color); }

  private:
    sf::RectangleShape background_;
    sf::Font messageFont_;
    sf::Text messageText_;
    void align_();
};
