#include "slot_mashine_game.h"
#include <iostream>
#include <random>
#include <string>


SlotMashineState::SlotMashineState(SlotMashineGame& game) : game_(game)
{

}

void SlotMashineState::setGameState_(std::unique_ptr<SlotMashineState> state)
{
    game_.state_ = std::move(state);
}

Button& SlotMashineState::startButton_()
{
    return game_.startButton_;
}


Button& SlotMashineState::stopButton_()
{
    return game_.stopButton_;
}

SlotMashineDisplay& SlotMashineState::display_()
{
    return game_.display_;
}

sf::RenderWindow& SlotMashineState::window_()
{
    return game_.window_;
}

sf::View& SlotMashineState::defaultView_()
{
    return game_.defaultView_;
}


WaitingForStart::WaitingForStart(SlotMashineGame& game) : SlotMashineState(game)
{

}
void WaitingForStart::draw()
{
    window_().clear();
    window_().setView(defaultView_());
    startButton_().draw(window_());
    display_().draw(window_());
    window_().display();
}

void WaitingForStart::processEvents()
{
    for (auto event = sf::Event(); window_().pollEvent(event);) {
        if (event.type == sf::Event::Closed) {
            window_().close();
        } else if (event.type == sf::Event::Resized) {
            display_().updateSize(event.size.width, event.size.height);
            
            sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
            defaultView_() = sf::View(visibleArea);

        } else if (event.type == sf::Event::MouseButtonPressed) {
            bool startButtonPressed = startButton_().isPressed(event.mouseButton.x, event.mouseButton.y);
            if (event.mouseButton.button == sf::Mouse::Left && startButtonPressed) {
                setGameState_(std::make_unique<AcceleratingReels>(game_));
                return;
            }
        }
    }
}

DisplayingPrize::DisplayingPrize(SlotMashineGame& game) : SlotMashineState(game)
{
    auto windowSize = window_().getSize();
    sf::Vector2f displayCenter {display_().position().x + display_().width()/2.0f, display_().position().y + display_().height()/2.0f};
    messageBox.setPosition({displayCenter.x - messageBox.size().x/2.0f, displayCenter.y - messageBox.size().y/2.0f});
    messageBox.setFillColor({20,20,20,255});
}
void DisplayingPrize::draw()
{
    window_().clear();
    window_().setView(defaultView_());
    display_().draw(window_());
    window_().setView(defaultView_());
    messageBox.draw(window_());
    startButton_().draw(window_());
    window_().display();
}

void DisplayingPrize::processEvents()
{
    messageBox.setMessageText(L"Выигрыш: " + std::to_wstring(calculatePrize_())); 

    for (auto event = sf::Event(); window_().pollEvent(event);) {
        if (event.type == sf::Event::Closed) {
            window_().close();
        } else if (event.type == sf::Event::Resized) {
            display_().updateSize(event.size.width, event.size.height);
            
            sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
            defaultView_() = sf::View(visibleArea);
        } else if (event.type == sf::Event::MouseButtonPressed) {
            bool startButtonPressed = startButton_().isPressed(event.mouseButton.x, event.mouseButton.y);
            if (event.mouseButton.button == sf::Mouse::Left && startButtonPressed) {
                setGameState_(std::make_unique<AcceleratingReels>(game_));
                return;
            }
            setGameState_(std::make_unique<WaitingForStart>(game_));
            return;
        }

    }
}

int DisplayingPrize::calculatePrize_()
{
    int prize = 100;
    auto slots = display_().displayedSlotRows();
    for(auto& row : slots) {
        int maxSequence = 1;
        int currentSequence = 1;
        for(int i = 1; i < row.size(); ++i) {
            if(row[i].getSlotType() == row[i-1].getSlotType()) {
                ++currentSequence;
                if( currentSequence > maxSequence ) {
                    maxSequence = currentSequence;
                }
            } else {
                currentSequence = 1;
            }
        }
        prize *= maxSequence;
    }
    return prize;

}

StoppingReels::StoppingReels(SlotMashineGame& game) : SlotMashineState(game)
{
    display_().requestStop();
}
void StoppingReels::draw()
{
    window_().clear();
    window_().setView(defaultView_());
    display_().draw(window_());
    window_().display();
}
void StoppingReels::processEvents()
{
    if(display_().isStopped()) {
        setGameState_(std::make_unique<DisplayingPrize>(game_));
        return;
    }

    for (auto event = sf::Event(); window_().pollEvent(event);) {
        if (event.type == sf::Event::Closed) {
            window_().close();
        } else if (event.type == sf::Event::Resized) {
            display_().updateSize(event.size.width, event.size.height);
            
            sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
            defaultView_() = sf::View(visibleArea);
        }
    }
}


SpinningReels::SpinningReels(SlotMashineGame& game) : SlotMashineState(game)
{

}
void SpinningReels::draw()
{
    window_().clear();
    window_().setView(defaultView_());
    stopButton_().draw(window_());
    display_().draw(window_());
    window_().display();

}
void SpinningReels::processEvents()
{
    
    for (auto event = sf::Event(); window_().pollEvent(event);) {
        if (event.type == sf::Event::Closed) {
            window_().close();
        } else if (event.type == sf::Event::Resized) {
            display_().updateSize(event.size.width, event.size.height);
            
            sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
            defaultView_() = sf::View(visibleArea);

        } else if (event.type == sf::Event::MouseButtonPressed) {
            bool stopButtonPressed = stopButton_().isPressed(event.mouseButton.x, event.mouseButton.y);
            if (event.mouseButton.button == sf::Mouse::Left && stopButtonPressed) {
                setGameState_(std::make_unique<StoppingReels>(game_));
                return;
            }
        }
    }
    
    if(clock_.getElapsedTime().asSeconds() > spinTimeout_) {
        display_().requestStop();
        setGameState_(std::make_unique<StoppingReels>(game_));
    }

}


AcceleratingReels::AcceleratingReels(SlotMashineGame& game) : SlotMashineState(game)
{
    display_().requestStart();
}

void AcceleratingReels::draw()
{
    if(display_().isStarted()) {
        setGameState_(std::make_unique<SpinningReels>(game_));
        return;
    }
    window_().clear();
    window_().setView(defaultView_());
    display_().draw(window_());
    window_().display();
}

void AcceleratingReels::processEvents()
{
    for (auto event = sf::Event(); window_().pollEvent(event);) {
        if (event.type == sf::Event::Closed) {
            window_().close();
        } else if (event.type == sf::Event::Resized) {
            display_().updateSize(event.size.width, event.size.height);
            
            sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
            defaultView_() = sf::View(visibleArea);
        }
    }
}



SlotMashineGame::SlotMashineGame() : 
    window_({1280u,800u}, "slots"),
    display_(900,600, 4)
{
    window_.setFramerateLimit(144);
    
    display_.setBackgroundColor({20, 20, 20});
    display_.setPosition({50, window_.getSize().y/2.0f - display_.height()/2}, window_.getSize().x, window_.getSize().y);

    startButton_.setText(L"START");
    startButton_.setFontSize(42);
    startButton_.setColor({255,20,20,255});
    startButton_.setFontColor({0,0,0,255});
    startButton_.setSize({250,100});
    startButton_.setPosition({display_.position().x + display_.width() + 50, display_.position().y});
    
    stopButton_.setText(L"STOP");
    stopButton_.setFontSize(42);
    stopButton_.setColor({255,20,20,255});
    stopButton_.setFontColor({0,0,0,255});
    stopButton_.setSize({250,100});
    stopButton_.setPosition({display_.position().x + display_.width() + 50, display_.position().y});

    auto slotGenerator = [](float slotWidth, float slotHeight) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double> dis(0.0, 1.0);
        double rand_num = dis(gen);
        std::unique_ptr<sf::Shape> slot;

        static bool loaded = false;
        static sf::Texture ruby;
        static sf::Texture diamond;
        static sf::Texture ametist;

        bool loadRes = true;

        if(!loaded) {
            loadRes = loadRes && ruby.loadFromFile("data/gems.png", {85,275,175, 175});
            loadRes = loadRes && ametist.loadFromFile("data/gems.png", {308,277,175, 170});
            loadRes = loadRes && diamond.loadFromFile("data/gems.png", {75,480,174, 170});
            
            if (!loadRes)
            {
                std::runtime_error("Cant load texture.");
            }

            diamond.setSmooth(true);
            ruby.setSmooth(true);
            ametist.setSmooth(true);
            loaded = true;
        }
        if (rand_num < 0.33) {
            slot = std::make_unique<Slot>(slotWidth, slotHeight, static_cast<int>(SlotType::Ruby));
            slot->setTexture(&ruby);
        } else if (rand_num < 0.66) {
            slot = std::make_unique<Slot>(slotWidth, slotHeight, static_cast<int>(SlotType::Diamond));
            slot->setTexture(&diamond);
        } else {
            slot = std::make_unique<Slot>(slotWidth, slotHeight, static_cast<int>(SlotType::Ametist));
            slot->setTexture(&ametist);
        }
        return slot;
    };

    display_.setSlotGenerator(slotGenerator);

    state_ = std::make_unique<WaitingForStart>(*this);

}


void SlotMashineGame::runGame()
{
    while (window_.isOpen()) {
        state_->processEvents();
        state_->draw(); 
    }

}


void SlotMashineGame::draw_()
{
    window_.clear();
    window_.setView(defaultView_);
    startButton_.draw(window_);
    stopButton_.draw(window_);
    display_.draw(window_);
    window_.display();
}

void SlotMashineGame::startSpinning_()
{
    state_ = std::make_unique<AcceleratingReels>(*this);
}

void SlotMashineGame::stopSpinning_()
{
    state_ = std::make_unique<StoppingReels>(*this);
}
