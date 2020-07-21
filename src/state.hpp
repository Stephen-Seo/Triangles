#ifndef TRIANGLES_STATE_HPP
#define TRIANGLES_STATE_HPP

#include <bitset>

#include <SFML/System.hpp>

namespace Tri {
    struct State {
        State();
        /*
         * 0 - display help
         */
        std::bitset<64> flags;
        const unsigned int width;
        const unsigned int height;
        const sf::Time dt;

        void update();
        void draw();
    };
}

#endif
