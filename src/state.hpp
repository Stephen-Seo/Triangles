#ifndef TRIANGLES_STATE_HPP
#define TRIANGLES_STATE_HPP

#include <bitset>
#include <vector>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

namespace Tri {
    class State {
    public:
        State(int argc, char **argv);
        ~State();
        /*
         * 0 - display help
         * 1 - is running
         */
    private:
        typedef std::bitset<64> BitsetType;
        BitsetType flags;
        unsigned int width;
        unsigned int height;
        const sf::Time dt;

        sf::RenderWindow window;
        std::vector<sf::ConvexShape> tris;
        unsigned int trisIndex;
        sf::Vector2f currentTri[3];
        enum CurrentState { NONE = 0, FIRST = 1, SECOND = 2} currentTri_state;
        sf::CircleShape pointCircle;

        sf::Event event;

    public:
        void handle_events();
        void update();
        void draw();

        unsigned int get_width();
        unsigned int get_height();

        const BitsetType get_flags() const;
    };
}

#endif
