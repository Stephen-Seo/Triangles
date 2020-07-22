#ifndef TRIANGLES_IMGUI_HELPER_HPP
#define TRIANGLES_IMGUI_HELPER_HPP

#include <imgui.h>

#include "state.hpp"

namespace Tri {
    // Seems misleading, but imgui handles setting up the window during update
    // so this should be called during update, not draw
    inline void draw_help(Tri::State *state) {
        if(state->get_flags().test(0)) {
            ImGui::SetNextWindowPos(sf::Vector2f(10.0f, 10.0f));
            ImGui::SetNextWindowSize(sf::Vector2f(
                state->get_width() - 20.0f,
                state->get_height() - 20.0f));
            ImGui::Begin("Help Window", nullptr, ImGuiWindowFlags_NoDecoration);
            ImGui::Text("This is the help window.");
            ImGui::End();
        }
    }
}

#endif
