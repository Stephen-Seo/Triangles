#include "state.hpp"

#include <cstring>
#include <cassert>
#include <string>
#include <cmath>

#include <imgui-SFML.h>

#include "helpers.hpp"

#define STARTING_HELP_FADE_RATE 0.2f

#ifndef NDEBUG
# include <cstdio>
#endif

Tri::State::State(int /*argc*/, char ** /*argv*/) :
width(800),
height(600),
dt(sf::microseconds(16666)),
notification_alpha(1.0f),
window(sf::VideoMode(800, 600), "Triangles", sf::Style::Titlebar | sf::Style::Close),
currentTri_state(CurrentState::NONE),
colorPickerColor{1.0f, 1.0f, 1.0f, 1.0f},
bgColorPickerColor{0.0f, 0.0f, 0.0f},
bgColor(sf::Color::Black),
inputWidthHeight{800, 600},
pi(std::acos(-1.0f)),
history_idx(0)
{
    flags.set(F_IS_RUNNING); // is running
    ImGui::SFML::Init(window);
    window.setFramerateLimit(60);

    set_notification_text("Press \"H\" for help");

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
        flags.reset(F_DRAW_CACHE_INITIALIZED);
    } else {
        flags.set(F_DRAW_CACHE_INITIALIZED);
        flags.set(F_DRAW_CACHE_DIRTY);
        drawCacheSprite.setTexture(drawCache.getTexture(), true);
    }
}

Tri::State::Action::Action() :
type(Tri::State::Action::AT_NONE),
idx(0),
color(sf::Color::Black),
data{{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}}
{}

Tri::State::Action::Action(const Type& type, IndexT idx, const sf::Color& color, float *data) :
type(type),
idx(idx),
color(color)
{
    init(data);
}

Tri::State::Action::Action(Type&& type, IndexT idx, sf::Color&& color, float *data) :
type(type),
idx(idx),
color(color)
{
    init(data);
}

void Tri::State::Action::init(float *data) {
    switch(type) {
    case AT_TRI:
    case AT_TRI_DEL:
        for(unsigned int i = 0; i < 6; ++i) {
            this->data.tri[i] = data[i];
        }
        break;
    case AT_POINT:
        this->data.point[0] = data[0];
        this->data.point[1] = data[1];
        break;
    default:
        type = AT_NONE;
        idx = 0;
        color = sf::Color::Black;
        this->data = {{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}};
        break;
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
            flags.reset(F_IS_RUNNING);
        } else if(event.type == sf::Event::KeyPressed) {
            if(!flags.test(F_DISPLAY_SAVE)) {
                // TODO use a switch statement
                if(event.key.code == sf::Keyboard::H) {
                    flags.flip(F_DISPLAY_HELP);
                } else if(event.key.code == sf::Keyboard::U) {
                    flags.set(F_DRAW_CACHE_DIRTY);
                    if(history_idx > 0) {
                        switch(history[history_idx-1].type) {
                        case Action::AT_TRI:
                            tris.erase(
                                tris.cbegin() + history[history_idx-1].idx);
                            restore_points_on_tri_del(history_idx - 1);
                            break;
                        case Action::AT_TRI_DEL:
                        {
                            tris.emplace(
                                tris.cbegin() + history[history_idx-1].idx,
                                sf::ConvexShape(3));
                            tris[history[history_idx-1].idx].setPoint(0, sf::Vector2f(
                                history[history_idx-1].data.tri[0],
                                history[history_idx-1].data.tri[1]));
                            tris[history[history_idx-1].idx].setPoint(1, sf::Vector2f(
                                history[history_idx-1].data.tri[2],
                                history[history_idx-1].data.tri[3]));
                            tris[history[history_idx-1].idx].setPoint(2, sf::Vector2f(
                                history[history_idx-1].data.tri[4],
                                history[history_idx-1].data.tri[5]));
                            tris[history[history_idx-1].idx].setFillColor(history[history_idx-1].color);
                            currentTri_state = CurrentState::NONE;
                            break;
                        }
                        case Action::AT_POINT:
                            assert(history[history_idx-1].idx + 1 == currentTri_state
                                && "Point in history must match point index");
                            assert(currentTri_state > 0
                                && "There must be a point to undo a point");
                            currentTri_state = static_cast<CurrentState>(currentTri_state - 1);
                            break;
                        default:
                            assert(!"Unreachable code");
                            break;
                        }
                        --history_idx;
                    }
                } else if(event.key.code == sf::Keyboard::R) {
                    flags.set(F_DRAW_CACHE_DIRTY);
                    if(history_idx < history.size()) {
                        switch(history[history_idx].type) {
                        case Action::AT_TRI:
                        {
                            tris.emplace(
                                tris.cbegin() + history[history_idx].idx,
                                sf::ConvexShape(3));
                            tris[history[history_idx].idx].setPoint(0, sf::Vector2f(
                                history[history_idx].data.tri[0],
                                history[history_idx].data.tri[1]));
                            tris[history[history_idx].idx].setPoint(1, sf::Vector2f(
                                history[history_idx].data.tri[2],
                                history[history_idx].data.tri[3]));
                            tris[history[history_idx].idx].setPoint(2, sf::Vector2f(
                                history[history_idx].data.tri[4],
                                history[history_idx].data.tri[5]));
                            tris[history[history_idx].idx].setFillColor(history[history_idx].color);
                            currentTri_state = CurrentState::NONE;
                            break;
                        }
                            break;
                        case Action::AT_TRI_DEL:
                            tris.erase(
                                tris.cbegin() + history[history_idx].idx);
                            restore_points_on_tri_del(history_idx);
                            break;
                        case Action::AT_POINT:
                            assert(history[history_idx].idx == currentTri_state
                                && "Point in history must match point index");
                            assert(currentTri_state < CurrentState::SECOND
                                && "Current point state must be 0 or 1");
                            currentTri[currentTri_state].x = history[history_idx].data.point[0];
                            currentTri[currentTri_state].y = history[history_idx].data.point[1];
                            currentTri_state = static_cast<CurrentState>(
                                currentTri_state + 1);
                            pointCircle.setFillColor(history[history_idx].color);
                            break;
                        default:
                            assert(!"Unreachable code");
                            break;
                        }
                        ++history_idx;
                    }
                } else if(event.key.code == sf::Keyboard::C) {
                    if(flags.test(F_DISPLAY_COLOR_P)) {
                        close_color_picker();
                    } else {
                        flags.set(F_DISPLAY_COLOR_P);
                    }
                } else if(event.key.code == sf::Keyboard::B) {
                    if(flags.test(F_DISPLAY_BG_COLOR_P)) {
                        close_bg_color_picker();
                    } else {
                        flags.set(F_DISPLAY_BG_COLOR_P);
                    }
                } else if(event.key.code == sf::Keyboard::S) {
                    flags.flip(F_DISPLAY_SAVE);
                } else if(event.key.code == sf::Keyboard::P) {
                    flags.flip(F_COPY_COLOR_MODE);
                    if(flags.test(F_COPY_COLOR_MODE)) {
                        set_notification_text(
                            "Copy color mode\n"
                            "Click to change\n"
                            "current draw color\n"
                            "to what was\n"
                            "clicked on");
                    } else {
                        notification_alpha = 0.0f;
                    }
                } else if(event.key.code == sf::Keyboard::I) {
                    flags.flip(F_DISPLAY_CHANGE_SIZE);
                    if(!flags.test(F_DISPLAY_CHANGE_SIZE)) {
                        inputWidthHeight[0] = width;
                        inputWidthHeight[1] = height;
                    }
                } else if(event.key.code == sf::Keyboard::E) {
                    if(flags.test(F_TRI_EDIT_MODE)) {
                        close_selected_tri_mode();
                    } else {
                        flags.flip(F_SELECT_TRI_MODE);
                        if(flags.test(F_SELECT_TRI_MODE)) {
                            set_notification_text("Click on a tri\nto edit it");
                        }
                    }
                }
            }
        } else if(event.type == sf::Event::MouseButtonPressed) {
            if(can_draw()) {
                switch(currentTri_state) {
                case CurrentState::NONE:
                {
                    currentTri[0] = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);
                    currentTri_state = CurrentState::FIRST;

                    if(history_idx < history.size()) {
                        history.resize(history_idx);
                    }
                    float points[2] = {currentTri[0].x, currentTri[0].y};
                    history.push_back(Action(Action::AT_POINT,
                                             0,
                                             pointCircle.getFillColor(),
                                             points));
                    ++history_idx;
                    break;
                }
                case CurrentState::FIRST:
                {
                    currentTri[1] = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);
                    currentTri_state = CurrentState::SECOND;

                    if(history_idx < history.size()) {
                        history.resize(history_idx);
                    }
                    float points[2] = {currentTri[1].x, currentTri[1].y};
                    history.push_back(Action(Action::AT_POINT,
                                             1,
                                             pointCircle.getFillColor(),
                                             points));
                    ++history_idx;
                    break;
                }
                case CurrentState::SECOND:
                {
                    currentTri[2] = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);
                    tris.emplace_back(sf::ConvexShape(3));
                    tris.back().setPoint(0, currentTri[0]);
                    tris.back().setPoint(1, currentTri[1]);
                    tris.back().setPoint(2, currentTri[2]);
                    tris.back().setFillColor(pointCircle.getFillColor());
                    currentTri_state = CurrentState::NONE;
                    flags.set(F_DRAW_CACHE_DIRTY);

                    if(history_idx < history.size()) {
                        history.resize(history_idx);
                    }
                    float points[6] = {
                        currentTri[0].x, currentTri[0].y,
                        currentTri[1].x, currentTri[1].y,
                        currentTri[2].x, currentTri[2].y,
                    };
                    history.push_back(Action(Action::AT_TRI,
                                             tris.size()-1,
                                             pointCircle.getFillColor(),
                                             points));
                    ++history_idx;
                    break;
                }
                }
            } else if(flags.test(F_COPY_COLOR_MODE)) {
                auto color = drawCache.getTexture().copyToImage()
                    .getPixel(event.mouseButton.x, event.mouseButton.y);
                colorPickerColor[0] = color.r / 255.0f;
                colorPickerColor[1] = color.g / 255.0f;
                colorPickerColor[2] = color.b / 255.0f;
                colorPickerColor[3] = 1.0f;
                pointCircle.setFillColor(color);
                flags.reset(F_COPY_COLOR_MODE);
                set_notification_text("Color set");
            } else if(flags.test(F_SELECT_TRI_MODE)) {
                sf::Vector2f mouseXY = window.mapPixelToCoords(
                    {event.mouseButton.x, event.mouseButton.y});
                for(unsigned int i = tris.size(); i-- > 0; ) {
                    if(is_within_shape(tris.at(i), mouseXY)) {
                        selectedTri = i;
                        tris[i].setOutlineColor(invert_color(tris[i].getFillColor()));
                        flags.reset(F_SELECT_TRI_MODE);
                        flags.set(F_TRI_EDIT_MODE);
                        flags.set(F_TRI_EDIT_DRAW_TRI);
                        selectedTriBlinkTimer = 1.0f;
                        selectedTriColor[0] = tris[i].getFillColor().r / 255.0f;
                        selectedTriColor[1] = tris[i].getFillColor().g / 255.0f;
                        selectedTriColor[2] = tris[i].getFillColor().b / 255.0f;
                        selectedTriColor[3] = tris[i].getFillColor().a / 255.0f;
                        break;
                    }
                }
                if(!flags.test(F_TRI_EDIT_MODE)) {
                    set_notification_text("Did not select\nanything");
                }
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

    if(flags.test(F_COLOR_P_COLOR_DIRTY)) {
        flags.reset(F_COLOR_P_COLOR_DIRTY);
        pointCircle.setFillColor(sf::Color(
            (unsigned char)(255 * colorPickerColor[0]),
            (unsigned char)(255 * colorPickerColor[1]),
            (unsigned char)(255 * colorPickerColor[2]),
            (unsigned char)(255 * colorPickerColor[3])));
    }

    if(flags.test(F_BG_COLOR_P_COLOR_DIRTY)) {
        flags.reset(F_BG_COLOR_P_COLOR_DIRTY);
        bgColor.r = (unsigned char)(255 * bgColorPickerColor[0]);
        bgColor.g = (unsigned char)(255 * bgColorPickerColor[1]);
        bgColor.b = (unsigned char)(255 * bgColorPickerColor[2]);
    }

    if(flags.test(F_TRI_EDIT_MODE)) {
        selectedTriBlinkTimer -= dt.asSeconds() * TRIANGLES_EDIT_TRI_BLINK_RATE;
        if(selectedTriBlinkTimer <= 0.0f) {
            selectedTriBlinkTimer = 1.0f;
            flags.flip(F_TRI_EDIT_DRAW_TRI);
        }
    }

    // Seems misleading, but imgui handles setting up the window during update
    Tri::draw_notification(this);
    Tri::draw_color_picker(this);
    Tri::draw_bg_color_picker(this);
    Tri::draw_edit_tri(this);
    Tri::draw_change_size(this);
    Tri::draw_save(this);
    Tri::draw_help(this);

    ImGui::EndFrame();
}

void Tri::State::draw() {
    if(flags.test(F_DRAW_CACHE_INITIALIZED)) {
        // draw cache initialized
        if(flags.test(F_DRAW_CACHE_DIRTY)) {
            // draw cache dirty
            flags.reset(F_DRAW_CACHE_DIRTY);
            draw_to_target(&drawCache);
            drawCache.display();
        }
        window.draw(drawCacheSprite);
    } else {
        draw_to_target(&window);
    }

    if(flags.test(F_TRI_EDIT_MODE) && flags.test(F_TRI_EDIT_DRAW_TRI)) {
        tris.at(selectedTri).setOutlineThickness(4.0f);
        window.draw(tris[selectedTri]);
        tris.at(selectedTri).setOutlineThickness(0.0f);
    }

    if(can_draw()) {
        for(unsigned int i = 0; i < currentTri_state; ++i) {
            pointCircle.setPosition(currentTri[i]);
            window.draw(pointCircle);
        }
    }

    // draw gui stuff
    ImGui::SFML::Render(window);

    window.display();
}

void Tri::State::draw_to_target(sf::RenderTarget *target) {
    target->clear(bgColor);

    // draw tris
    for(unsigned int i = 0; i < tris.size(); ++i) {
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

void Tri::State::set_notification_text(const char *text) {
    notification_text.fill(0);
    std::strncpy(notification_text.data(),
        text,
        notification_text.max_size() - 1);
    notification_alpha = 1.0f;
}

float* Tri::State::get_color() {
    flags.set(F_COLOR_P_COLOR_DIRTY);
    return colorPickerColor;
}

float* Tri::State::get_bg_color() {
    flags.set(F_BG_COLOR_P_COLOR_DIRTY);
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
    flags.reset(F_DISPLAY_SAVE);
}

bool Tri::State::can_draw() const {
    return !flags.test(F_DISPLAY_HELP)
        && !flags.test(F_DISPLAY_COLOR_P)
        && !flags.test(F_DISPLAY_BG_COLOR_P)
        && !flags.test(F_DISPLAY_SAVE)
        && !flags.test(F_COPY_COLOR_MODE)
        && !flags.test(F_DISPLAY_CHANGE_SIZE)
        && !flags.test(F_SELECT_TRI_MODE)
        && !flags.test(F_TRI_EDIT_MODE);
}

void Tri::State::reset_modes() {
    flags.reset(F_DISPLAY_HELP);
    flags.reset(F_DISPLAY_COLOR_P);
    flags.reset(F_DISPLAY_BG_COLOR_P);
    flags.reset(F_DISPLAY_SAVE);
    flags.reset(F_COPY_COLOR_MODE);
    flags.reset(F_DISPLAY_CHANGE_SIZE);
    flags.reset(F_SELECT_TRI_MODE);
    flags.reset(F_TRI_EDIT_MODE);
}

void Tri::State::close_help() {
    flags.reset(F_DISPLAY_HELP);
}

void Tri::State::close_color_picker() {
    flags.reset(F_DISPLAY_COLOR_P);
    flags.set(F_DRAW_CACHE_DIRTY);
}

void Tri::State::close_bg_color_picker() {
    flags.reset(F_DISPLAY_BG_COLOR_P);
    flags.set(F_DRAW_CACHE_DIRTY);
}

bool Tri::State::change_width_height() {
    std::bitset<2> warnings;
    if(inputWidthHeight[0] < 0 || inputWidthHeight[1] < 0) {
        failedMessage = "Width or Height cannot be less than 0";
        return false;
    }
    if(inputWidthHeight[0] < 200) {
        inputWidthHeight[0] = 200;
        warnings.set(F_DISPLAY_HELP);
    }
    if(inputWidthHeight[1] < 150) {
        inputWidthHeight[1] = 150;
        warnings.set(F_IS_RUNNING);
    }

    if(warnings.test(0) && warnings.test(1)) {
        set_notification_text("Width set to 200\nHeight set to 150");
    } else if(warnings.test(0)) {
        set_notification_text("Width set to 200");
    } else if(warnings.test(1)) {
        set_notification_text("Height set to 150");
    }

    this->width = inputWidthHeight[0];
    this->height = inputWidthHeight[1];

    window.setSize({this->width, this->height});
    sf::View newView(
        sf::Vector2f(width / 2.0f, height / 2.0f),
        sf::Vector2f(width, height));
    window.setView(newView);

    drawCache.create(width, height);
    drawCacheSprite.setTexture(drawCache.getTexture(), true);
    flags.set(F_DRAW_CACHE_DIRTY);

    currentTri_state = CurrentState::NONE;

    return true;
}

int* Tri::State::get_input_width_height() {
    return inputWidthHeight;
}

void Tri::State::close_input_width_height_window() {
    flags.reset(F_DISPLAY_CHANGE_SIZE);
}

float Tri::State::get_pi() const {
    return pi;
}

float* Tri::State::get_selected_tri_color() {
    tris.at(selectedTri).setFillColor(sf::Color(
        (unsigned char)(255.0f * selectedTriColor[0]),
        (unsigned char)(255.0f * selectedTriColor[1]),
        (unsigned char)(255.0f * selectedTriColor[2]),
        (unsigned char)(255.0f * selectedTriColor[3])));
    return selectedTriColor;
}

void Tri::State::close_selected_tri_mode() {
    tris.at(selectedTri).setFillColor(sf::Color(
        (unsigned char)(255.0f * selectedTriColor[0]),
        (unsigned char)(255.0f * selectedTriColor[1]),
        (unsigned char)(255.0f * selectedTriColor[2]),
        (unsigned char)(255.0f * selectedTriColor[3])));
    flags.set(F_DRAW_CACHE_DIRTY);
    reset_modes();
}

void Tri::State::restore_points_on_tri_del(Action::IndexT end) {
    assert(end < history.size()
        && "Index on history must be in range");
    currentTri[2].x = history[end].data.tri[4];
    currentTri[2].y = history[end].data.tri[5];
    pointCircle.setFillColor(history[end].color);
    unsigned int currentTriIdx = 1;
    while(end-- > 0) {
        if(history[end].type == Action::AT_POINT) {
            assert(history[end].idx == currentTriIdx
                && "Last point must be second point");
            currentTri[currentTriIdx].x = history[end].data.point[0];
            currentTri[currentTriIdx].y = history[end].data.point[1];
            if(currentTriIdx > 0) {
                --currentTriIdx;
            } else {
                currentTri_state = CurrentState::SECOND;
                return;
            }
        }
    }

    assert(!"Unreachable code");
    return;
}
