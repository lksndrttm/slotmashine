#include "message_box.h"

MessageBox::MessageBox()
{
 if (!messageFont_.loadFromFile("data/Monocraft.otf"))
    {
        throw std::runtime_error("MessageBox creation error. Cant load font.");
    }
    background_.setSize({300.0f,100.0f});

    messageText_.setFont(messageFont_);
    messageText_.setCharacterSize(32);
    messageText_.setPosition(background_.getPosition());
    messageText_.setFillColor(sf::Color::Blue);

    messageText_.setString("Message Text.");
    align_();
}

void MessageBox::draw(sf::RenderWindow& window)
{
    window.draw(background_);
    window.draw(messageText_);
}

void MessageBox::setSize(const sf::Vector2f& size)
{
    background_.setSize(size);
    align_();
}

void MessageBox::setPosition(const sf::Vector2f& position)
{
    background_.setPosition(position);
    align_();
}

void MessageBox::setMessageText(const std::wstring& message)
{
    messageText_.setString(message);
    align_();
}

void MessageBox::align_()
{
    messageText_.setOrigin(messageText_.getLocalBounds().width/2.0f, messageText_.getLocalBounds().height/2.0f);
    messageText_.setPosition(background_.getPosition().x + background_.getSize().x/2.0f, background_.getPosition().y + background_.getSize().y/2.0f);
}
