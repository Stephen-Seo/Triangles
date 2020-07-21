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

    sf::RenderWindow window(
        sf::VideoMode(state.width, state.height),
        "Triangles",
        sf::Style::Titlebar | sf::Style::Close);
    ImGui::SFML::Init(window);
    window.setFramerateLimit(60);

    // main loop
    sf::Event event;
    while(window.isOpen()) {
        // events
        window.pollEvent(event);
        ImGui::SFML::ProcessEvent(event);
        if(event.type == sf::Event::Closed) {
            window.close();
        }
        state.handle_event(&event);

        // update
        ImGui::SFML::Update(window, state.dt);

        state.update();

        ImGui::EndFrame();
        // update end

        // draw
        window.clear();
        ImGui::SFML::Render(window);
        state.draw();
        window.display();
    }

    // cleanup
    window.close();
    ImGui::SFML::Shutdown();

    return 0;
}
