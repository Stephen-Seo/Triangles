#include "state.hpp"

#include <cstring>
#include <cassert>
#include <string>
#include <cmath>
#include <cstdio>

#include <raylib.h>

#include "helpers.hpp"

#define STARTING_HELP_FADE_RATE 0.2f

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
Tri::State::State(int argc, char **argv) :
flags(),
width(DEFAULT_WIDTH),
height(DEFAULT_HEIGHT),
dt(1.0f/60.0f),
notificationAlpha(1.0f),
notificationText(),
tris(),
currentTri(),
currentTri_state(CurrentState::NONE),
pointCircle({7.0F, 7.0F}, 7.0F, WHITE),
colorPickerColor{255, 255, 255, 255},
bgColorPickerColor{0, 0, 0, 255},
bgColor(BLACK),
saveFilenameBuffer(),
failedMessage(),
drawCache(),
pi(std::acos(-1.0f)),
selectedTri(),
selectedTriColor{255, 255, 255, 255},
selectedTriBlinkTimer(),
inputWidth(800),
inputHeight(600),
history(),
history_idx(0)
{
    InitWindow(width, height, "Triangles");
    SetTargetFPS(60);

    flags.set(F_IS_RUNNING); // is running

    set_notification_text("Press \"H\" for help");

    saveFilenameBuffer.fill(0);

    drawCache = LoadRenderTexture(width, height);
    flags.set(F_DRAW_CACHE_INITIALIZED);
    flags.set(F_DRAW_CACHE_DIRTY);

    GuiSetStyle(DEFAULT, BACKGROUND_COLOR, 0x303030);
}
#pragma GCC diagnostic pop

Tri::State::Action::Action() :
type(Tri::State::Action::AT_NONE),
idx(0),
color(BLACK),
data{{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}}
{}

Tri::State::Action::Action(const Type& type, IndexT idx, const Color& color, float *data) :
type(type),
idx(idx),
color(color),
data()
{
    init(data);
}

Tri::State::Action::Action(Type&& type, IndexT idx, Color&& color, float *data) :
type(type),
idx(idx),
color(color),
data()
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
        color = BLACK;
        this->data = {{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}};
        break;
    }
}

Tri::State::~State() {
    UnloadRenderTexture(drawCache);
    CloseWindow();
}

void Tri::State::handle_events() {
    if(WindowShouldClose()) {
        flags.reset(F_IS_RUNNING);
    }

    int keyPressed = GetKeyPressed();
    while(keyPressed > 0) {
        if(!flags.test(F_DISPLAY_SAVE)) {
            switch(keyPressed) {
            case KEY_H:
                flags.flip(F_DISPLAY_HELP);
                break;
            case KEY_U:
                flags.set(F_DRAW_CACHE_DIRTY);
                if (history_idx > 0) {
                    switch(history[history_idx-1].type) {
                    case Action::AT_TRI:
                        tris.erase(
                            tris.cbegin() + history[history_idx-1].idx);
                        restore_points_on_tri_del(history_idx - 1);
                        break;
                    case Action::AT_TRI_DEL:
                    {
                        tris.emplace(
                            tris.cbegin() + history[history_idx].idx,
                            Triangle(
                                {{
                                    {history[history_idx-1].data.tri[0],
                                     history[history_idx-1].data.tri[1]},
                                    {history[history_idx-1].data.tri[2],
                                     history[history_idx-1].data.tri[3]},
                                    {history[history_idx-1].data.tri[4],
                                     history[history_idx-1].data.tri[5]},
                                }},
                                history[history_idx-1].color
                            )
                        );
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
                break;
            case KEY_R:
                flags.set(F_DRAW_CACHE_DIRTY);
                if(history_idx < history.size()) {
                    switch(history[history_idx].type) {
                    case Action::AT_TRI:
                    {
                        tris.emplace(
                            tris.cbegin() + history[history_idx].idx,
                            Triangle(
                                {{
                                    {history[history_idx].data.tri[0],
                                     history[history_idx].data.tri[1]},
                                    {history[history_idx].data.tri[2],
                                     history[history_idx].data.tri[3]},
                                    {history[history_idx].data.tri[4],
                                     history[history_idx].data.tri[5]},
                                }},
                                history[history_idx].color
                            )
                        );
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
                        pointCircle.fillColor = history[history_idx].color;
                        break;
                    default:
                        assert(!"Unreachable code");
                        break;
                    }
                    ++history_idx;
                }
                break;
            case KEY_C:
                if(flags.test(F_DISPLAY_COLOR_P)) {
                    close_color_picker();
                } else {
                    flags.set(F_DISPLAY_COLOR_P);
                }
                break;
            case KEY_B:
                if(flags.test(F_DISPLAY_BG_COLOR_P)) {
                    close_bg_color_picker();
                } else {
                    flags.set(F_DISPLAY_BG_COLOR_P);
                }
                break;
            case KEY_S:
                flags.flip(F_DISPLAY_SAVE);
                break;
            case KEY_P:
                flags.flip(F_COPY_COLOR_MODE);
                if(flags.test(F_COPY_COLOR_MODE)) {
                    set_notification_text(
                        "Copy color mode\n"
                        "Click to change\n"
                        "current draw color\n"
                        "to what was\n"
                        "clicked on");
                } else {
                    notificationAlpha = 0.0f;
                }
                break;
            case KEY_I:
                flags.flip(F_DISPLAY_CHANGE_SIZE);
                if(!flags.test(F_DISPLAY_CHANGE_SIZE)) {
                    close_input_width_height_window();
                } else {
                    failedMessage = "Press TAB to switch between width/height";
                }
                break;
            case KEY_E:
                if(flags.test(F_TRI_EDIT_MODE)) {
                    close_selected_tri_mode();
                } else {
                    flags.flip(F_SELECT_TRI_MODE);
                    if(flags.test(F_SELECT_TRI_MODE)) {
                        set_notification_text("Click on a tri\nto edit it");
                    }
                }
                break;
            case KEY_TAB:
                flags.flip(F_TAB_TOGGLE);
                break;
            }
        }
        keyPressed = GetKeyPressed();
    }

    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if(can_draw()) {
            switch(currentTri_state) {
            case CurrentState::NONE: {
                currentTri[0] = {GetMouseX(), GetMouseY()};
                currentTri_state = CurrentState::FIRST;
                if(history_idx < history.size()) {
                    history.resize(history_idx);
                }
                float points[2] = {currentTri[0].x, currentTri[0].y};
                history.push_back(Action(Action::AT_POINT,
                                         0,
                                         pointCircle.fillColor,
                                         points));
                ++history_idx;
                break; }
            case CurrentState::FIRST: {
                currentTri[1] = {GetMouseX(), GetMouseY()};
                currentTri_state = CurrentState::SECOND;
                if(history_idx < history.size()) {
                    history.resize(history_idx);
                }
                float points[2] = {currentTri[1].x, currentTri[1].y};
                history.push_back(Action(Action::AT_POINT,
                                         1,
                                         pointCircle.fillColor,
                                         points));
                ++history_idx;
                break; }
            case CurrentState::SECOND: {
                currentTri[2] = {GetMouseX(), GetMouseY()};
                make_counter_clockwise(currentTri);
                tris.emplace_back(currentTri, pointCircle.fillColor);
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
                                         pointCircle.fillColor,
                                         points));
                ++history_idx;
                break; }
            }
        } else if(flags.test(F_COPY_COLOR_MODE)) {
            check_draw_cache();
            Image drawImage = LoadImageFromTexture(drawCache.texture);
            Color *colors = LoadImageColors(drawImage);
            int mx = GetMouseX();
            int my = GetMouseY();
            if(mx < 0) { mx = 0; }
            else if(mx >= drawImage.width) { mx = drawImage.width - 1; }
            if(my < 0) { my = 0; }
            else if(my >= drawImage.height) { my = drawImage.height - 1; }

            my = drawImage.height - my;

            colorPickerColor = colors[mx + my * drawImage.width];

            pointCircle.fillColor = colors[mx + my * drawImage.width];
            flags.reset(F_COPY_COLOR_MODE);
            set_notification_text("Color set");

            UnloadImageColors(colors);
            UnloadImage(drawImage);
        } else if(flags.test(F_SELECT_TRI_MODE)) {
            int mx = GetMouseX();
            int my = GetMouseY();
            if(mx < 0) { mx = 0; }
            else if(mx >= (int)width) { mx = width - 1; }
            if(my < 0) { my = 0; }
            else if(my >= (int)height) { my = height - 1; }

            for (auto &tri : tris) {
                if(is_within_shape(tri, {mx, my})) {
                    tri.outlineColor = invert_color(tri.fillColor);
                    flags.reset(F_SELECT_TRI_MODE);
                    flags.set(F_TRI_EDIT_MODE);
                    flags.set(F_TRI_EDIT_DRAW_TRI);
                    selectedTriBlinkTimer = 1.0f;
                    selectedTriColor = tri.fillColor;
                    break;
                }
            }
            if(!flags.test(F_TRI_EDIT_MODE)) {
                set_notification_text("Did not select\nanything");
            }
        }
    }
}

void Tri::State::update() {
    dt = GetFrameTime();

    if(notificationAlpha > 0.0f) {
        notificationAlpha -= dt * STARTING_HELP_FADE_RATE;
        if(notificationAlpha < 0.0f) {
            notificationAlpha = 0.0f;
        }
    }

    if(flags.test(F_COLOR_P_COLOR_DIRTY)) {
        flags.reset(F_COLOR_P_COLOR_DIRTY);
        pointCircle.fillColor = colorPickerColor;
    }

    if(flags.test(F_BG_COLOR_P_COLOR_DIRTY)) {
        flags.reset(F_BG_COLOR_P_COLOR_DIRTY);
        bgColor = bgColorPickerColor;
        bgColor.a = 255;
    }

    if(flags.test(F_TRI_EDIT_MODE)) {
        selectedTriBlinkTimer -= dt * TRIANGLES_EDIT_TRI_BLINK_RATE;
        if(selectedTriBlinkTimer <= 0.0f) {
            selectedTriBlinkTimer = 1.0f;
            flags.flip(F_TRI_EDIT_DRAW_TRI);
        }
    }
}

void Tri::State::draw() {
    // Should be able to directly draw a texture held by the RenderTexture2D
    if(flags.test(F_DRAW_CACHE_INITIALIZED)) {
        // draw cache initialized
        check_draw_cache();
        BeginDrawing();
        // hack to flip in the y direction, since RenderTexture2D's texture
        // is flipped
        DrawTextureRec(
            drawCache.texture,
            {
                0.0f,
                0.0f,
                (float)drawCache.texture.width,
                (float)-drawCache.texture.height
            },
            {0.0f, 0.0f},
            WHITE);
    } else {
        BeginDrawing();
        draw_to_target(nullptr);
    }

    if(flags.test(F_TRI_EDIT_MODE) && flags.test(F_TRI_EDIT_DRAW_TRI)) {
//        tris.at(selectedTri).setOutlineThickness(4.0f);
        tris[selectedTri].draw();
//        tris.at(selectedTri).setOutlineThickness(0.0f);
    }

    if(can_draw()) {
        for(unsigned int i = 0; i < currentTri_state; ++i) {
            DrawCircle(currentTri[i].x, currentTri[i].y, pointCircle.getRadius(), pointCircle.fillColor);
        }
    }

    Tri::draw_notification(this);
    Tri::draw_color_picker(this);
    Tri::draw_bg_color_picker(this);
    Tri::draw_edit_tri(this);
    Tri::draw_change_size(this);
    Tri::draw_save(this);
    Tri::draw_help(this);

    EndDrawing();
}

void Tri::State::draw_to_target(RenderTexture2D *target) {
    if(target) {
        BeginTextureMode(*target);
        ClearBackground(bgColor);

        // draw tris
        for(unsigned int i = 0; i < tris.size(); ++i) {
            tris[i].draw();
        }
        EndTextureMode();
    } else {
        // Expects BeginDrawing() already having been called prior to this fn
        ClearBackground(bgColor);

        // draw tris
        for(unsigned int i = 0; i < tris.size(); ++i) {
            tris[i].draw();
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

float Tri::State::get_notification_alpha() const {
    return notificationAlpha;
}

const char* Tri::State::get_notification_text() const {
    return notificationText.data();
}

void Tri::State::set_notification_text(const char *text) {
    notificationText.fill(0);
    std::strncpy(notificationText.data(),
        text,
        notificationText.max_size() - 1);
    notificationAlpha = 1.0f;
}

void Tri::State::append_notification_text(const char *text) {
    auto length = std::strlen(notificationText.data());
    if(length + 1 >= notificationText.max_size()) {
        return;
    }
    std::strncpy(
        notificationText.data() + length,
        text,
        notificationText.max_size() - length - 1);
    notificationAlpha = 1.0f;
}

Color& Tri::State::get_color() {
    flags.set(F_COLOR_P_COLOR_DIRTY);
    return colorPickerColor;
}

Color& Tri::State::get_bg_color() {
    flags.set(F_BG_COLOR_P_COLOR_DIRTY);
    return bgColorPickerColor;
}

std::array<char, 256>* Tri::State::get_save_filename_buffer() {
    return &saveFilenameBuffer;
}

bool Tri::State::do_save() {
    RenderTexture2D saveTexture = LoadRenderTexture(width, height);

    draw_to_target(&saveTexture);

    Image saveImage = LoadImageFromTexture(saveTexture.texture);
    ImageFlipVertical(&saveImage);
    UnloadRenderTexture(saveTexture);
    if(ExportImage(saveImage, saveFilenameBuffer.data())) {
#ifndef NDEBUG
        printf("Saved to \"%s\"\n", saveFilenameBuffer.data());
#endif
        failedMessage.clear();
        UnloadImage(saveImage);
        return true;
    } else {
#ifndef NDEBUG
        printf("ERROR: Failed to save \"%s\"\n", saveFilenameBuffer.data());
#endif
        failedMessage = std::string("Failed to save (does the name end in \".png\"?)");
        UnloadImage(saveImage);
        return false;
    }
}

const std::string& Tri::State::failed_message() const {
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
    if(inputWidth < 0 || inputHeight < 0) {
        failedMessage = "Width or Height cannot be less than 0";
        return false;
    }
    if(inputWidth < 800) {
        inputWidth = 800;
    }
    if(inputHeight < 600) {
        inputHeight = 600;
    }

    notificationText.fill(0);
    std::array<char, 5> tempBuf = {0, 0, 0, 0, 0};

    append_notification_text("Width set to ");
    snprintf(tempBuf.data(), 5, "%u", inputWidth);
    append_notification_text(tempBuf.data());

    append_notification_text(", Height set to ");
    snprintf(tempBuf.data(), 5, "%u", inputHeight);
    append_notification_text(tempBuf.data());

    this->width = inputWidth;
    this->height = inputHeight;

    UnloadRenderTexture(drawCache);
    SetWindowSize(this->width, this->height);
    drawCache = LoadRenderTexture(this->width, this->height);

    flags.set(F_DRAW_CACHE_DIRTY);

    currentTri_state = CurrentState::NONE;

    return true;
}

void Tri::State::close_input_width_height_window() {
    failedMessage.clear();
    inputWidth = width;
    inputHeight = height;
    flags.reset(F_DISPLAY_CHANGE_SIZE);
}

float Tri::State::get_pi() const {
    return pi;
}

Color& Tri::State::get_selected_tri_color() {
    tris.at(selectedTri).fillColor = selectedTriColor;
    return selectedTriColor;
}

void Tri::State::close_selected_tri_mode() {
    tris.at(selectedTri).fillColor = selectedTriColor;
    flags.set(F_DRAW_CACHE_DIRTY);
    reset_modes();
}

bool Tri::State::check_draw_cache() {
    if(flags.test(F_DRAW_CACHE_INITIALIZED) && flags.test(F_DRAW_CACHE_DIRTY)) {
        // draw cache initialized and dirty
        flags.reset(F_DRAW_CACHE_DIRTY);
        draw_to_target(&drawCache);
        return true;
    } else {
        return false;
    }
}

int* Tri::State::get_input_width() {
    return &inputWidth;
}

int* Tri::State::get_input_height() {
    return &inputHeight;
}

void Tri::State::restore_points_on_tri_del(Action::IndexT end) {
    assert(end < history.size()
        && "Index on history must be in range");
    currentTri[2].x = history[end].data.tri[4];
    currentTri[2].y = history[end].data.tri[5];
    pointCircle.fillColor = history[end].color;
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
