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

        enum CurrentState {NONE = 0, FIRST = 1, SECOND = 2};
    private:
        /*
         * 0 - display help
         * 1 - is running
         * 2 - display color picker
         * 3 - color picker color dirty
         */
        typedef std::bitset<64> BitsetType;
        BitsetType flags;
        unsigned int width;
        unsigned int height;
        const sf::Time dt;
        float starting_help_alpha;

        sf::RenderWindow window;
        std::vector<sf::ConvexShape> tris;
        unsigned int trisIndex;
        sf::Vector2f currentTri[3];
        CurrentState currentTri_state;
        CurrentState currentTri_maxState;
        sf::CircleShape pointCircle;

        sf::Event event;

        float colorPickerColor[4];

    public:
        void handle_events();
        void update();
        void draw();

        unsigned int get_width() const;
        unsigned int get_height() const;

        const BitsetType get_flags() const;

        float get_starting_help_alpha() const;

        float* get_color();
    };
}

#endif
