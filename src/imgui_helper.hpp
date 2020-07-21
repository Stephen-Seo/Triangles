#ifndef TRIANGLES_IMGUI_HELPER_HPP
#define TRIANGLES_IMGUI_HELPER_HPP

#include <imgui.h>

#include "state.hpp"

namespace Tri {
    inline void draw_help(Tri::State *state) {
        if(state->flags.test(0)) {
            ImGui::SetNextWindowPos(sf::Vector2f(10.0f, 10.0f));
            ImGui::SetNextWindowSize(sf::Vector2f(
                state->width - 20.0f,
                state->height - 20.0f));
            ImGui::Begin("Help Window", nullptr, ImGuiWindowFlags_NoDecoration);
            ImGui::Text("This is the help window.");
            ImGui::End();
        }
    }
}

#endif
