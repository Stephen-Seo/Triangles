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
            ImGui::Begin("Help Window", nullptr, ImGuiWindowFlags_NoDecoration);
            ImGui::Text("This is the help window. Press \"H\" to toggle this window.");
            ImGui::Text("Click anywhere to create triangles, one point at a time");
            ImGui::Text("Press \"U\" to undo. Clicking will remove all future undo history");
            ImGui::Text("Press \"R\" to undo.");
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
            ImGui::Begin("Use Help Window", nullptr, ImGuiWindowFlags_NoDecoration);
            ImGui::SetWindowFontScale(3.0f);
            ImGui::Text("Press \"H\" for help");
            ImGui::End();

            ImGui::PopStyleVar();
        }
    }
}

#endif
