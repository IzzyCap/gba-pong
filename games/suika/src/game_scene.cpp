#include "game_scene.h"

#include "bn_keypad.h"
#include "bn_string.h"
#include "bn_algorithm.h"

#include "high_scores.h"

namespace suika
{

game_scene::game_scene(bn::sprite_text_generator& text_generator) :
    _text_generator(text_generator),
    _current_type(_random.get_int(0, SPAWN_MAX_TYPE + 1)),
    _next_type(_random.get_int(0, SPAWN_MAX_TYPE + 1)),
    _current_sprite(create_fruit_sprite(_current_type, 0, SPAWN_Y)),
    _next_sprite(create_fruit_sprite(_next_type, 96, -44))
{
    _text_generator.set_center_alignment();
    _text_generator.generate(96, -66, "NEXT", _next_label);
    _refresh_score();
}

void game_scene::_refresh_score()
{
    if(_score != _last_score)
    {
        _score_sprites.clear();
        _text_generator.set_left_alignment();
        bn::string<32> text = "Score: " + bn::to_string<32>(_score);
        _text_generator.generate(-116, -76, text, _score_sprites);
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

    if(bn::keypad::a_pressed() && _drop_cooldown == 0 && _fruits.size() < MAX_FRUITS - 1)
    {
        _fruits.push_back({_cursor_x, SPAWN_Y, bn::fixed(0), bn::fixed(0.5),
                           _cursor_x, SPAWN_Y, _current_type, 0, false,
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
    bool merged = false;

    while(try_merge(_fruits, _score) && guard < 32)
    {
        merged = true;
        ++guard;
    }

    // A merge removes fruits, so anything resting above must fall again.
    if(merged)
    {
        for(fruit_t& f : _fruits)
        {
            f.asleep = false;
            f.settle_frames = 0;
            f.anchor_x = f.x;
            f.anchor_y = f.y;
        }
    }

    for(fruit_t& f : _fruits)
    {
        f.sprite.set_position(f.x, f.y);
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
