#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <imgui.h>
#include <imgui-SFML.h>

#include "state.hpp"

#ifdef _MSC_VER
#include "windows.h"

int WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {
    // init
    Tri::State state(0, nullptr);

    // main loop
    while(state.get_flags().test(1)) {
        state.handle_events();
        state.update();
        state.draw();
    }
    return 0;
}
#else
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
#endif
