#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <imgui.h>
#include <imgui-SFML.h>

#include "state.hpp"

int main(int argc, char **argv) {
    // init
    Tri::State state(argc, argv);

    // main loop
    while(state.get_flags().test(1)) {
        state.handle_events();
        state.update();
        state.draw();
    }

    return 0;
}
