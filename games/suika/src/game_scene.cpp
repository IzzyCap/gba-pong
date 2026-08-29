#include "game_scene.h"

#include "bn_keypad.h"
#include "bn_string.h"
#include "bn_window.h"
#include "bn_blending.h"
#include "bn_algorithm.h"

#include "high_scores.h"
#include "settings.h"
#include "story.h"

#include "bn_regular_bg_items_suika_bg.h"
#include "bn_regular_bg_items_suika_game_zone.h"
#include "bn_regular_bg_items_beepboy.h"
#include "bn_regular_bg_items_number_bg.h"
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

    // A super-short pause between merges so that when several are pending at once
    // (chain reactions), the player sees them resolve one at a time instead of
    // all popping on the same frame.
    constexpr int MERGE_COOLDOWN = 12;

    // Drop guide line: four stacked 8x32 segments spanning the jar interior.
    constexpr bn::fixed DROP_LINE_Y[] = {-48, -16, 16, 48};
    constexpr int DROP_LINE_FRAMES = 8;     // frames in drop_line.bmp (8x256)
    constexpr int DROP_LINE_ANIM_SPEED = 4; // game frames between animation steps

    // Corrupted fruit animation timing. CORRUPT_FRAMES lives in fruit.h because the
    // tile-building shares it. Corruption spreads as corrupted fruits merge; each
    // corrupted fruit's first frame dwells 3-5 s, then the remaining frames play at
    // the normal per-frame speed and the cycle repeats.
    constexpr int CORRUPT_ANIM_SPEED = 8;   // game frames per step for frames 1..3
    constexpr int CORRUPT_HOLD_MIN = 180;   // 3 s at 60 fps: frame-0 dwell (min)
    constexpr int CORRUPT_HOLD_MAX = 300;   // 5 s at 60 fps: frame-0 dwell (max)

    // Persisted top-3 ranking, tucked into the bottom-right corner (right-aligned).
    constexpr bn::fixed RANK_X = 116;
    constexpr bn::fixed RANK_LABEL_Y = 36;
    constexpr bn::fixed RANK_FIRST_Y = 49;
    constexpr bn::fixed RANK_LINE_H = 12;
    constexpr int RANK_COUNT = 3;

    // Combo: a merge landed within COMBO_WINDOW frames of the previous one
    // extends the combo; letting the window lapse ends it (the next merge starts
    // back at x1). This is the invisible "cooldown" the player races against.
    constexpr int COMBO_WINDOW = 60;             // ~0.75 s at 60 fps to chain again

    // Combo readout: a popup centred on the merge that rises slowly while it fades
    // out, independent of (and quicker than) the chain window.
    constexpr int COMBO_POPUP_LIFE = 30;         // ~0.5 s to fade out
    constexpr bn::fixed COMBO_RISE = 12;         // pixels the popup drifts up over its life
    constexpr int COMBO_UI_PRIORITY = 1;         // draw the readout in front of fruits (pr 3)

    // Bugged Fruits developer tool.
    constexpr int BUGGED_SPAWN_EVERY = 8;        // one bugged fruit appears every 8 throws
    constexpr int BUGGED_FRUIT_TYPE = 3;         // it spawns at fruit_3's size
    constexpr int BUGGED_BG_PRIORITY = 2;        // number bg draws in front of the jar (pr 3)
    constexpr bn::fixed BUGGED_BG_SCROLL = 1;    // vertical scroll per frame: streams the digits

    // Scoring 1000 points with Bouncing Fruits active unlocks the Bugged Fruits
    // developer tool (revealed in the settings Developer Tools menu afterwards).
    constexpr int BUGGED_UNLOCK_SCORE = 1000;
}

game_scene::game_scene(bn::sprite_text_generator& text_generator) :
    _text_generator(text_generator),
    _bg(bn::regular_bg_items::suika_bg.create_bg(0, 0)),
    _game_zone_bg(bn::regular_bg_items::suika_game_zone.create_bg(0, 0)),
    _current_type(corrupted_fruits ? 0 : _random.get_int(0, SPAWN_MAX_TYPE + 1)),
    _next_type(_random.get_int(0, SPAWN_MAX_TYPE + 1)),
    _current_corrupt(corrupted_fruits),
    _current_sprite(create_fruit_sprite(_current_type, 0, SPAWN_Y, _current_corrupt)),
    _next_sprite(create_fruit_sprite(_next_type, 96, -44))
{
    // Two background layers: the green jar-room (suika_bg) sits at the back and
    // the jar (suika_game_zone) draws over it. The jar image is transparent
    // (palette color 0) around its border, so the leaves show through. Both share
    // sprites' backmost priority (3) so every sprite -- fruits, score text,
    // aim guide and text -- still renders in front of them.
    _bg.set_priority(3);
    _game_zone_bg.set_priority(3);
    _game_zone_bg.set_z_order(0);  // lower z order = drawn over the backdrop

    // Once the "corrupted fruit_8" story beat is reached (persisted in SRAM),
    // beepboy hides between the jar-room backdrop (suika_bg) and the jar
    // (suika_game_zone). Pushing suika_bg one z order further back leaves room
    // for beepboy in the middle. Both share the sprites' backmost priority (3),
    // so every sprite still renders in front.
    if(story_progress >= STORY_CORRUPTED_FRUITS_DONE)
    {
        _beepboy_bg = bn::regular_bg_items::beepboy.create_bg(0, 0);
        _beepboy_bg->set_priority(3);
        _beepboy_bg->set_z_order(1);  // between suika_bg (back) and the jar (front)
        _bg.set_z_order(2);           // furthest back, behind beepboy
    }
    else
    {
        _bg.set_z_order(1);  // higher z order = drawn first (further back)
    }

    // Bugged Fruits: bring up the animated red 0/1 background and mask it to the
    // sprite window, so it is only visible through the silhouettes of the bugged
    // fruits (whose own sprites are invisible window masks). It draws in front of
    // the jar so a bugged fruit reads as a fruit-shaped patch of streaming digits.
    if(bugged_fruits)
    {
        _number_bg = bn::regular_bg_items::number_bg.create_bg(0, 0);
        _number_bg->set_priority(BUGGED_BG_PRIORITY);

        bn::window::internal().set_show_bg(*_number_bg, false);
        bn::window::external().set_show_bg(*_number_bg, false);
        bn::window::outside().set_show_bg(*_number_bg, false);
        bn::window::sprites().set_show_bg(*_number_bg, true);
    }

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

    // The first dropped fruit is corrupted (forced to level 0 above) and corruption
    // then rides the single merged fruit up the chain. Only one corrupted fruit
    // ever exists, so load just its current type's frames now and swap the tiles
    // for the next type on demand as it merges (see _ensure_corrupt_tiles).
    if(corrupted_fruits)
    {
        _ensure_corrupt_tiles(_current_type);

        _has_corrupt = true;

        // Frame 0 lingers a random 3-5 s before the glitchy frames play.
        _corrupt_timer = _random.get_int(CORRUPT_HOLD_MIN, CORRUPT_HOLD_MAX + 1);
    }

    _text_generator.set_center_alignment();
    _text_generator.generate(96, -66, "NEXT", _next_label);
    _text_generator.generate(HOLD_X, HOLD_LABEL_Y, "HOLD", _hold_label);
    _text_generator.generate(SCORE_X, SCORE_LABEL_Y, "SCORE", _score_label);
    _refresh_score();

    // Persisted top-3 ranking in the bottom-right corner.
    high_score_table scores = load_high_scores();
    _text_generator.set_right_alignment();
    _text_generator.generate(RANK_X, RANK_LABEL_Y, "TOP 3", _ranking_sprites);

    for(int i = 0; i < RANK_COUNT; ++i)
    {
        bn::string<16> line = bn::to_string<8>(i + 1) + ". " + bn::to_string<8>(scores[i]);
        _text_generator.generate(RANK_X, RANK_FIRST_Y + i * RANK_LINE_H, line, _ranking_sprites);
    }

    _text_generator.set_center_alignment();
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

void game_scene::_ensure_corrupt_tiles(int type)
{
    if(type == _loaded_corrupt_type)
    {
        return;
    }

    // Release the previous type's frames before building the new ones so at most
    // one corrupted type occupies sprite VRAM at any moment.
    _corrupt_tiles.clear();
    create_corrupted_tiles(type, _corrupt_tiles);

    // Remember the type even when it has no art (empty vector) so we don't try to
    // rebuild it every frame; animate_corrupt simply skips empty tile sets.
    _loaded_corrupt_type = type;
}

void game_scene::_register_combo_merge(int points, bn::fixed x, bn::fixed y)
{
    // A merge landing while the previous window is still open extends the combo;
    // otherwise it starts a fresh one at x1.
    _combo_count = (_combo_timer > 0) ? _combo_count + 1 : 1;
    _combo_timer = COMBO_WINDOW;
    _combo_x = x;
    _combo_y = y;

    // Apply the combo multiplier: this merge is worth its base points times the
    // current multiplier. try_merge already added the base points, so top up the
    // score with the extra (multiplier - 1) copies here.
    _score += points * (_combo_count - 1);

    // The popup only shows once a real combo forms: the second merge onward
    // (x1 is a lone merge, so it stays silent).
    if(_combo_count >= 2)
    {
        _combo_life = COMBO_POPUP_LIFE;
        _refresh_combo_text(points);
    }
}

void game_scene::_refresh_combo_text(int points)
{
    _combo_sprites.clear();

    // "<points> x<multiplier>", e.g. "20 x5": the points this merge scored and
    // the current combo multiplier.
    bn::string<16> text = bn::to_string<8>(points) + " x" + bn::to_string<6>(_combo_count);

    // Centred on the merge, then blended so the shared transparency weight can
    // fade it out little by little as it rises.
    _text_generator.set_center_alignment();
    _text_generator.generate(_combo_x, _combo_y, text, _combo_sprites);

    for(bn::sprite_ptr& sprite : _combo_sprites)
    {
        sprite.set_bg_priority(COMBO_UI_PRIORITY);
        sprite.set_blending_enabled(true);
    }

    // A fresh popup starts fully opaque again.
    bn::blending::set_transparency_alpha(1);
}

void game_scene::_update_combo()
{
    // Chain window: once it lapses, the next merge starts a fresh x1 combo.
    if(_combo_timer > 0)
    {
        --_combo_timer;
    }

    // Popup: drifts slowly upward while fading out, then clears itself. It is
    // quicker than the chain window, so it vanishes well before the combo does.
    if(_combo_life > 0)
    {
        --_combo_life;

        bn::fixed t = bn::fixed(_combo_life) / COMBO_POPUP_LIFE;  // 1 -> 0
        bn::blending::set_transparency_alpha(t);                  // fade out

        bn::fixed y = _combo_y - COMBO_RISE * (1 - t);           // slow drift up

        for(bn::sprite_ptr& sprite : _combo_sprites)
        {
            sprite.set_y(y);
        }

        if(_combo_life == 0)
        {
            _combo_sprites.clear();
            bn::blending::set_transparency_alpha(1);
        }
    }
}

void game_scene::_end_combo()
{
    _combo_count = 0;
    _combo_timer = 0;
    _combo_life = 0;
    _combo_sprites.clear();

    // Release the shared transparency weight so other blended sprites (none today,
    // but future-proofed) render at full opacity.
    bn::blending::set_transparency_alpha(1);
}

bn::optional<scene_type> game_scene::update()
{
    if(_game_over)
    {
        if(bn::keypad::start_pressed())
        {
            return scene_type::creepy_game_over;
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
            _hold_corrupt = _current_corrupt;
            _current_type = _next_type;
            _current_corrupt = _next_corrupt;
            _next_type = _random.get_int(0, SPAWN_MAX_TYPE + 1);
            _next_corrupt = false;
            _next_sprite = create_fruit_sprite(_next_type, 96, -44);
        }
        else
        {
            int swapped = _current_type;
            _current_type = _hold_type;
            _hold_type = swapped;

            bool swapped_corrupt = _current_corrupt;
            _current_corrupt = _hold_corrupt;
            _hold_corrupt = swapped_corrupt;
        }

        _hold_sprite = create_fruit_sprite(_hold_type, HOLD_X, HOLD_Y, _hold_corrupt);
        _current_sprite = create_fruit_sprite(_current_type, _cursor_x, SPAWN_Y, _current_corrupt);

        r = fruit_radius(_current_type);
        _cursor_x = bn::clamp(_cursor_x, LEFT + r, RIGHT - r);
    }

    if(bn::keypad::a_pressed() && _drop_cooldown == 0 && _fruits.size() < MAX_FRUITS - 1)
    {
        _fruits.push_back({_cursor_x, SPAWN_Y, bn::fixed(0), bn::fixed(0.5),
                           _current_type,
                           create_fruit_sprite(_current_type, _cursor_x, SPAWN_Y, _current_corrupt),
                           _current_corrupt});
        _current_type = _next_type;
        _current_corrupt = _next_corrupt;
        _next_type = _random.get_int(0, SPAWN_MAX_TYPE + 1);
        _next_corrupt = false;
        _next_sprite = create_fruit_sprite(_next_type, 96, -44);
        _current_sprite = create_fruit_sprite(_current_type, _cursor_x, SPAWN_Y, _current_corrupt);
        _drop_cooldown = DROP_COOLDOWN;

        r = fruit_radius(_current_type);
        _cursor_x = bn::clamp(_cursor_x, LEFT + r, RIGHT - r);

        // Bugged Fruits: every 8th throw a bugged fruit (fruit_3 size) drops in at
        // a random column. It carries its own falling velocity like any drop.
        if(bugged_fruits && (++_throw_count % BUGGED_SPAWN_EVERY) == 0 &&
           _fruits.size() < MAX_FRUITS)
        {
            bn::fixed br = fruit_radius(BUGGED_FRUIT_TYPE);
            bn::fixed bx = _random.get_int((LEFT + br).integer(), (RIGHT - br).integer() + 1);
            _fruits.push_back({bx, SPAWN_Y, bn::fixed(0), bn::fixed(0.5),
                               BUGGED_FRUIT_TYPE,
                               create_fruit_sprite(BUGGED_FRUIT_TYPE, bx, SPAWN_Y, false, true),
                               false, true});
        }
    }

    step_physics(_fruits);

    if(_merge_cooldown > 0)
    {
        --_merge_cooldown;
    }

    bool corrupt_story_merge = false;
    bn::fixed merge_x = 0;
    bn::fixed merge_y = 0;
    int score_before = _score;

    // Resolve at most one merge per short cooldown so chained or simultaneous
    // merges play out one at a time and stay visible, instead of all popping on
    // the same frame.
    if(_merge_cooldown == 0 && try_merge(_fruits, _score, corrupt_story_merge, merge_x, merge_y))
    {
        _merge_cooldown = MERGE_COOLDOWN;

        // Bugged-fruit merges score nothing (they shrink instead of growing), so
        // only start a combo for a real, point-scoring merge.
        if(_score > score_before)
        {
            _register_combo_merge(_score - score_before, merge_x, merge_y);
        }
    }

    _update_combo();

    // Scoring 1000 points while Bouncing Fruits is active is the story beat that
    // unlocks the Bugged Fruits developer tool. Advance the story once so the new
    // option reveals itself in the Developer Tools menu from now on.
    if(bouncing_fruits && _score >= BUGGED_UNLOCK_SCORE &&
       story_progress == STORY_BOUNCING_FRUITS)
    {
        story_set_progress(STORY_BOUNCING_FRUITS_DONE);
    }

    // First time a corrupted fruit merges with a matching normal fruit (type
    // CORRUPT_STORY_MERGE_TYPE) during the Corrupted Fruits story beat: play the
    // creepy sequence and advance the story so this only ever fires once.
    if(corrupt_story_merge && story_progress == STORY_CORRUPTED_FRUITS)
    {
        story_set_progress(STORY_CORRUPTED_FRUITS_DONE);
        return scene_type::creepy_corrupted;
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

    // Stream the bugged-fruit number background downward so the digits keep
    // changing. It wraps on the 256x256 map, so a plain scroll loops seamlessly.
    if(_number_bg)
    {
        _number_bg->set_y(_number_bg->y() + BUGGED_BG_SCROLL);
    }

    // Corrupted fruits animate on one shared clock: frame 0 dwells for a random
    // 3-5 s, then the remaining frames play at the normal per-frame speed,
    // looping. Each sprite is re-tiled from its own type's frames, so corruption
    // keeps its glitchy look as it merges up the chain (types without corrupted
    // art yet simply show the normal design while still carrying the corrupted
    // flag; single-frame types like corrupted_fruit_1 just stay static).
    if(_has_corrupt)
    {
        // Exactly one corrupted fruit exists at a time (it rides the merged fruit
        // up the chain), so find its current type and make sure that type's frames
        // are the ones resident in VRAM before animating. This swaps the tiles in
        // the moment a merge bumps the corrupted fruit to a bigger type.
        int corrupt_type = -1;

        if(_current_corrupt)
        {
            corrupt_type = _current_type;
        }
        else if(_hold_corrupt)
        {
            corrupt_type = _hold_type;
        }
        else
        {
            for(const fruit_t& f : _fruits)
            {
                if(f.corrupted)
                {
                    corrupt_type = f.type;
                    break;
                }
            }
        }

        if(corrupt_type >= 0)
        {
            _ensure_corrupt_tiles(corrupt_type);
        }

        if(--_corrupt_timer <= 0)
        {
            _corrupt_frame = (_corrupt_frame + 1) % CORRUPT_FRAMES;
            _corrupt_timer = (_corrupt_frame == 0)
                    ? _random.get_int(CORRUPT_HOLD_MIN, CORRUPT_HOLD_MAX + 1)
                    : CORRUPT_ANIM_SPEED;
        }

        auto animate_corrupt = [this](bn::sprite_ptr& sprite, int type, bool corrupt)
        {
            // Only the single loaded corrupted type has resident frames; clamp to
            // its own frame count so single-frame corrupted fruits (e.g.
            // corrupted_fruit_1) stay on frame 0 instead of indexing past them.
            if(corrupt && type == _loaded_corrupt_type && ! _corrupt_tiles.empty())
            {
                int frame = _corrupt_frame % _corrupt_tiles.size();
                sprite.set_tiles(_corrupt_tiles[frame]);
            }
        };

        for(fruit_t& f : _fruits)
        {
            animate_corrupt(f.sprite, f.type, f.corrupted);
        }

        animate_corrupt(_current_sprite, _current_type, _current_corrupt);
        animate_corrupt(_next_sprite, _next_type, _next_corrupt);

        if(_hold_sprite)
        {
            animate_corrupt(*_hold_sprite, _hold_type, _hold_corrupt);
        }
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

            _end_combo();

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
