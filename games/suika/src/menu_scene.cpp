#include "menu_scene.h"

#include "bn_keypad.h"
#include "bn_string.h"

#include "high_scores.h"

#include "bn_regular_bg_items_suika_bg.h"

namespace suika
{

menu_scene::menu_scene(bn::sprite_text_generator& text_generator) :
    _bg(bn::regular_bg_items::suika_bg.create_bg(0, 0))
{
    text_generator.set_center_alignment();
    text_generator.generate(0, -60, "SUIKA", _text_sprites);
    text_generator.generate(0, -44, "- fruit merge -", _text_sprites);

    text_generator.generate(0, -24, "RANKING", _text_sprites);

    high_score_table scores = load_high_scores();

    for(int index = 0; index < HIGH_SCORE_COUNT; ++index)
    {
        bn::string<16> line = bn::to_string<8>(index + 1) + ".  " +
                              bn::to_string<8>(scores[index]);
        text_generator.generate(0, -12 + index * 12, line, _text_sprites);
    }

    text_generator.generate(0, 60, "press start", _prompt_sprites);
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
