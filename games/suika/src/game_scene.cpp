#include "game_scene.h"

#include "bn_keypad.h"
#include "bn_string.h"
#include "bn_algorithm.h"

#include "high_scores.h"

#include "bn_sprite_items_score_panel.h"

namespace suika
{

namespace
{
    // Score panel sits in the left margin, clear of the play area (x >= -58).
    constexpr bn::fixed PANEL_X = -92;
    constexpr bn::fixed PANEL_Y = -58;
    constexpr bn::fixed SCORE_VALUE_Y = -53;  // centred in the panel's body

    // Held-fruit display, below the score panel in the left margin.
    constexpr bn::fixed HOLD_X = -96;
    constexpr bn::fixed HOLD_Y = 34;
    constexpr bn::fixed HOLD_LABEL_Y = 12;
}

game_scene::game_scene(bn::sprite_text_generator& text_generator) :
    _text_generator(text_generator),
    _current_type(_random.get_int(0, SPAWN_MAX_TYPE + 1)),
    _next_type(_random.get_int(0, SPAWN_MAX_TYPE + 1)),
    _current_sprite(create_fruit_sprite(_current_type, 0, SPAWN_Y)),
    _next_sprite(create_fruit_sprite(_next_type, 96, -44)),
    _score_panel(bn::sprite_items::score_panel.create_sprite(PANEL_X, PANEL_Y))
{
    // The text generator emits sprites at bg_priority 3, so the panel must share
    // that layer (then z_order keeps it behind) or it would cover the digits.
    _score_panel.set_bg_priority(3);
    _score_panel.set_z_order(1);  // keep the panel behind the score digits
    _text_generator.set_center_alignment();
    _text_generator.generate(96, -66, "NEXT", _next_label);
    _text_generator.generate(HOLD_X, HOLD_LABEL_Y, "HOLD", _hold_label);
    _refresh_score();
}

void game_scene::_refresh_score()
{
    if(_score != _last_score)
    {
        _score_sprites.clear();
        _text_generator.set_center_alignment();
        _text_generator.generate(PANEL_X, SCORE_VALUE_Y, bn::to_string<16>(_score), _score_sprites);
        _last_score = _score;
    }
}

bn::optional<scene_type> game_scene::update()
{
    if(_game_over)
    {
        if(bn::keypad::start_pressed())
        {
            return scene_type::menu;
        }

        return bn::nullopt;
    }

    bn::fixed r = fruit_radius(_current_type);

    if(bn::keypad::left_held())
    {
        _cursor_x -= CURSOR_SPEED;
    }

    if(bn::keypad::right_held())
    {
        _cursor_x += CURSOR_SPEED;
    }

    _cursor_x = bn::clamp(_cursor_x, LEFT + r, RIGHT - r);

    if(_drop_cooldown > 0)
    {
        --_drop_cooldown;
    }

    // Hold / swap: stash the current fruit for later. With an empty hold slot
    // the current fruit is banked and the next one is pulled up; otherwise the
    // current fruit is swapped with the one already being held.
    if(bn::keypad::b_pressed())
    {
        if(_hold_type < 0)
        {
            _hold_type = _current_type;
            _current_type = _next_type;
            _next_type = _random.get_int(0, SPAWN_MAX_TYPE + 1);
            _next_sprite = create_fruit_sprite(_next_type, 96, -44);
        }
        else
        {
            int swapped = _current_type;
            _current_type = _hold_type;
            _hold_type = swapped;
        }

        _hold_sprite = create_fruit_sprite(_hold_type, HOLD_X, HOLD_Y);
        _current_sprite = create_fruit_sprite(_current_type, _cursor_x, SPAWN_Y);

        r = fruit_radius(_current_type);
        _cursor_x = bn::clamp(_cursor_x, LEFT + r, RIGHT - r);
    }

    if(bn::keypad::a_pressed() && _drop_cooldown == 0 && _fruits.size() < MAX_FRUITS - 1)
    {
        _fruits.push_back({_cursor_x, SPAWN_Y, bn::fixed(0), bn::fixed(0.5),
                           _current_type,
                           create_fruit_sprite(_current_type, _cursor_x, SPAWN_Y)});
        _current_type = _next_type;
        _next_type = _random.get_int(0, SPAWN_MAX_TYPE + 1);
        _next_sprite = create_fruit_sprite(_next_type, 96, -44);
        _current_sprite = create_fruit_sprite(_current_type, _cursor_x, SPAWN_Y);
        _drop_cooldown = DROP_COOLDOWN;

        r = fruit_radius(_current_type);
        _cursor_x = bn::clamp(_cursor_x, LEFT + r, RIGHT - r);
    }

    step_physics(_fruits);

    int guard = 0;

    while(try_merge(_fruits, _score) && guard < 32)
    {
        ++guard;
    }

    for(fruit_t& f : _fruits)
    {
        // Only move the sprite when the fruit shifts by more than half a pixel.
        // Any residual sub-pixel settling stays invisible instead of flickering
        // across a pixel boundary, while real motion updates every frame.
        bn::fixed dx = f.x - f.sprite.x();
        bn::fixed dy = f.y - f.sprite.y();

        if(dx * dx + dy * dy > bn::fixed(0.36))  // moved more than ~0.6 px
        {
            f.sprite.set_position(f.x, f.y);
        }
    }

    _current_sprite.set_position(_cursor_x, SPAWN_Y);
    _current_sprite.set_visible(_drop_cooldown == 0);

    if(is_overflowing(_fruits))
    {
        if(++_overflow_frames > OVERFLOW_LIMIT)
        {
            _current_sprite.set_visible(false);

            if(! _score_saved)
            {
                int rank = submit_high_score(_score);
                _score_saved = true;

                _text_generator.set_center_alignment();
                _text_generator.generate(0, -8, "GAME OVER", _msg_sprites);

                if(rank >= 0)
                {
                    bn::string<32> text = "New rank #" + bn::to_string<32>(rank + 1) + "!";
                    _text_generator.generate(0, 6, text, _msg_sprites);
                }

                _text_generator.generate(0, 20, "Press START", _msg_sprites);
            }

            _game_over = true;
        }
    }
    else
    {
        _overflow_frames = 0;
    }

    _refresh_score();

    return bn::nullopt;
}

}
