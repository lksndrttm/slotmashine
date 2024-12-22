#include "slot_mashine_display.h"


SlotMashineDisplay::SlotMashineDisplay(float width, float height, int reelsCount, const sf::Vector2f& position) :
    width_(width),
    height_(height),
    displayRect_({width, height})
{
    displayRect_.setPosition(position);
    reels_.reserve(reelsCount);

    float reelWidth = 170;
    float slotHeight = reelWidth;

    float slotVertSpacing = 30;
    float reelYOffset = -slotHeight - slotVertSpacing/2;

    float spacing = (width_ - (reelWidth * reelsCount)) / reelsCount;
    float currentXOffset = spacing/2;

    for(int i = 0; i < reelsCount; ++i) {
        reels_.emplace_back(5, reelWidth, reelWidth, sf::Vector2f(currentXOffset, reelYOffset));
        reels_[i].setSpeed(4 + (0.5 * i));
        currentXOffset += reelWidth + spacing;
    }
}


void SlotMashineDisplay::draw(sf::RenderWindow& window)
{
    window.draw(displayRect_);
    window.setView(displayView_);
    for(auto& reel : reels_) {
        reel.draw(window);
    }
    window.setView(window.getDefaultView());
}

void SlotMashineDisplay::requestStart()
{
    for(auto& reel : reels_) {
        reel.requestStartSpinning();
    }
}

void SlotMashineDisplay::requestStop()
{
    for(auto& reel : reels_) {
        reel.requestStopSpinning();
    }
}

void SlotMashineDisplay::updateSize(float wwidth, float wheight)
{
    float x1 = displayRect_.getPosition().x;
    float y1 = displayRect_.getPosition().y;
    float x2 = x1 + displayRect_.getSize().x;
    float y2 = y1 + displayRect_.getSize().y;

    float relX1 = displayRect_.getPosition().x / wwidth;
    float relY1 = displayRect_.getPosition().y / wheight;
    float relX2 = displayRect_.getSize().x / wwidth; 
    float relY2 = displayRect_.getSize().y / wheight;

    auto visibleArea = sf::FloatRect(0.0f,0.0f, displayRect_.getSize().x , displayRect_.getSize().y);
    displayView_ = sf::View(visibleArea);
    displayView_.setViewport({relX1,relY1,relX2,relY2});
}


void SlotMashineDisplay::setBackgroundColor(const sf::Color& color)
{
    displayRect_.setFillColor(color);
}

void SlotMashineDisplay::setPosition(const sf::Vector2f& position, int wwidth, int wheight)
{ 
    displayRect_.setPosition(position);
    updateSize(wwidth, wheight);
}

void SlotMashineDisplay::setSlotGenerator(const std::function<std::unique_ptr<sf::Shape>(float,float)>& generator)
{
    for(auto& reel : reels_) {
        reel.setSlotGenerator(generator);
    }
}

std::vector<std::vector<Slot>> SlotMashineDisplay::displayedSlotRows() const
{
    std::vector<std::vector<Slot>> res;
    res.reserve(3);
    for(int i = 0; i < 3; ++i) {
        res.push_back({});
        res[i].reserve(reels_.size());
        for(int j = 0; j < reels_.size(); ++j) {
            Slot slot = *dynamic_cast<Slot*>(reels_[j].slots()[i+1].get());
            res[i].push_back(slot);
        }
    }
    return res;

}

bool SlotMashineDisplay::isStarted() const
{
    for(auto& reel : reels_) {
        if( !reel.isStarted() ) {
            return false;
        }
    }
    return true;
}

bool SlotMashineDisplay::isStopped() const
{
    for(auto& reel : reels_) {
        if(!reel.isStopped()) {
            return false;
        }
    }
    return true;
}
