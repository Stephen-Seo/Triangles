#include "state.hpp"

#include <cstring>
#include <cassert>

#include <imgui-SFML.h>

#include "imgui_helper.hpp"

#define STARTING_HELP_FADE_RATE 0.2f

#ifndef NDEBUG
# include <cstdio>
#endif

Tri::State::State(int argc, char **argv) :
width(800),
height(600),
dt(sf::microseconds(16666)),
starting_help_alpha(1.0f),
window(sf::VideoMode(800, 600), "Triangles", sf::Style::Titlebar | sf::Style::Close),
trisIndex(0),
currentTri_state(CurrentState::NONE),
currentTri_maxState(CurrentState::NONE)
{
    flags.set(1); // is running
    ImGui::SFML::Init(window);
    window.setFramerateLimit(60);

    pointCircle.setRadius(7.0f);
    pointCircle.setOrigin(7.0f, 7.0f);
    pointCircle.setFillColor(sf::Color::White);
    pointCircle.setOutlineColor(sf::Color::Black);
    pointCircle.setOutlineThickness(1.0f);
}

Tri::State::~State() {
    window.close();
    ImGui::SFML::Shutdown();
}

void Tri::State::handle_events() {
    while(window.pollEvent(event)) {
        ImGui::SFML::ProcessEvent(event);
        if(event.type == sf::Event::Closed) {
            window.close();
            flags.reset(1);
        } else if(event.type == sf::Event::KeyPressed) {
            if(event.key.code == sf::Keyboard::H) {
                flags.flip(0);
            } else if(event.key.code == sf::Keyboard::U) {
                if(currentTri_state > 0) {
                    switch(currentTri_state) {
                    case FIRST:
                        currentTri_state = CurrentState::NONE;
                        break;
                    case SECOND:
                        currentTri_state = CurrentState::FIRST;
                        break;
                    default:
                        assert(!"Unreachable code");
                        break;
                    }
                } else if(trisIndex > 0) {
                    --trisIndex;
                }
            } else if(event.key.code == sf::Keyboard::R) {
                if(currentTri_state != CurrentState::NONE
                        && currentTri_state < currentTri_maxState) {
                    switch(currentTri_state) {
                    case NONE:
                        currentTri_state = CurrentState::FIRST;
                        break;
                    case FIRST:
                        currentTri_state = CurrentState::SECOND;
                        break;
                    default:
                        assert(!"Unreachable code");
                        break;
                    }
                } else if(tris.size() > trisIndex) {
                    ++trisIndex;
                } else if(currentTri_state < currentTri_maxState) {
                    switch(currentTri_state) {
                    case NONE:
                        currentTri_state = CurrentState::FIRST;
                        break;
                    case FIRST:
                        currentTri_state = CurrentState::SECOND;
                        break;
                    default:
                        assert(!"Unreachable code");
                        break;
                    }
                }
            }
        } else if(event.type == sf::Event::MouseButtonPressed) {
            switch(currentTri_state) {
            case CurrentState::NONE:
                currentTri[0] = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);
                if(trisIndex < tris.size()) {
                    tris.resize(trisIndex);
                }
                currentTri_state = CurrentState::FIRST;
                currentTri_maxState = CurrentState::FIRST;
                break;
            case CurrentState::FIRST:
                currentTri[1] = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);
                if(trisIndex < tris.size()) {
                    tris.resize(trisIndex);
                }
                currentTri_state = CurrentState::SECOND;
                currentTri_maxState = CurrentState::SECOND;
                break;
            case CurrentState::SECOND:
                currentTri[2] = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);
                if(trisIndex < tris.size()) {
                    tris.resize(trisIndex);
                }
                ++trisIndex;
                tris.emplace_back(sf::ConvexShape(3));
                tris.back().setPoint(0, currentTri[0]);
                tris.back().setPoint(1, currentTri[1]);
                tris.back().setPoint(2, currentTri[2]);
                tris.back().setFillColor(sf::Color::White); // TODO use chosen color
                currentTri_state = CurrentState::NONE;
                currentTri_maxState = CurrentState::NONE;
                break;
            }
        }
    }
}

void Tri::State::update() {
    ImGui::SFML::Update(window, dt);

    if(starting_help_alpha > 0.0f) {
        starting_help_alpha -= dt.asSeconds() * STARTING_HELP_FADE_RATE;
        if(starting_help_alpha < 0.0f) {
            starting_help_alpha = 0.0f;
        }
    }

    // Seems misleading, but imgui handles setting up the window during update
    Tri::draw_show_help(this);
    Tri::draw_help(this);

    ImGui::EndFrame();
}

void Tri::State::draw() {
    window.clear();

    // draw tris
    for(unsigned int i = 0; i < trisIndex; ++i) {
        window.draw(tris[i]);
    }

    // draw points
    for(unsigned int i = 0; i < currentTri_state; ++i) {
        pointCircle.setPosition(currentTri[i]);
        window.draw(pointCircle);
    }

    // draw gui stuff
    ImGui::SFML::Render(window);

    window.display();
}

unsigned int Tri::State::get_width() const {
    return width;
}

unsigned int Tri::State::get_height() const {
    return height;
}

const Tri::State::BitsetType Tri::State::get_flags() const {
    return flags;
}

float Tri::State::get_starting_help_alpha() const {
    return starting_help_alpha;
}
