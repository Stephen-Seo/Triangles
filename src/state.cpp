#include "state.hpp"

#include "imgui_helper.hpp"

Tri::State::State() :
width(800),
height(600),
dt(sf::microseconds(16666))
{}

void Tri::State::update() {
    Tri::draw_help(this);
}

void Tri::State::draw() {
}
