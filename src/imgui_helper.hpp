#ifndef TRIANGLES_IMGUI_HELPER_HPP
#define TRIANGLES_IMGUI_HELPER_HPP

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
        if(state->get_flags().test(0)) {
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
            ImGui::Text("Press \"S\" to save what was drawn as a png image");
            if(ImGui::Button("Close")) {
                state->close_help();
            }
            ImGui::End();
        }
    }

    inline void draw_show_help(Tri::State *state) {
        float alpha = state->get_starting_help_alpha();
        if(alpha > 0.0f) {
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);

            ImGui::SetNextWindowPos(sf::Vector2f(
                (state->get_width() - SHOW_HELP_WIDTH) / 2.0f,
                (state->get_height() - SHOW_HELP_HEIGHT) / 2.0f));
            ImGui::SetNextWindowSize(sf::Vector2f(
                SHOW_HELP_WIDTH,
                SHOW_HELP_HEIGHT));
            ImGui::Begin("Use Help Window", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings);
            ImGui::SetWindowFontScale(3.0f);
            ImGui::Text("Press \"H\" for help");
            ImGui::End();

            ImGui::PopStyleVar();
        }
    }

    inline void draw_color_picker(Tri::State *state) {
        if(state->get_flags().test(2)) {
            ImGui::Begin("Tri Color Picker");
            ImGui::ColorPicker4("Tri Color", state->get_color());
            if(ImGui::Button("Close")) {
                state->close_color_picker();
            }
            ImGui::End();
        }
    }

    inline void draw_bg_color_picker(Tri::State *state) {
        if(state->get_flags().test(5)) {
            ImGui::Begin("BG Color Picker");
            ImGui::ColorPicker3("BG Color", state->get_bg_color());
            if(ImGui::Button("Close")) {
                state->close_bg_color_picker();
            }
            ImGui::End();
        }
    }

    inline void draw_save(Tri::State *state) {
        if(state->get_flags().test(6)) {
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
            auto string_view = state->failed_save_message();
            if(!string_view.empty()) {
                ImGui::TextUnformatted(string_view.data(), string_view.data() + string_view.size());
            }
            ImGui::End();
        }
    }
}

#endif
