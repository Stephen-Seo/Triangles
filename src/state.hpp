#ifndef TRIANGLES_STATE_HPP
#define TRIANGLES_STATE_HPP

#include <bitset>
#include <vector>
#include <array>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

namespace Tri {
    class State {
    public:
        State(int argc, char **argv);
        ~State();

        enum CurrentState {NONE = 0, FIRST = 1, SECOND = 2};
    private:
        enum FlagName {
            F_DISPLAY_HELP              = 0,
            F_IS_RUNNING                = 1,
            F_DISPLAY_COLOR_P           = 2,
            F_COLOR_P_COLOR_DIRTY       = 3,
            F_BG_COLOR_P_COLOR_DIRTY    = 4,
            F_DISPLAY_BG_COLOR_P        = 5,
            F_DISPLAY_SAVE              = 6,
            F_DRAW_CACHE_DIRTY          = 7,
            F_DRAW_CACHE_INITIALIZED    = 8,
            F_COPY_COLOR_MODE           = 9,
            F_DISPLAY_CHANGE_SIZE       = 10,
        };

        // use enum FlagName
        typedef std::bitset<64> BitsetType;
        BitsetType flags;
        unsigned int width;
        unsigned int height;
        const sf::Time dt;
        float notification_alpha;
        typedef std::array<char, 256> NotificationBufferType;
        NotificationBufferType notification_text;

        sf::RenderWindow window;
        std::vector<sf::ConvexShape> tris;
        unsigned int trisIndex;
        sf::Vector2f currentTri[3];
        CurrentState currentTri_state;
        CurrentState currentTri_maxState;
        sf::CircleShape pointCircle;

        sf::Event event;

        float colorPickerColor[4];
        float bgColorPickerColor[3];
        sf::Color bgColor;

        typedef std::array<char, 256> FilenameBufferType;
        FilenameBufferType saveFilenameBuffer;
        std::string failedMessage;

        sf::RenderTexture drawCache;
        sf::Sprite drawCacheSprite;

        int inputWidthHeight[2];

    public:
        void handle_events();
        void update();
        void draw();

    private:
        void draw_to_target(sf::RenderTarget *target);

    public:
        unsigned int get_width() const;
        unsigned int get_height() const;

        const BitsetType get_flags() const;

        float get_notification_alpha() const;
        const char* get_notification_text() const;

        float* get_color();
        float* get_bg_color();

        FilenameBufferType* get_save_filename_buffer();
        bool do_save();
        std::string_view failed_message() const;
        void close_save();

    private:
        bool can_draw() const;

    public:
        void close_help();
        void close_color_picker();
        void close_bg_color_picker();

        bool change_width_height();
        int* get_input_width_height();
        void close_input_width_height_window();

    };
}

#endif
