#include "menu_scene.h"

#include "bn_keypad.h"

namespace suika
{

menu_scene::menu_scene(bn::sprite_text_generator& text_generator)
{
    text_generator.set_center_alignment();
    text_generator.generate(0, -44, "SUIKA", _text_sprites);
    text_generator.generate(0, -28, "- fruit merge -", _text_sprites);
    text_generator.generate(0, 4, "move:  L / R", _text_sprites);
    text_generator.generate(0, 16, "drop:  A", _text_sprites);
    text_generator.generate(0, 44, "press start", _prompt_sprites);
}

bn::optional<scene_type> menu_scene::update()
{
    // Blink the prompt so the menu feels alive.
    ++_blink;

    for(bn::sprite_ptr& s : _prompt_sprites)
    {
        s.set_visible((_blink / 30) % 2 == 0);
    }

    if(bn::keypad::a_pressed() || bn::keypad::start_pressed())
    {
        return scene_type::game;
    }

    return bn::nullopt;
}

}
