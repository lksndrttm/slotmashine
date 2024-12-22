#include "reel.h"

#include <cmath>

ReelState::ReelState(Reel& reel) : reel_(reel) {}



void ReelState::setReelState(std::unique_ptr<ReelState> state)
{
    reel_.setState(std::move(state));
}

void ReelStopped::spin()
{
    return;
}

ReelStopping::ReelStopping(Reel& reel) : ReelState(reel) 
{
    initialPos_ = reel_.position();
}
    
void ReelStopping::spin()
{
    reel_.setPosition({initialPos_.x, getCurrentY_(clock_.getElapsedTime())});
    if( clock_.getElapsedTime().asSeconds() > springingTime_ ) {
        setReelState(std::make_unique<ReelStopped>(reel_));
    }
}

float ReelStopping::getCurrentY_(const sf::Time& time) const
{
    const double pi = std::acos(-1);
    if(time.asSeconds() > springingTime_) {
        return initialPos_.y;
    }
    return initialPos_.y  + springingDistance_/2.0f + (springingDistance_/2.0f) * std::sin( time.asSeconds() * (2*pi/springingTime_) );
}



void ReelAligning::spin()
{
    float toAlign = (reel_.position().y + reel_.height()) - reel_.slots().back()->getPosition().y;
    float currentSpeed = reel_.speed();

    //finding minimum distance to align slots
    auto slotCount = reel_.slots().size();
    for(int i = slotCount - 1; i >= 0; --i) {
        auto& slot = reel_.slots()[i];
        float temp = reel_.position().y + reel_.height() - reel_.slotHeight() - reel_.spacing() - slot->getPosition().y;
        if(temp < 0) {
            continue;
        } else {
            toAlign = temp;
            break;
        }
    }

    sf::Time elapsed = clock_.restart();
    if(currentSpeed * elapsed.asMilliseconds() -  toAlign  > 0 ) {
        reel_.revolve(toAlign);
        setReelState(std::make_unique<ReelStopping>(reel_));
    } else {
        reel_.revolve(currentSpeed * elapsed.asMilliseconds());
    }
}

void ReelSpinning::spin()
{
    sf::Time elapsed = clock_.restart();
    reel_.revolve(reel_.speed() * elapsed.asMilliseconds());
}

ReelAccelerating::ReelAccelerating(Reel& reel) : ReelState(reel)
{
    acceleratingStep_ = reel_.speed() / acceleratingTime_;
}

float ReelAccelerating::getCurrentSpeed_() const
{
    float currentSpeed = acceleratingStep_ * sniceStateActivated_.getElapsedTime().asSeconds();
    return currentSpeed;
}

void ReelAccelerating::spin()
{
    sf::Time elapsed = clock_.restart();
    float currentSpeed = getCurrentSpeed_();
    if(currentSpeed < reel_.speed()) {
        reel_.revolve(currentSpeed * elapsed.asMilliseconds());
    } else {
        setReelState(std::make_unique<ReelSpinning>(reel_));
    }
}



Reel::Reel(int slotsCount, float slotWidth, float slotHeight, const sf::Vector2f& position) :
    slotsCount_(slotsCount),
    slotWidth_(slotWidth),
    slotHeight_(slotHeight),
    position_(position)
{
    auto defaultSlotGenerator = [](float slotWidth, float slotHeight){
        return std::make_unique<sf::RectangleShape>(sf::Vector2f{slotWidth, slotHeight});
    };

    slotGenerator_ = defaultSlotGenerator;

    height_ = slotsCount_ * (slotHeight_ + spacing_);
    state_ = std::make_unique<ReelStopped>(*this);

    sf::Vector2f currentPos = position_;
    for(int i = 0; i < slotsCount_; ++i) {
        slots_.emplace_back(slotGenerator_(slotWidth_, slotHeight_));
        slots_[i]->setPosition(position_.x, position_.y + (slotHeight_ + spacing_) * i);
    }

}

bool Reel::isStarted() const
{
    return dynamic_cast<ReelSpinning*>(state_.get());
}

bool Reel::isStopped() const
{
    return dynamic_cast<ReelStopped*>(state_.get());
}

void Reel::setSpacing(float spacing)
{
    spacing_ = spacing;
    for(int i = 0; i < slots_.size(); ++i) {
        slots_[i]->setPosition(position_.x, position_.y + (slotHeight_ + spacing_) * i);
    }
}

void Reel::setPosition(sf::Vector2f position)
{
    sf::Vector2f posDiff = position - position_;
    position_ = std::move(position);
    sf::Vector2f currentPos = position_;
    for(int i = 0; i < slotsCount_; ++i) {
        slots_[i]->setPosition(position_.x,  slots_[i]->getPosition().y + posDiff.y);
    }
}

void Reel::setSlotGenerator(const std::function<std::unique_ptr<sf::Shape>(float,float)>& generator)
{
    slotGenerator_ = generator;
    slots_.clear();
    sf::Vector2f currentPos = position_;
    for(int i = 0; i < slotsCount_; ++i) {
        slots_.emplace_back(slotGenerator_(slotWidth_, slotHeight_));
        slots_[i]->setPosition(position_.x, position_.y + (slotHeight_ + spacing_) * i);
    }
}

void Reel::draw(sf::RenderWindow& window)
{
    state_->spin();
    for(auto& slot : slots_) {
        window.draw(*slot);
    }
}

void Reel::requestStartSpinning()
{
    state_ = std::make_unique<ReelAccelerating>(*this);
}

void Reel::requestStopSpinning()
{
    state_ = std::make_unique<ReelAligning>(*this);
}

void Reel::revolve(float y)
{
    for(int i = 0; i < slots_.size(); ++i) {
        slots_[i]->move(0, y);
    }

    while (slots_.back()->getPosition().y > position_.y + height_ - slotHeight_) {
        slots_.pop_back();
        slots_.push_front(slotGenerator_(slotWidth_,slotHeight_));
        if(slots_.size() < 2){
            slots_.front()->setPosition(position_);
        } else {
            slots_[0]->setPosition(position_.x, slots_[1]->getPosition().y - slotHeight_ - spacing_);
        }
    }
}
