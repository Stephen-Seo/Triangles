#include "state.hpp"

#include "imgui_helper.hpp"

Tri::State::State() :
width(800),
height(600),
dt(sf::microseconds(16666)),
currentTri_state(CurrentState::NONE)
{
    currentTri.setPointCount(3);
    currentTri.setFillColor(sf::Color::White);
}

void Tri::State::handle_event(sf::Event *event) {
    if(event->type == sf::Event::KeyPressed) {
        if(event->key.code == sf::Keyboard::H) {
            flags.set(0);
        }
    } else if(event->type == sf::Event::KeyReleased) {
        if(event->key.code == sf::Keyboard::H) {
            flags.reset(0);
        }
    } else if(event->type == sf::Event::MouseButtonPressed) {
        switch(currentTri_state) {
        case CurrentState::NONE:
            break;
        case CurrentState::FIRST:
            break;
        case CurrentState::SECOND:
            break;
        case CurrentState::THIRD:
            break;
        }
    }
}

void Tri::State::update() {
    // Seems misleading, but imgui handles setting up the window during update
    Tri::draw_help(this);
}

void Tri::State::draw() {
}
