#ifndef TRIANGLES_STATE_HPP
#define TRIANGLES_STATE_HPP

#include <bitset>
#include <vector>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

namespace Tri {
    struct State {
        State();
        ~State();
        /*
         * 0 - display help
         */
        std::bitset<64> flags;
        const unsigned int width;
        const unsigned int height;
        const sf::Time dt;

        sf::RenderWindow window;
        std::vector<sf::ConvexShape> tris;
        sf::ConvexShape currentTri;
        enum CurrentState { NONE, FIRST, SECOND, THIRD } currentTri_state;

        sf::Event event;

        void handle_events();
        void update();
        void draw();
    };
}

#endif
