#include "state.hpp"

#include <imgui-SFML.h>

#include "imgui_helper.hpp"

Tri::State::State() :
width(800),
height(600),
dt(sf::microseconds(16666)),
window(sf::VideoMode(800, 600), "Triangles", sf::Style::Titlebar | sf::Style::Close),
currentTri_state(CurrentState::NONE)
{
    ImGui::SFML::Init(window);
    window.setFramerateLimit(60);

    currentTri.setPointCount(3);
    currentTri.setFillColor(sf::Color::White);
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
        } else if(event.type == sf::Event::KeyPressed) {
            if(event.key.code == sf::Keyboard::H) {
                flags.flip(0);
            }
        } else if(event.type == sf::Event::MouseButtonPressed) {
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
}

void Tri::State::update() {
    ImGui::SFML::Update(window, dt);

    // Seems misleading, but imgui handles setting up the window during update
    Tri::draw_help(this);

    ImGui::EndFrame();
}

void Tri::State::draw() {
    window.clear();
    ImGui::SFML::Render(window);

    window.display();
}
