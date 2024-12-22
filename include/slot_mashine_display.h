#pragma once
#include "reel.h"
#include "slot.h"
#include <functional>
#include <vector>

#include <SFML/Graphics.hpp>

class SlotMashineDisplay
{
  public:
    SlotMashineDisplay(float width, float height, int reelsCount, const sf::Vector2f& position = {0.0f,0.0f});
    void draw(sf::RenderWindow& window);
    void requestStart();
    void requestStop();
    void updateSize(float wwidth, float wheight);

    void setBackgroundColor(const sf::Color& color);
    void setPosition(const sf::Vector2f& position, int wwidth, int wheight);
    void setSlotGenerator(const std::function<std::unique_ptr<sf::Shape>(float,float)>& generator);

    sf::Vector2f position() const { return displayRect_.getPosition(); }
    float width() const { return width_; }
    float height() const { return height_; }
    std::vector<std::vector<Slot>> displayedSlotRows() const;

    bool isStarted() const;
    bool isStopped() const;

  private:
    std::vector<Reel> reels_;
    float spacing_ = 40;
    float width_;
    float height_;
    sf::RectangleShape displayRect_;
    sf::View displayView_;
};
