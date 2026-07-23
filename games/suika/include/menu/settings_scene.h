#ifndef SUIKA_SETTINGS_SCENE_H
#define SUIKA_SETTINGS_SCENE_H

#include "bn_vector.h"
#include "bn_sprite_ptr.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sprite_text_generator.h"

#include "scene.h"
#include "typewriter_text.h"

namespace suika
{

// Settings / admin screen. The main list only exposes the "Admin Panel" option
// (disabled and grayed out until the secret combo is entered) plus Back. Once
// unlocked, the admin panel offers System Status, Logs and Developer Tools.
// Move with UP/DOWN, confirm with A or START; B goes back one level.
class settings_scene : public scene
{

public:
    explicit settings_scene(bn::sprite_text_generator& text_generator);

    [[nodiscard]] bn::optional<scene_type> update() final;

private:
    // Which screen of the admin flow is currently shown.
    enum class view
    {
        list,            // settings list: Admin Panel / Back
        panel,           // admin panel menu
        system_status,   // read-only status screen
        logs,            // read-only log screen
        dev_tools,       // developer tools menu
        player1_dialog   // Player 1 conversation (typewriter text)
    };

    bn::sprite_text_generator& _text_generator;
    bn::regular_bg_ptr _bg;
    bn::vector<bn::sprite_ptr, 16> _title_sprites;
    bn::vector<bn::sprite_ptr, 64> _option_sprites;
    view _view = view::list;
    int _selected = 0;         // highlighted option in the settings list
    int _admin_selected = 0;   // highlighted option in the admin panel
    int _dev_selected = 0;     // highlighted option in developer tools
    int _combo_progress = 0;   // matched keys of the unlock combo so far
    int _status_timer = 0;     // frames spent on the System Status screen
    int _status_selected = 0;  // highlighted entry on the System Status screen
    typewriter_text _player1_dialog;   // letter-by-letter Player 1 conversation

    void _refresh_options();
    void _refresh_title();
    void _refresh_list();
    void _refresh_admin_menu();
    void _refresh_system_status();
    void _refresh_logs();
    void _refresh_dev_tools();
    void _refresh_player1_dialog();

    // Feeds this frame's input to the unlock combo state machine.
    // Returns 0 if input is free for normal handling, 1 if a combo key was
    // consumed this frame, 2 if the combo was just completed.
    int _feed_unlock_combo();

    bn::optional<scene_type> _update_list();
    bn::optional<scene_type> _update_panel();
    bn::optional<scene_type> _update_info_screen();
    bn::optional<scene_type> _update_system_status();
    bn::optional<scene_type> _update_player1_dialog();
    bn::optional<scene_type> _update_dev_tools();
};

}

#endif
