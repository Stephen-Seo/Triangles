#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <imgui.h>
#include <imgui-SFML.h>

int main(int argc, char **argv) {
    // init
    sf::RenderWindow window(sf::VideoMode(800, 600), "Triangles");
    ImGui::SFML::Init(window);
    window.setFramerateLimit(60);
    const sf::Time dt = sf::microseconds(16666);

    // main loop
    sf::Event event;
    while(window.isOpen()) {
        // events
        window.pollEvent(event);
        ImGui::SFML::ProcessEvent(event);
        if(event.type == sf::Event::Closed) {
            window.close();
        }

        // update
        ImGui::SFML::Update(window, dt);

        ImGui::Begin("Test window");
        ImGui::Text("Test text");

        ImGui::End();
        ImGui::EndFrame();
        // update end

        // draw
        window.clear();
        ImGui::SFML::Render(window);
        window.display();
    }

    // cleanup
    window.close();
    ImGui::SFML::Shutdown();

    return 0;
}
