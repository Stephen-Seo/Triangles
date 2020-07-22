#include "state.hpp"

#include <cstring>
#include <cassert>
#include <string>

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
currentTri_maxState(CurrentState::NONE),
colorPickerColor{1.0f, 1.0f, 1.0f, 1.0f},
bgColorPickerColor{0.0f, 0.0f, 0.0f},
bgColor(sf::Color::Black)
{
    flags.set(1); // is running
    ImGui::SFML::Init(window);
    window.setFramerateLimit(60);

    pointCircle.setRadius(7.0f);
    pointCircle.setOrigin(7.0f, 7.0f);
    pointCircle.setFillColor(sf::Color::White);
    pointCircle.setOutlineColor(sf::Color::Black);
    pointCircle.setOutlineThickness(1.0f);

    saveFilenameBuffer.fill(0);
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
            if(!flags.test(6)) {
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
                } else if(event.key.code == sf::Keyboard::C) {
                    flags.flip(2);
                } else if(event.key.code == sf::Keyboard::B) {
                    flags.flip(5);
                } else if(event.key.code == sf::Keyboard::S) {
                    flags.flip(6);
                }
            }
        } else if(event.type == sf::Event::MouseButtonPressed) {
            if(!flags.test(2) && !flags.test(5) && !flags.test(6)) {
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
                    tris.back().setFillColor(pointCircle.getFillColor());
                    currentTri_state = CurrentState::NONE;
                    currentTri_maxState = CurrentState::NONE;
                    break;
                }
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

    if(flags.test(3)) {
        flags.reset(3);
        pointCircle.setFillColor(sf::Color(
            (unsigned char)(255 * colorPickerColor[0]),
            (unsigned char)(255 * colorPickerColor[1]),
            (unsigned char)(255 * colorPickerColor[2]),
            (unsigned char)(255 * colorPickerColor[3])));
    }

    if(flags.test(4)) {
        flags.reset(4);
        bgColor.r = (unsigned char)(255 * bgColorPickerColor[0]);
        bgColor.g = (unsigned char)(255 * bgColorPickerColor[1]);
        bgColor.b = (unsigned char)(255 * bgColorPickerColor[2]);
    }

    // Seems misleading, but imgui handles setting up the window during update
    Tri::draw_show_help(this);
    Tri::draw_color_picker(this);
    Tri::draw_bg_color_picker(this);
    Tri::draw_save(this);
    Tri::draw_help(this);

    ImGui::EndFrame();
}

void Tri::State::draw() {
    draw_to_target(&window);

    // draw gui stuff
    ImGui::SFML::Render(window);

    window.display();
}

void Tri::State::draw_to_target(sf::RenderTarget *target, bool draw_points) {
    target->clear(bgColor);

    // draw tris
    for(unsigned int i = 0; i < trisIndex; ++i) {
        target->draw(tris[i]);
    }

    // draw points
    if(draw_points) {
        for(unsigned int i = 0; i < currentTri_state; ++i) {
            pointCircle.setPosition(currentTri[i]);
            target->draw(pointCircle);
        }
    }
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

float* Tri::State::get_color() {
    flags.set(3);
    return colorPickerColor;
}

float* Tri::State::get_bg_color() {
    flags.set(4);
    return bgColorPickerColor;
}

Tri::State::FilenameBufferType* Tri::State::get_save_filename_buffer() {
    return &saveFilenameBuffer;
}

bool Tri::State::do_save() {
    sf::RenderTexture saveTexture;
    if(!saveTexture.create(width, height)) {
#ifndef NDEBUG
        puts("ERROR: Failed to create texture for saving");
#endif
        failedSaveMessage = std::string("Failed to create texture for saving");
        return false;
    }

    draw_to_target(&saveTexture, false);
    saveTexture.display();

    sf::Image saveImage = saveTexture.getTexture().copyToImage();
    std::string filename = std::string(saveFilenameBuffer.data());
    if(saveImage.saveToFile(filename)) {
#ifndef NDEBUG
        printf("Saved to \"%s\"\n", filename.c_str());
#endif
        failedSaveMessage.clear();
        return true;
    } else {
#ifndef NDEBUG
        printf("ERROR: Failed to save \"%s\"\n", filename.c_str());
#endif
        failedSaveMessage = std::string("Failed to save (does the name end in \".png\"?)");
        return false;
    }
}

std::string_view Tri::State::failed_save_message() const {
    return failedSaveMessage;
}

void Tri::State::close_save() {
    flags.reset(6);
}
