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
notification_alpha(1.0f),
window(sf::VideoMode(800, 600), "Triangles", sf::Style::Titlebar | sf::Style::Close),
trisIndex(0),
currentTri_state(CurrentState::NONE),
currentTri_maxState(CurrentState::NONE),
colorPickerColor{1.0f, 1.0f, 1.0f, 1.0f},
bgColorPickerColor{0.0f, 0.0f, 0.0f},
bgColor(sf::Color::Black),
inputWidthHeight{800, 600}
{
    flags.set(1); // is running
    ImGui::SFML::Init(window);
    window.setFramerateLimit(60);

    notification_text.fill(0);
    std::strcpy(notification_text.data(), "Press \"H\" for help");

    pointCircle.setRadius(7.0f);
    pointCircle.setOrigin(7.0f, 7.0f);
    pointCircle.setFillColor(sf::Color::White);
    pointCircle.setOutlineColor(sf::Color::Black);
    pointCircle.setOutlineThickness(1.0f);

    saveFilenameBuffer.fill(0);

    if(!drawCache.create(800, 600)) {
#ifndef NDEBUG
        puts("ERROR: Failed to initialize RenderTexture (draw cache)");
#endif
        flags.reset(8);
    } else {
        flags.set(8);
        flags.set(7);
        drawCacheSprite.setTexture(drawCache.getTexture(), true);
    }
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
                // TODO use a switch statement
                if(event.key.code == sf::Keyboard::H) {
                    flags.flip(0);
                } else if(event.key.code == sf::Keyboard::U) {
                    flags.set(7);
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
                    flags.set(7);
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
                    if(flags.test(2)) {
                        close_color_picker();
                    } else {
                        flags.set(2);
                    }
                } else if(event.key.code == sf::Keyboard::B) {
                    if(flags.test(5)) {
                        close_bg_color_picker();
                    } else {
                        flags.set(5);
                    }
                } else if(event.key.code == sf::Keyboard::S) {
                    flags.flip(6);
                } else if(event.key.code == sf::Keyboard::P) {
                    flags.flip(9);
                    if(flags.test(9)) {
                        notification_text.fill(0);
                        std::strcpy(notification_text.data(),
                            "Copy color mode\n"
                            "Click to change\n"
                            "current draw color\n"
                            "to what was\n"
                            "clicked on");
                        notification_alpha = 1.0f;
                    } else {
                        notification_alpha = 0.0f;
                    }
                } else if(event.key.code == sf::Keyboard::I) {
                    flags.flip(10);
                    if(!flags.test(10)) {
                        inputWidthHeight[0] = width;
                        inputWidthHeight[1] = height;
                    }
                }
            }
        } else if(event.type == sf::Event::MouseButtonPressed) {
            if(can_draw()) {
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
                    flags.set(7);
                    break;
                }
            } else if(flags.test(9)) {
                auto color = drawCache.getTexture().copyToImage().getPixel(event.mouseButton.x, event.mouseButton.y);
                colorPickerColor[0] = color.r / 255.0f;
                colorPickerColor[1] = color.g / 255.0f;
                colorPickerColor[2] = color.b / 255.0f;
                colorPickerColor[3] = 1.0f;
                pointCircle.setFillColor(color);
                flags.reset(9);
                notification_text.fill(0);
                std::strcpy(notification_text.data(),
                    "Color set");
                notification_alpha = 1.0f;
            }
        }
    }
}

void Tri::State::update() {
    ImGui::SFML::Update(window, dt);

    if(notification_alpha > 0.0f) {
        notification_alpha -= dt.asSeconds() * STARTING_HELP_FADE_RATE;
        if(notification_alpha < 0.0f) {
            notification_alpha = 0.0f;
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
    Tri::draw_notification(this);
    Tri::draw_color_picker(this);
    Tri::draw_bg_color_picker(this);
    Tri::draw_change_size(this);
    Tri::draw_save(this);
    Tri::draw_help(this);

    ImGui::EndFrame();
}

void Tri::State::draw() {
    if(flags.test(8)) {
        // draw cache initialized
        if(flags.test(7)) {
            // draw cache dirty
            flags.reset(7);
            draw_to_target(&drawCache);
            drawCache.display();
        }
        window.draw(drawCacheSprite);
    } else {
        draw_to_target(&window);
    }

    for(unsigned int i = 0; i < currentTri_state; ++i) {
        pointCircle.setPosition(currentTri[i]);
        window.draw(pointCircle);
    }

    // draw gui stuff
    ImGui::SFML::Render(window);

    window.display();
}

void Tri::State::draw_to_target(sf::RenderTarget *target) {
    target->clear(bgColor);

    // draw tris
    for(unsigned int i = 0; i < trisIndex; ++i) {
        target->draw(tris[i]);
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

float Tri::State::get_notification_alpha() const {
    return notification_alpha;
}

const char* Tri::State::get_notification_text() const {
    return notification_text.data();
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
        failedMessage = std::string("Failed to create texture for saving");
        return false;
    }

    draw_to_target(&saveTexture);
    saveTexture.display();

    sf::Image saveImage = saveTexture.getTexture().copyToImage();
    std::string filename = std::string(saveFilenameBuffer.data());
    if(saveImage.saveToFile(filename)) {
#ifndef NDEBUG
        printf("Saved to \"%s\"\n", filename.c_str());
#endif
        failedMessage.clear();
        return true;
    } else {
#ifndef NDEBUG
        printf("ERROR: Failed to save \"%s\"\n", filename.c_str());
#endif
        failedMessage = std::string("Failed to save (does the name end in \".png\"?)");
        return false;
    }
}

std::string_view Tri::State::failed_message() const {
    return failedMessage;
}

void Tri::State::close_save() {
    flags.reset(6);
}

bool Tri::State::can_draw() const {
    return !flags.test(0)
        && !flags.test(2)
        && !flags.test(5)
        && !flags.test(6)
        && !flags.test(9)
        && !flags.test(10);
}

void Tri::State::close_help() {
    flags.reset(0);
}

void Tri::State::close_color_picker() {
    flags.reset(2);
    flags.set(7);
}

void Tri::State::close_bg_color_picker() {
    flags.reset(5);
    flags.set(7);
}

bool Tri::State::change_width_height() {
    std::bitset<2> warnings;
    if(inputWidthHeight[0] < 0 || inputWidthHeight[1] < 0) {
        failedMessage = "Width or Height cannot be less than 0";
        return false;
    }
    if(inputWidthHeight[0] < 200) {
        inputWidthHeight[0] = 200;
        warnings.set(0);
    }
    if(inputWidthHeight[1] < 150) {
        inputWidthHeight[1] = 150;
        warnings.set(1);
    }

    if(warnings.test(0) && warnings.test(1)) {
        notification_alpha = 1.0f;
        notification_text.fill(0);
        std::strcpy(
            notification_text.data(),
            "Width set to 200\nHeight set to 150"
        );
    } else if(warnings.test(0)) {
        notification_alpha = 1.0f;
        notification_text.fill(0);
        std::strcpy(
            notification_text.data(),
            "Width set to 200"
        );
    } else if(warnings.test(1)) {
        notification_alpha = 1.0f;
        notification_text.fill(0);
        std::strcpy(
            notification_text.data(),
            "Height set to 150"
        );
    }

    this->width = inputWidthHeight[0];
    this->height = inputWidthHeight[1];

    window.setSize(sf::Vector2u(width, height));
    sf::View newView(
        sf::Vector2f(width / 2.0f, height / 2.0f),
        sf::Vector2f(width, height));
    window.setView(newView);

    drawCache.create(width, height);
    drawCacheSprite.setTexture(drawCache.getTexture(), true);
    flags.set(7);

    currentTri_state = CurrentState::NONE;

    return true;
}

int* Tri::State::get_input_width_height() {
    return inputWidthHeight;
}

void Tri::State::close_input_width_height_window() {
    flags.reset(10);
}
