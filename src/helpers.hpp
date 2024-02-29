#ifndef TRIANGLES_IMGUI_HELPER_HPP
#define TRIANGLES_IMGUI_HELPER_HPP

#include <cmath>
#include <optional>
#include <cassert>

#include <raylib.h>
#include <raygui.h>

#include "shape.hpp"
#include "state.hpp"

#define SHOW_HELP_WIDTH (state->get_width() / 2.0f)
#define SHOW_HELP_HEIGHT (state->get_height() / 2.0f)

#define NOTIFICATION_FADE_RATE 0.2F

#define DEFAULT_WIDTH 800
#define DEFAULT_HEIGHT 600
#define CHANGE_SIZE_MIN_X 800
#define CHANGE_SIZE_MAX_X 1920
#define CHANGE_SIZE_MIN_Y 600
#define CHANGE_SIZE_MAX_Y 1080

#define TRI_GUI_BG_COLOR 0x505050FF
#define TRI_GUI_BASE_COLOR 0x404040FF
#define TRI_GUI_TEXT_COLOR 0xFFFFFFFF

#define CLICK_TIMEOUT_TIME 0.4F

#ifndef NDEBUG
# include <cstdio>
#endif

namespace Tri {
    // Seems misleading, but imgui handles setting up the window during update
    // so this should be called during update, not draw
    inline void draw_help(Tri::State *state) {
        if(state->get_flags().test(Tri::State::F_DISPLAY_HELP)) {
            GuiSetAlpha(1.0f);
            if(!GuiWindowBox({10.0f,
                             10.0f,
                             800.0f - 20.0f,
                             600.0f - 20.0f},
                             "Help")) {
                GuiLabel(
                    {14.0f, 38.0f, 800.0f - 28.0f, 16.0f},
                    "This is the help window - Press \"H\" to toggle this window");
                GuiLabel(
                    {14.0f, 56.0f, 800.0f - 28.0f, 16.0f},
                    "Click anywhere to create triangles, one point at a time");
                GuiLabel(
                    {14.0f, 74.0f, 800.0f - 28.0f, 16.0f},
                    "You cannot draw when a window is open");
                GuiLabel(
                    {14.0f, 92.0f, 800.0f - 28.0f, 16.0f},
                    "Press \"U\" to undo. Clicking will remove all future undo history");
                GuiLabel(
                    {14.0f, 110.0f, 800.0f - 28.0f, 16.0f},
                    "Press \"R\" to redo.");
                GuiLabel(
                    {14.0f, 128.0f, 800.0f - 28.0f, 16.0f},
                    "Press \"C\" to change colors");
                GuiLabel(
                    {14.0f, 146.0f, 800.0f - 28.0f, 16.0f},
                    "Press \"B\" to change background color");
                GuiLabel(
                    {14.0f, 164.0f, 800.0f - 28.0f, 16.0f},
                    "Press \"P\" to set current color to a color on screen");
                GuiLabel(
                    {14.0f, 182.0f, 800.0f - 28.0f, 16.0f},
                    "Press \"S\" to save what was drawn as a png image");
                GuiLabel(
                    {14.0f, 200.0f, 800.0f - 28.0f, 16.0f},
                    "Press \"I\" to resize the canvas");
                GuiLabel(
                    {14.0f, 218.0f, 800.0f - 28.0f, 16.0f},
                    "Press \"E\" to edit the selected tri");
                if(GuiButton({14.0f, 238.0f, 100.0f, 16.0f}, "Close")) {
                    state->close_help();
                }
            } else {
                state->close_help();
            }
        }
    }

    inline void draw_notification(Tri::State *state) {
        float alpha = state->get_notification_alpha();
        if(alpha > 0.0f) {
            GuiSetAlpha(alpha);
            GuiPanel({(800 - SHOW_HELP_WIDTH) / 2.0f,
                      (600 - SHOW_HELP_HEIGHT) / 2.0f,
                      SHOW_HELP_WIDTH,
                      SHOW_HELP_HEIGHT}, nullptr);
            GuiLabel({4.0f + (800 - SHOW_HELP_WIDTH) / 2.0f,
                      4.0f + (600 - SHOW_HELP_HEIGHT) / 2.0f,
                      SHOW_HELP_WIDTH - 8.0f,
                      SHOW_HELP_HEIGHT - 8.0f},
                     state->get_notification_text());
        }
    }

    inline void draw_color_picker(Tri::State *state) {
        if(state->get_flags().test(Tri::State::F_DISPLAY_COLOR_P)) {
            GuiSetAlpha(1.0f);
            if(!GuiWindowBox({4.0f, 4.0f, 242.0f, 328.0f}, "Tri Color Picker")) {
                auto &color = state->get_color();
                GuiColorPicker(
                    {8.0f, 32.0f, 206.0f, 240.0f},
                    nullptr,
                    &color);
                float alpha = ((float)color.a) / 255.0F;
                GuiColorBarAlpha(
                    {8.0F, 280.0F, 206.0F, 20.0F},
                    nullptr,
                    &alpha
                );
                color.a = alpha * 255.0F;
                if(GuiButton({8.0f, 308.0f, 234.0f, 16.0f}, "Close")) {
                    state->close_color_picker();
                }
            } else {
                state->close_color_picker();
            }
        }
    }

    inline void draw_bg_color_picker(Tri::State *state) {
        if(state->get_flags().test(Tri::State::F_DISPLAY_BG_COLOR_P)) {
            GuiSetAlpha(1.0f);
            if(!GuiWindowBox({250.0f, 4.0f, 242.0f, 292.0f}, "BG Color Picker")) {
                auto &colorArray = state->get_bg_color();
                GuiColorPicker(
                    {254.0f, 32.0f, 206.0f, 240.0f},
                    nullptr,
                    &colorArray);
                if(GuiButton({254.0f, 274.0f, 234.0f, 16.0f}, "Close")) {
                    state->close_bg_color_picker();
                }
            } else {
                state->close_bg_color_picker();
            }
        }
    }

    inline void draw_save(Tri::State *state) {
        if(state->get_flags().test(Tri::State::F_DISPLAY_SAVE)) {
            auto *filenameBuffer = state->get_save_filename_buffer();
            GuiSetAlpha(1.0f);
            if(!GuiWindowBox({4.0f, 300.0f, 292.0f, 292.0f}, "Save")) {
                GuiTextBox(
                    {8.0f, 328.0f, 284.0f, 20.0f},
                    filenameBuffer->data(),
                    filenameBuffer->size() - 1,
                    true);
                if(GuiButton({8.0f, 352.0f, 50.0f, 16.0f}, "Save")) {
                    if(state->do_save()) {
                        state->close_save();
                    }
                }
                const std::string &string = state->failed_message();
                if(!string.empty()) {
                    GuiLabel({8.0f, 372.0f, 284.0f, 16.0f}, string.c_str());
                }
            } else {
                state->close_save();
            }
        }
    }

    inline void draw_change_size(Tri::State *state) {
        if(state->get_flags().test(Tri::State::F_DISPLAY_CHANGE_SIZE)) {
            GuiSetAlpha(1.0f);
            if(!GuiWindowBox({300.0f, 300.0f, 292.0f, 292.0f}, "Change Size")) {
                GuiValueBox(
                        {384.0f, 328.0f, 80.0f, 16.0f},
                        "Width",
                        state->get_input_width(),
                        CHANGE_SIZE_MIN_X,
                        CHANGE_SIZE_MAX_X,
                        !state->get_flags().test(State::F_TAB_TOGGLE));
                GuiValueBox(
                        {384.0f, 348.0f, 80.0f, 16.0f},
                        "Height",
                        state->get_input_height(),
                        CHANGE_SIZE_MIN_Y,
                        CHANGE_SIZE_MAX_Y,
                        state->get_flags().test(State::F_TAB_TOGGLE));
                const std::string &failMessage = state->failed_message();
                if(!failMessage.empty()) {
                    GuiLabel({304.0f, 368.0f, 284.0f, 16.0f}, failMessage.c_str());
                }
                if(GuiButton({304.0f, 394.0f, 70.0f, 16.0f}, "Cancel")) {
                    state->close_input_width_height_window();
                }
                if(GuiButton({378.0f, 394.0f, 50.0f, 16.0f}, "Set")) {
                    if(state->change_width_height()) {
                        state->close_input_width_height_window();
                    }
                }
            } else {
                state->close_input_width_height_window();
            }
        }
    }

    inline bool is_within_shape(
            const Tri::Shape &shape,
            glm::vec2 xy) {
        float radius = shape.getRadius();
        std::vector<glm::vec2> vertices;
        shape.getTransformedVertices(vertices);
        if(radius > 0.0f) {
            assert(vertices.size() == 1);

            xy = xy - vertices[0];
            return std::sqrt(xy.x * xy.x + xy.y * xy.y) <= radius;
        } else {
            assert(vertices.size() > 2);

            std::optional<bool> is_right;
            for(unsigned int i = 0; i < vertices.size(); ++i) {
                glm::vec2 t_a, t_b;
                t_b = vertices[(i + 1) % vertices.size()] - vertices[i];
                t_a = xy - vertices[i];

                // cross product, where z coordinate is 0
                // Use sign of z value to determine if line is to right or left
                //
                // a   x b   = c
                // a_1   b_1   0
                // a_2   b_2   0
                // 0     0     a_1 * b_2 - a_2 * b_1
                //
                // in this case "a" is "t_b"
                float z = t_b.x * t_a.y - t_b.y * t_a.x;
                if(is_right.has_value()) {
                    if(is_right.value()) {
                        if(z >= 0.0f) {
                            return false;
                        }
                    } else if(z < 0.0f) {
                        return false;
                    }
                } else {
                    is_right = z < 0.0f;
                }
            }
            return true;
        }
    }

    inline Color invert_color(const Color &other) {
        return Color{
            (unsigned char)(255 - (int)other.r),
            (unsigned char)(255 - (int)other.g),
            (unsigned char)(255 - (int)other.b),
            other.a};
    }

    inline void draw_edit_tri(Tri::State *state) {
        if(state->get_flags().test(Tri::State::F_TRI_EDIT_MODE)) {
            GuiSetAlpha(1.0f);
            if(!GuiWindowBox({500.0f, 4.0f, 242.0f, 328.0f}, "Edit Tri Color Picker")) {
                auto &color = state->get_selected_tri_color();
                GuiColorPicker(
                    {504.0f, 32.0f, 206.0f, 240.0f},
                    nullptr,
                    &color);
                float alpha = ((float)color.a) / 255.0F;
                GuiColorBarAlpha(
                    {504.0F, 280.0F, 206.0F, 20.0F},
                    nullptr,
                    &alpha
                );
                color.a = alpha * 255.0F;
                if(GuiButton({504.0f, 308.0f, 234.0f, 16.0f}, "Close")
                        && state->get_click_timeout() == 0.0F) {
                    state->close_selected_tri_mode();
                }
            } else {
                state->close_selected_tri_mode();
            }
        }
    }

    inline void make_counter_clockwise(std::array<glm::vec2, 3> &tri) {
        // 2D cross product to get the sign to determine if clockwise
        glm::vec2 t_a, t_b;
        t_b = tri[1] - tri[0];
        t_a = tri[2] - tri[0];

        float z = t_b.x * t_a.y - t_b.y * t_a.x;
        if(z >= 0.0f) {
            // is clockwise, swap first with last
            t_a = tri[0];
            tri[0] = tri[2];
            tri[2] = t_a;
        }
    }

    inline float sq_lerp(float start, float end, float amt) {
        amt = amt * amt;
        return start * (1.0F - amt) + end * amt;
    }
}

// Color operator helpers
bool operator==(const Color& a, const Color& b);
bool operator!=(const Color& a, const Color& b);

#endif
