#pragma once

#include <functional>
#include <memory>
#include <deque>

#include <SFML/Graphics.hpp>

class ReelState;

class Reel
{
  public:
    Reel(int slotsCount, float slotWidth, float slotHeight, const sf::Vector2f& posotion = {0.0f,0.0f});
    
    void setPosition(sf::Vector2f position);
    sf::Vector2f position() const { return position_; }
    const std::deque<std::unique_ptr<sf::Shape>>& slots() const { return slots_; }

    void setSlotGenerator(const std::function<std::unique_ptr<sf::Shape>(float,float)>& generator);

    void setSpeed(float speed) { speed_ = speed; }
    float speed() const { return speed_; }

    bool isStarted() const;
    bool isStopped() const;

    void setSpacing(float spacing);
    float spacing() const { return spacing_; }

    float height() const { return height_; }
    float slotWidth() const { return slotWidth_; }
    float slotHeight() const { return slotHeight_; }


    void draw(sf::RenderWindow& window);
    void revolve(float y);

    void requestStartSpinning();
    void requestStopSpinning();

    friend class ReelState;

  private: 
    void setState(std::unique_ptr<ReelState> state) { state_ = std::move(state); }
    std::function<std::unique_ptr<sf::Shape>(float, float)> slotGenerator_; 

    std::unique_ptr<ReelState> state_;
    std::deque<std::unique_ptr<sf::Shape>> slots_;
    float speed_ = 4;
    float slotWidth_;
    float slotHeight_;
    float spacing_ = 30;
    int slotsCount_;
    float height_;

    sf::Vector2f position_ {0.0f, 0.0f};

};


//Reel states
class ReelState
{
  public:
    ReelState(Reel& reel);
    virtual void spin() = 0;
    virtual ~ReelState() = default;

  protected:
    void setReelState(std::unique_ptr<ReelState> state);
    Reel& reel_;
};

class ReelStopped : public ReelState
{
  public:
    ReelStopped(Reel& reel) : ReelState(reel) {}
    virtual void spin() override;
};

class ReelStopping : public ReelState
{
  public:
    ReelStopping(Reel& reel);
    virtual void spin() override;
  private:
    float getCurrentY_(const sf::Time& time) const;

    sf::Vector2f initialPos_;
    sf::Clock clock_;
    float springingTime_ = 0.2;
    float springingDistance_ = 20;
};

class ReelAligning : public ReelState
{
  public:
    ReelAligning(Reel& reel) : ReelState(reel) {}
    virtual void spin() override;
  private:
    sf::Clock clock_;
};

class ReelSpinning : public ReelState
{
  public:
    ReelSpinning(Reel& reel) : ReelState(reel) {}
    virtual void spin() override;
  private:
    sf::Clock clock_;
};

class ReelAccelerating : public ReelState
{
  public:
    ReelAccelerating(Reel& reel);
    virtual void spin() override;
  private:
    float getCurrentSpeed_() const;
    float acceleratingTime_ = 1;
    float acceleratingStep_;
    sf::Clock clock_;
    sf::Clock sniceStateActivated_;
};
