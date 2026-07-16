#include "game_scene.h"

#include "bn_keypad.h"
#include "bn_string.h"
#include "bn_algorithm.h"

#include "high_scores.h"

#include "bn_regular_bg_items_suika_bg.h"
#include "bn_regular_bg_items_suika_game_zone.h"
#include "bn_sprite_items_drop_line.h"

namespace suika
{

namespace
{
    // Score display sits in the left margin, clear of the play area (x >= -58).
    constexpr bn::fixed SCORE_X = -92;
    constexpr bn::fixed SCORE_LABEL_Y = -66;  // "SCORE" heading
    constexpr bn::fixed SCORE_VALUE_Y = -53;  // value below the heading

    // Held-fruit display, below the score panel in the left margin.
    constexpr bn::fixed HOLD_X = -96;
    constexpr bn::fixed HOLD_Y = 34;
    constexpr bn::fixed HOLD_LABEL_Y = 12;

    // Drop guide line: four stacked 8x32 segments spanning the jar interior.
    constexpr bn::fixed DROP_LINE_Y[] = {-48, -16, 16, 48};
    constexpr int DROP_LINE_FRAMES = 8;     // frames in drop_line.bmp (8x256)
    constexpr int DROP_LINE_ANIM_SPEED = 4; // game frames between animation steps
}

game_scene::game_scene(bn::sprite_text_generator& text_generator) :
    _text_generator(text_generator),
    _bg(bn::regular_bg_items::suika_bg.create_bg(0, 0)),
    _game_zone_bg(bn::regular_bg_items::suika_game_zone.create_bg(0, 0)),
    _current_type(_random.get_int(0, SPAWN_MAX_TYPE + 1)),
    _next_type(_random.get_int(0, SPAWN_MAX_TYPE + 1)),
    _current_sprite(create_fruit_sprite(_current_type, 0, SPAWN_Y)),
    _next_sprite(create_fruit_sprite(_next_type, 96, -44))
{
    // Two background layers: the green jar-room (suika_bg) sits at the back and
    // the jar (suika_game_zone) draws over it. The jar image is transparent
    // (palette color 0) around its border, so the leaves show through. Both share
    // sprites' backmost priority (3) so every sprite -- fruits, score text,
    // aim guide and text -- still renders in front of them.
    _bg.set_priority(3);
    _game_zone_bg.set_priority(3);
    _bg.set_z_order(1);            // higher z order = drawn first (further back)
    _game_zone_bg.set_z_order(0);  // lower z order = drawn over the backdrop

    // Aim guide: stacked segments that follow the cursor. They share the fruits'
    // background priority (3) but use a higher z order, so the fruits (z order 0)
    // are drawn on top -- the guide sits behind the fruits yet over the jar.
    for(bn::fixed y : DROP_LINE_Y)
    {
        bn::sprite_ptr segment = bn::sprite_items::drop_line.create_sprite(_cursor_x, y);
        segment.set_bg_priority(3);
        segment.set_z_order(1);
        _drop_line.push_back(segment);
    }

    // Pre-build the guide's animation frames once (the dash pattern shifted down
    // one pixel per frame); cycling them makes the line appear to flow downward.
    for(int i = 0; i < DROP_LINE_FRAMES; ++i)
    {
        _line_tiles.push_back(bn::sprite_items::drop_line.tiles_item().create_tiles(i));
    }

    _text_generator.set_center_alignment();
    _text_generator.generate(96, -66, "NEXT", _next_label);
    _text_generator.generate(HOLD_X, HOLD_LABEL_Y, "HOLD", _hold_label);
    _text_generator.generate(SCORE_X, SCORE_LABEL_Y, "SCORE", _score_label);
    _refresh_score();
}

void game_scene::_refresh_score()
{
    if(_score != _last_score)
    {
        _score_sprites.clear();
        _text_generator.set_center_alignment();
        _text_generator.generate(SCORE_X, SCORE_VALUE_Y, bn::to_string<16>(_score), _score_sprites);
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

    // Guide line follows the cursor; shown only while a fruit is ready to drop.
    // Cycling the pre-built frames flows the dashes downward in a seamless loop.
    _line_anim = (_line_anim + 1) % (DROP_LINE_FRAMES * DROP_LINE_ANIM_SPEED);
    const bn::sprite_tiles_ptr& line_frame = _line_tiles[_line_anim / DROP_LINE_ANIM_SPEED];

    for(bn::sprite_ptr& segment : _drop_line)
    {
        segment.set_x(_cursor_x);
        segment.set_tiles(line_frame);
        segment.set_visible(_drop_cooldown == 0);
    }

    if(is_overflowing(_fruits))
    {
        if(++_overflow_frames > OVERFLOW_LIMIT)
        {
            _current_sprite.set_visible(false);

            for(bn::sprite_ptr& segment : _drop_line)
            {
                segment.set_visible(false);
            }

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
