#ifndef TRIANGLES_IMGUI_HELPER_HPP
#define TRIANGLES_IMGUI_HELPER_HPP

#include <cmath>
#include <optional>

#include <imgui.h>

#include "state.hpp"

#define SHOW_HELP_WIDTH (state->get_width() / 2.0f)
#define SHOW_HELP_HEIGHT (state->get_height() / 2.0f)

#ifndef NDEBUG
# include <cstdio>
#endif

namespace Tri {
    // Seems misleading, but imgui handles setting up the window during update
    // so this should be called during update, not draw
    inline void draw_help(Tri::State *state) {
        if(state->get_flags().test(Tri::State::F_DISPLAY_HELP)) {
            ImGui::SetNextWindowPos(sf::Vector2f(10.0f, 10.0f));
            ImGui::SetNextWindowSize(sf::Vector2f(
                state->get_width() - 20.0f,
                state->get_height() - 20.0f));
            ImGui::SetNextWindowBgAlpha(0.7f);
            ImGui::Begin("Help Window", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings);
            ImGui::Text("This is the help window - Press \"H\" to toggle this window");
            ImGui::Text("Click anywhere to create triangles, one point at a time");
            ImGui::Text("You cannot draw when a window is open");
            ImGui::Text("Press \"U\" to undo. Clicking will remove all future undo history");
            ImGui::Text("Press \"R\" to redo.");
            ImGui::Text("Press \"C\" to change colors");
            ImGui::Text("Press \"B\" to change background color");
            ImGui::Text("Press \"P\" to set current color to a color on screen");
            ImGui::Text("Press \"S\" to save what was drawn as a png image");
            if(ImGui::Button("Close")) {
                state->close_help();
            }
            ImGui::End();
        }
    }

    inline void draw_notification(Tri::State *state) {
        float alpha = state->get_notification_alpha();
        if(alpha > 0.0f) {
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);

            ImGui::SetNextWindowPos(sf::Vector2f(
                (state->get_width() - SHOW_HELP_WIDTH) / 2.0f,
                (state->get_height() - SHOW_HELP_HEIGHT) / 2.0f));
            ImGui::SetNextWindowSize(sf::Vector2f(
                SHOW_HELP_WIDTH,
                SHOW_HELP_HEIGHT));
            ImGui::Begin(
                "Notification Window",
                nullptr,
                ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings);
            ImGui::SetWindowFontScale(3.0f);
            ImGui::Text("%s", state->get_notification_text());
            ImGui::End();

            ImGui::PopStyleVar();
        }
    }

    inline void draw_color_picker(Tri::State *state) {
        if(state->get_flags().test(Tri::State::F_DISPLAY_COLOR_P)) {
            ImGui::Begin("Tri Color Picker");
            ImGui::ColorPicker4("Tri Color", state->get_color());
            if(ImGui::Button("Close")) {
                state->close_color_picker();
            }
            ImGui::End();
        }
    }

    inline void draw_bg_color_picker(Tri::State *state) {
        if(state->get_flags().test(Tri::State::F_DISPLAY_BG_COLOR_P)) {
            ImGui::Begin("BG Color Picker");
            ImGui::ColorPicker3("BG Color", state->get_bg_color());
            if(ImGui::Button("Close")) {
                state->close_bg_color_picker();
            }
            ImGui::End();
        }
    }

    inline void draw_save(Tri::State *state) {
        if(state->get_flags().test(Tri::State::F_DISPLAY_SAVE)) {
            auto *filenameBuffer = state->get_save_filename_buffer();
            ImGui::Begin("Save");
            ImGui::InputText("Filename", filenameBuffer->data(), filenameBuffer->size() - 1);
            if(ImGui::Button("Save")) {
                if(state->do_save()) {
                    state->close_save();
                }
            } else if(ImGui::Button("Cancel")) {
                state->close_save();
            }
            auto string_view = state->failed_message();
            if(!string_view.empty()) {
                ImGui::TextUnformatted(string_view.data(), string_view.data() + string_view.size());
            }
            ImGui::End();
        }
    }

    inline void draw_change_size(Tri::State *state) {
        if(state->get_flags().test(Tri::State::F_DISPLAY_CHANGE_SIZE)) {
            ImGui::Begin("ChangeSize");
            ImGui::InputInt2("Width and Height", state->get_input_width_height());
            auto string_view = state->failed_message();
            if(!string_view.empty()) {
                ImGui::TextUnformatted(string_view.data(), string_view.data() + string_view.size());
            }
            if(ImGui::Button("Cancel")) {
                state->close_input_width_height_window();
            }
            if(ImGui::Button("Set")) {
                if(state->change_width_height()) {
                    state->close_input_width_height_window();
                }
            }
            ImGui::End();
        }
    }

    inline bool is_within_shape(
            const sf::ConvexShape &shape,
            sf::Vector2f xy) {
        std::optional<bool> is_right;
        sf::Transform t = shape.getTransform();
        for(unsigned int i = 0; i < shape.getPointCount(); ++i) {
            sf::Vector2f t_a = t.transformPoint(shape.getPoint(i));
            sf::Vector2f t_b = t.transformPoint(shape.getPoint((i + 1) % shape.getPointCount()));

            t_b = t_b - t_a;
            t_a = xy - t_a;

            // TODO
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

    inline sf::Color invert_color(const sf::Color &other) {
        return sf::Color(255 - other.r, 255 - other.g, 255 - other.b);
    }

    inline void draw_edit_tri(Tri::State *state) {
        if(state->get_flags().test(Tri::State::F_TRI_EDIT_MODE)) {
            ImGui::Begin("Edit Tri Color Picker");
            ImGui::ColorPicker4("Tri Color", state->get_selected_tri_color());
            if(ImGui::Button("Close")) {
                state->close_selected_tri_mode();
            }
            ImGui::End();
        }
    }
}

#endif
