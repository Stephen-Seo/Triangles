#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <imgui.h>
#include <imgui-SFML.h>

#include "state.hpp"
#include "imgui_helper.hpp"

int main(int argc, char **argv) {
    // init
    Tri::State state{};

    // main loop
    while(state.window.isOpen()) {
        state.handle_events();
        state.update();
        state.draw();
    }

    return 0;
}
