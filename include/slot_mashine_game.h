#pragma once

#include "button.h"
#include "slot_mashine_display.h"
#include "message_box.h"

#include <memory>

#include <SFML/Graphics.hpp>


class SlotMashineState;

class SlotMashineGame
{
  public:
    SlotMashineGame();
    void runGame();

    enum class SlotType
    {
        Ruby = 1,
        Diamond,
        Ametist
    };

  private:
    void draw_();
    void startSpinning_();
    void stopSpinning_();
    int calculatePrize_();


    sf::RenderWindow window_;
    SlotMashineDisplay display_;
    Button startButton_;
    Button stopButton_;
    sf::View defaultView_;

    std::unique_ptr<SlotMashineState> state_;


    friend class SlotMashineState;
};


// Slot mashine states
class SlotMashineState
{
  public:
    SlotMashineState(SlotMashineGame& game);
    virtual void draw() = 0;
    virtual void processEvents() = 0;

  protected:
    void setGameState_(std::unique_ptr<SlotMashineState> state);
    Button& startButton_();
    Button& stopButton_();
    SlotMashineDisplay& display_();
    sf::RenderWindow& window_();
    sf::View& defaultView_();

    SlotMashineGame& game_;
};

class WaitingForStart : public SlotMashineState
{
  public:
    WaitingForStart(SlotMashineGame& game);
    void draw() override;
    void processEvents() override;
};

class DisplayingPrize : public SlotMashineState
{
  public:
    DisplayingPrize(SlotMashineGame& game);
    void draw() override;
    void processEvents() override;

  private:
    MessageBox messageBox;
    int calculatePrize_();
};

class StoppingReels : public SlotMashineState
{
  public:
    StoppingReels(SlotMashineGame& game);
    void draw() override;
    void processEvents() override;
};

class SpinningReels : public SlotMashineState
{
  public:
    SpinningReels(SlotMashineGame& game);
    void draw() override;
    void processEvents() override;
  private:
    sf::Clock clock_;
    float spinTimeout_ = 5;
};

class AcceleratingReels : public SlotMashineState
{
  public:
    AcceleratingReels(SlotMashineGame& game);
    void draw() override;
    void processEvents() override;
};
