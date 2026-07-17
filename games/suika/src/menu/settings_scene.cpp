#include "settings_scene.h"

#include "bn_keypad.h"
#include "bn_string.h"
#include "bn_sprite_palette_item.h"

#include "settings.h"

#include "bn_regular_bg_items_suika_bg.h"

namespace suika
{

namespace
{
    constexpr bn::fixed OPTION_START_Y = 0;
    constexpr bn::fixed OPTION_STEP_Y = 22;

    // Admin panel menu options.
    constexpr int ADMIN_OPTION_COUNT = 4;
    constexpr int ADMIN_OPTION_SYSTEM_STATUS = 0;
    constexpr int ADMIN_OPTION_LOGS = 1;
    constexpr int ADMIN_OPTION_DEV_TOOLS = 2;
    constexpr int ADMIN_OPTION_BACK = 3;
    constexpr bn::fixed ADMIN_MENU_START_Y = -33;

    // Developer Tools menu options (only the first is enabled).
    constexpr int DEV_OPTION_COUNT = 4;
    constexpr int DEV_OPTION_CORRUPTED = 0;

    // Read-only info screens (System Status / Logs).
    constexpr bn::fixed INFO_START_Y = -36;
    constexpr bn::fixed INFO_STEP_Y = 12;
    constexpr bn::fixed INFO_LEFT_X = -84;

    // A dimmed copy of the font palette used to draw the disabled "Admin Panel"
    // option in gray. The font glyphs use index 12 for the black outline and
    // index 14 for the fill, so only index 14 is darkened here to a gray.
    constexpr bn::color GRAY_PALETTE_COLORS[16] = {
        bn::color(0, 0, 0),    bn::color(0, 0, 0),    bn::color(0, 0, 0),  bn::color(0, 0, 0),
        bn::color(0, 0, 0),    bn::color(0, 0, 0),    bn::color(0, 0, 0),  bn::color(0, 0, 0),
        bn::color(0, 0, 0),    bn::color(0, 0, 0),    bn::color(0, 0, 0),  bn::color(0, 0, 0),
        bn::color(0, 0, 0),    bn::color(0, 0, 0),    bn::color(14, 14, 14), bn::color(0, 0, 0)
    };

    constexpr bn::sprite_palette_item GRAY_PALETTE_ITEM(
            bn::span<const bn::color>(GRAY_PALETTE_COLORS, 16),
            bn::bpp_mode::BPP_4, bn::compression_type::NONE);

    // A copy of the font palette with the glyph fill (index 14) set to red,
    // used to draw the "Off" value of the Corrupted Fruits option.
    constexpr bn::color RED_PALETTE_COLORS[16] = {
        bn::color(0, 0, 0),    bn::color(0, 0, 0),    bn::color(0, 0, 0),  bn::color(0, 0, 0),
        bn::color(0, 0, 0),    bn::color(0, 0, 0),    bn::color(0, 0, 0),  bn::color(0, 0, 0),
        bn::color(0, 0, 0),    bn::color(0, 0, 0),    bn::color(0, 0, 0),  bn::color(0, 0, 0),
        bn::color(0, 0, 0),    bn::color(0, 0, 0),    bn::color(31, 0, 0), bn::color(0, 0, 0)
    };

    constexpr bn::sprite_palette_item RED_PALETTE_ITEM(
            bn::span<const bn::color>(RED_PALETTE_COLORS, 16),
            bn::bpp_mode::BPP_4, bn::compression_type::NONE);

    // A copy of the font palette with the glyph fill (index 14) set to green,
    // used to draw the "On" value of the Corrupted Fruits option.
    constexpr bn::color GREEN_PALETTE_COLORS[16] = {
        bn::color(0, 0, 0),    bn::color(0, 0, 0),    bn::color(0, 0, 0),  bn::color(0, 0, 0),
        bn::color(0, 0, 0),    bn::color(0, 0, 0),    bn::color(0, 0, 0),  bn::color(0, 0, 0),
        bn::color(0, 0, 0),    bn::color(0, 0, 0),    bn::color(0, 0, 0),  bn::color(0, 0, 0),
        bn::color(0, 0, 0),    bn::color(0, 0, 0),    bn::color(0, 31, 0), bn::color(0, 0, 0)
    };

    constexpr bn::sprite_palette_item GREEN_PALETTE_ITEM(
            bn::span<const bn::color>(GREEN_PALETTE_COLORS, 16),
            bn::bpp_mode::BPP_4, bn::compression_type::NONE);
}

settings_scene::settings_scene(bn::sprite_text_generator& text_generator) :
    _text_generator(text_generator),
    _bg(bn::regular_bg_items::suika_bg.create_bg(0, 0))
{
    if(admin_panel_pending)
    {
        // Returning from the creepy scene right after the unlock combo.
        admin_panel_pending = false;
        _view = view::panel;
    }
    else if(dev_tools_pending)
    {
        // Returning from the creepy scene right after activating Corrupted Fruits.
        dev_tools_pending = false;
        _view = view::dev_tools;
        _admin_selected = ADMIN_OPTION_DEV_TOOLS;
        _dev_selected = DEV_OPTION_CORRUPTED;
    }

    _refresh_options();
}

void settings_scene::_refresh_options()
{
    _refresh_title();

    _option_sprites.clear();
    _text_generator.set_center_alignment();

    switch(_view)
    {

    case view::panel:
        _refresh_admin_menu();
        break;

    case view::system_status:
        _refresh_system_status();
        break;

    case view::logs:
        _refresh_logs();
        break;

    case view::dev_tools:
        _refresh_dev_tools();
        break;

    default:
        _refresh_list();
        break;
    }
}

void settings_scene::_refresh_title()
{
    _title_sprites.clear();
    _text_generator.set_center_alignment();
    _text_generator.generate(0, -50, _view == view::list ? "SETTINGS" : "ADMIN PANEL",
                             _title_sprites);
}

void settings_scene::_refresh_list()
{
    bn::string<40> admin = "Admin Panel";

    if(_selected == 0)
    {
        admin = bn::string<40>("> ") + admin + " <";
    }

    // While locked the option is disabled, so draw it grayed out.
    if(admin_unlocked)
    {
        _text_generator.generate(0, OPTION_START_Y, admin, _option_sprites);
    }
    else
    {
        bn::sprite_palette_item normal_palette = _text_generator.palette_item();
        _text_generator.set_palette_item(GRAY_PALETTE_ITEM);
        _text_generator.generate(0, OPTION_START_Y, admin, _option_sprites);
        _text_generator.set_palette_item(normal_palette);
    }

    _text_generator.generate(0, OPTION_START_Y + OPTION_STEP_Y,
                             _selected == 1 ? "> Back <" : "Back", _option_sprites);
}

void settings_scene::_refresh_admin_menu()
{
    static const char* const ADMIN_OPTIONS[ADMIN_OPTION_COUNT] = {
        "System Status", "Logs", "Developer Tools", "Back"
    };

    bn::fixed y = ADMIN_MENU_START_Y;

    for(int i = 0; i < ADMIN_OPTION_COUNT; ++i)
    {
        bn::string<40> text = ADMIN_OPTIONS[i];

        if(i == _admin_selected)
        {
            text = bn::string<40>("> ") + text + " <";
        }

        _text_generator.generate(0, y, text, _option_sprites);
        y += OPTION_STEP_Y;
    }
}

void settings_scene::_refresh_system_status()
{
    static const char* const LINES[] = {
        "ROM.............OK",
        "SRAM............OK",
        "INPUT...........OK",
        "AUDIO...........OK",
        "",
        "Player 1",
        "Assimilation 100%",
        "Player 2",
        "Assimilation 6%"
    };
    constexpr int LINE_COUNT = 9;

    _text_generator.set_left_alignment();

    bn::fixed y = INFO_START_Y;

    for(int i = 0; i < LINE_COUNT; ++i)
    {
        if(LINES[i][0] != '\0')
        {
            _text_generator.generate(INFO_LEFT_X, y, LINES[i], _option_sprites);
        }

        y += INFO_STEP_Y;
    }
}

void settings_scene::_refresh_logs()
{
    static const char* const LINES[] = {
        "> PLAYER 1 LOGIN OK",
        "> P1 ASSIM 27%",
        "> P1 ASSIM 61%",
        "> P1 ASSIM 100%",
        "> NEW PLAYER JOINED",
        "> P2 ASSIM START",
        "> P2 ASSIM 6%"
    };
    constexpr int LINE_COUNT = 7;

    _text_generator.set_left_alignment();

    bn::fixed y = INFO_START_Y;

    for(int i = 0; i < LINE_COUNT; ++i)
    {
        _text_generator.generate(INFO_LEFT_X, y, LINES[i], _option_sprites);
        y += INFO_STEP_Y;
    }
}

void settings_scene::_refresh_dev_tools()
{
    bn::fixed y = ADMIN_MENU_START_Y;

    for(int i = 0; i < DEV_OPTION_COUNT; ++i)
    {
        if(i == DEV_OPTION_CORRUPTED)
        {
            // Draw "Corrupted Fruits: " and its value as separate segments laid
            // out left-aligned from an X that keeps the whole line centered.
            // While still toggleable the label is normal and the "Off" value is
            // red; once activated the option is locked (label grayed as disabled)
            // but the "On" value stays green.
            const bool selected = (i == _dev_selected);
            const char* prefix = selected ? "> " : "";
            const char* label = "Corrupted Fruits: ";
            const char* value = corrupted_fruits ? "On" : "Off";
            const char* suffix = selected ? " <" : "";

            const int prefix_w = _text_generator.width(prefix);
            const int label_w = _text_generator.width(label);
            const int value_w = _text_generator.width(value);
            const int suffix_w = _text_generator.width(suffix);
            const int total_w = prefix_w + label_w + value_w + suffix_w;

            bn::sprite_palette_item normal_palette = _text_generator.palette_item();
            const bn::sprite_palette_item& label_palette =
                    corrupted_fruits ? GRAY_PALETTE_ITEM : normal_palette;
            const bn::sprite_palette_item& value_palette =
                    corrupted_fruits ? GREEN_PALETTE_ITEM : RED_PALETTE_ITEM;

            _text_generator.set_left_alignment();
            bn::fixed x = bn::fixed(-total_w) / 2;

            _text_generator.set_palette_item(label_palette);
            _text_generator.generate(x, y, prefix, _option_sprites);
            x += prefix_w;
            _text_generator.generate(x, y, label, _option_sprites);
            x += label_w;

            _text_generator.set_palette_item(value_palette);
            _text_generator.generate(x, y, value, _option_sprites);
            x += value_w;

            _text_generator.set_palette_item(label_palette);
            _text_generator.generate(x, y, suffix, _option_sprites);

            _text_generator.set_palette_item(normal_palette);
            _text_generator.set_center_alignment();
        }
        else
        {
            // "?????" placeholders are always disabled and drawn grayed out.
            bn::string<40> text = "?????";

            if(i == _dev_selected)
            {
                text = bn::string<40>("> ") + text + " <";
            }

            bn::sprite_palette_item normal_palette = _text_generator.palette_item();
            _text_generator.set_palette_item(GRAY_PALETTE_ITEM);
            _text_generator.generate(0, y, text, _option_sprites);
            _text_generator.set_palette_item(normal_palette);
        }

        y += OPTION_STEP_Y;
    }
}

int settings_scene::_feed_unlock_combo()
{
    static constexpr bn::keypad::key_type SEQUENCE[] = {
        bn::keypad::key_type::LEFT,  bn::keypad::key_type::RIGHT,
        bn::keypad::key_type::A, bn::keypad::key_type::LEFT,  
        bn::keypad::key_type::A, bn::keypad::key_type::LEFT
    };
    constexpr int SEQUENCE_LEN = 6;

    // Find the single combo-relevant key pressed this frame (if any).
    bn::keypad::key_type pressed_key = bn::keypad::key_type::A;
    int pressed_count = 0;

    for(bn::keypad::key_type key : { bn::keypad::key_type::LEFT, bn::keypad::key_type::RIGHT,
                                     bn::keypad::key_type::B, bn::keypad::key_type::A })
    {
        if(bn::keypad::pressed(key))
        {
            pressed_key = key;
            ++pressed_count;
        }
    }

    if(pressed_count != 1)
    {
        if(pressed_count > 1)
        {
            _combo_progress = 0;   // ambiguous input, restart
        }

        return 0;
    }

    if(pressed_key == SEQUENCE[_combo_progress])
    {
        ++_combo_progress;

        if(_combo_progress == SEQUENCE_LEN)
        {
            _combo_progress = 0;
            return 2;   // completed
        }

        return 1;   // advanced: this key belongs to the combo, consume it
    }

    // Wrong key: restart (this key may be the first key of a new attempt).
    _combo_progress = (pressed_key == SEQUENCE[0]) ? 1 : 0;
    return 0;
}

bn::optional<scene_type> settings_scene::_update_list()
{
    int combo = 0;

    if(! admin_unlocked)
    {
        combo = _feed_unlock_combo();

        if(combo == 2)
        {
            admin_unlocked = true;       // enabled forever from now on
            admin_panel_pending = true;  // open the admin panel after the scare
            settings_save();             // persist the unlock across power cycles
            return scene_type::creepy_admin;
        }
    }

    if(bn::keypad::up_pressed() || bn::keypad::down_pressed())
    {
        _selected ^= 1;   // two options: just toggle the highlight
        _refresh_options();
    }

    if(combo != 1 && bn::keypad::b_pressed())
    {
        return scene_type::menu;
    }

    if(combo != 1 && (bn::keypad::a_pressed() || bn::keypad::start_pressed()))
    {
        if(_selected == 1)
        {
            return scene_type::menu;   // Back
        }

        if(admin_unlocked)
        {
            _view = view::panel;   // open the admin panel
            _admin_selected = 0;
            _refresh_options();
        }
    }

    return bn::nullopt;
}

bn::optional<scene_type> settings_scene::_update_panel()
{
    if(bn::keypad::up_pressed())
    {
        _admin_selected = (_admin_selected + ADMIN_OPTION_COUNT - 1) % ADMIN_OPTION_COUNT;
        _refresh_options();
    }
    else if(bn::keypad::down_pressed())
    {
        _admin_selected = (_admin_selected + 1) % ADMIN_OPTION_COUNT;
        _refresh_options();
    }

    if(bn::keypad::b_pressed())
    {
        _view = view::list;   // back to the settings list
        _refresh_options();
        return bn::nullopt;
    }

    if(bn::keypad::a_pressed() || bn::keypad::start_pressed())
    {
        switch(_admin_selected)
        {

        case ADMIN_OPTION_SYSTEM_STATUS:
            _view = view::system_status;
            _refresh_options();
            break;

        case ADMIN_OPTION_LOGS:
            _view = view::logs;
            _refresh_options();
            break;

        case ADMIN_OPTION_DEV_TOOLS:
            _view = view::dev_tools;
            _dev_selected = 0;
            _refresh_options();
            break;

        case ADMIN_OPTION_BACK:
            _view = view::list;
            _refresh_options();
            break;

        default:
            break;
        }
    }

    return bn::nullopt;
}

bn::optional<scene_type> settings_scene::_update_info_screen()
{
    if(bn::keypad::a_pressed() || bn::keypad::b_pressed() || bn::keypad::start_pressed())
    {
        _view = view::panel;   // return to the admin panel menu
        _refresh_options();
    }

    return bn::nullopt;
}

bn::optional<scene_type> settings_scene::_update_dev_tools()
{
    if(bn::keypad::up_pressed())
    {
        _dev_selected = (_dev_selected + DEV_OPTION_COUNT - 1) % DEV_OPTION_COUNT;
        _refresh_options();
    }
    else if(bn::keypad::down_pressed())
    {
        _dev_selected = (_dev_selected + 1) % DEV_OPTION_COUNT;
        _refresh_options();
    }

    if(bn::keypad::b_pressed())
    {
        _view = view::panel;   // back to the admin panel menu
        _refresh_options();
        return bn::nullopt;
    }

    if(bn::keypad::a_pressed() || bn::keypad::start_pressed())
    {
        if(_dev_selected == DEV_OPTION_CORRUPTED && ! corrupted_fruits)
        {
            // Activating Corrupted Fruits is permanent: lock it On, persist it,
            // and play the creepy scene before returning to the Developer Tools
            // menu (where the option now shows as a disabled, grayed "On").
            corrupted_fruits = true;
            settings_save();
            dev_tools_pending = true;
            return scene_type::creepy_admin;
        }
        // The other options (and the already-activated toggle) are disabled.
    }

    return bn::nullopt;
}

bn::optional<scene_type> settings_scene::update()
{
    switch(_view)
    {

    case view::panel:
        return _update_panel();

    case view::system_status:
    case view::logs:
        return _update_info_screen();

    case view::dev_tools:
        return _update_dev_tools();

    default:
        return _update_list();
    }
}

}
