#ifndef TRIANGLES_STATE_HPP
#define TRIANGLES_STATE_HPP

#define TRIANGLES_EDIT_TRI_BLINK_RATE 2.0f

#include <bitset>
#include <vector>
#include <array>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
#include "glm/glm.hpp"
#pragma GCC diagnostic pop

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
        struct Action {
        public:
            typedef std::vector<Action>::size_type IndexT;

            enum Type {
                AT_TRI,
                AT_TRI_DEL,
                AT_POINT,
                AT_COLOR,
                AT_NONE,
            };

            Action();
            Action(const Type& type,
                   IndexT idx,
                   const Color& color,
                   float *data);
            Action(Type&& type,
                   IndexT idx,
                   Color&& color,
                   float *data);

            Type type;
            IndexT idx;
            Color color;
            union Data {
                float tri[6];
                float point[2];
                Color newColor;
            } data;

            Action &setNewColor(Color color);

        private:
            void init(float *data);
        };

        // use enum FlagName
        typedef std::bitset<64> BitsetType;
        BitsetType flags;
        unsigned int width;
        unsigned int height;
        float dt;
        float notificationAlpha;
        float notificationAmt;
        std::array<char, 256> notificationText;

        std::vector<Triangle> tris;
        std::array<glm::vec2, 3> currentTri;
        CurrentState currentTri_state;
        Circle pointCircle;

        Color colorPickerColor;
        Color bgColorPickerColor;
        Color bgColor;

        std::array<char, 256> saveFilenameBuffer;
        std::string failedMessage;

        RenderTexture2D drawCache;

        const float pi;

        unsigned int selectedTri;
        Color selectedTriColor;
        Color prevTriColor;
        float selectedTriBlinkTimer;

        int inputWidth;
        int inputHeight;

        std::vector<Action> history;
        Action::IndexT history_idx;

        float clickTimeout;

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
        void reset_notification_alpha();
        void clear_notification_alpha();

        const char* get_notification_text() const;

    private:
        void set_notification_text(const char *text);
        void append_notification_text(const char *text);

    public:
        Color& get_color();
        Color& get_bg_color();

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

        Color& get_selected_tri_color();
        void close_selected_tri_mode();

    private:
        bool check_draw_cache();

    public:
        int* get_input_width();
        int* get_input_height();

    private:
        void restore_points_on_tri_del(Action::IndexT end);

    public:
        float get_click_timeout() const;
    };
}

#endif
