#pragma once

#include <SFML/Graphics.hpp>

class Slot : public sf::RectangleShape
{
  public:
    Slot(float width, float height, int slotType) : sf::RectangleShape({height, width}), slotType_(slotType) {}
    int getSlotType() { return slotType_; }

  private:
    int slotType_;
};
