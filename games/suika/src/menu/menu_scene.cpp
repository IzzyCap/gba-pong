#include "menu_scene.h"

#include "bn_keypad.h"

#include "bn_regular_bg_items_suika_bg.h"

namespace suika
{

namespace
{
    constexpr bn::fixed OPTION_START_Y = 6;
    constexpr bn::fixed OPTION_STEP_Y = 20;
}

menu_scene::menu_scene(bn::sprite_text_generator& text_generator) :
    _text_generator(text_generator),
    _bg(bn::regular_bg_items::suika_bg.create_bg(0, 0))
{
    _text_generator.set_center_alignment();
    _text_generator.generate(0, -60, "SUIKA", _title_sprites);
    _text_generator.generate(0, -44, "- fruit merge -", _title_sprites);
    _refresh_options();
}

void menu_scene::_refresh_options()
{
    _option_sprites.clear();
    _text_generator.set_center_alignment();
    _text_generator.generate(0, OPTION_START_Y,
                             _selected == 0 ? "> Start <" : "Start", _option_sprites);
    _text_generator.generate(0, OPTION_START_Y + OPTION_STEP_Y,
                             _selected == 1 ? "> Settings <" : "Settings", _option_sprites);
}

bn::optional<scene_type> menu_scene::update()
{
    if(bn::keypad::up_pressed() || bn::keypad::down_pressed())
    {
        _selected ^= 1;   // two options: just toggle the highlight
        _refresh_options();
    }

    if(bn::keypad::a_pressed() || bn::keypad::start_pressed())
    {
        return _selected == 0 ? scene_type::game : scene_type::settings;
    }

    return bn::nullopt;
}

}
