#ifndef TRIANGLES_STATE_HPP
#define TRIANGLES_STATE_HPP

#define TRIANGLES_EDIT_TRI_BLINK_RATE 2.0f

#include <bitset>
#include <vector>
#include <array>

#include "glm/glm.hpp"

#include "triangle.hpp"
#include "circle.hpp"

namespace Tri {
    class State {
    public:
        State(int argc, char **argv);
        ~State();

        enum CurrentState {NONE = 0, FIRST = 1, SECOND = 2};
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
            F_SELECT_TRI_MODE           = 11,
            F_TRI_EDIT_MODE             = 12,
            F_TRI_EDIT_DRAW_TRI         = 13,
            F_TAB_TOGGLE                = 14,
        };

    private:
        // use enum FlagName
        typedef std::bitset<64> BitsetType;
        BitsetType flags;
        unsigned int width;
        unsigned int height;
        float dt;
        float notification_alpha;
        std::array<char, 256> notification_text;

        std::vector<Triangle> tris;
        unsigned int trisIndex;
        std::array<glm::vec2, 3> currentTri;
        CurrentState currentTri_state;
        CurrentState currentTri_maxState;
        Circle pointCircle;

        std::array<float, 4> colorPickerColor;
        std::array<float, 3> bgColorPickerColor;
        Color bgColor;

        std::array<char, 256> saveFilenameBuffer;
        std::string failedMessage;

        RenderTexture2D drawCache;

        const float pi;

        unsigned int selectedTri;
        std::array<float, 4> selectedTriColor;
        float selectedTriBlinkTimer;

        int inputWidth;
        int inputHeight;

    public:
        void handle_events();
        void update();
        void draw();

    private:
        void draw_to_target(RenderTexture2D *target);

    public:
        unsigned int get_width() const;
        unsigned int get_height() const;

        const BitsetType get_flags() const;

        float get_notification_alpha() const;
        const char* get_notification_text() const;

    private:
        void set_notification_text(const char *text);

    public:
        std::array<float, 4>& get_color();
        std::array<float, 3>& get_bg_color();

        std::array<char, 256>* get_save_filename_buffer();
        bool do_save();
        const std::string& failed_message() const;
        void close_save();

    private:
        bool can_draw() const;
        void reset_modes();

    public:
        void close_help();
        void close_color_picker();
        void close_bg_color_picker();

        bool change_width_height();
        void close_input_width_height_window();

        float get_pi() const;

        std::array<float, 4>& get_selected_tri_color();
        void close_selected_tri_mode();

    private:
        bool check_draw_cache();

    public:
        int* get_input_width();
        int* get_input_height();

    };
}

#endif
