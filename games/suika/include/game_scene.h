#ifndef SUIKA_GAME_SCENE_H
#define SUIKA_GAME_SCENE_H

#include "bn_random.h"
#include "bn_vector.h"
#include "bn_sprite_ptr.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sprite_tiles_ptr.h"
#include "bn_sprite_text_generator.h"

#include "scene.h"
#include "fruit.h"

namespace suika
{

// Gameplay: drop fruits, merge matching pairs, and avoid overflowing the jar.
// On game over it waits for START and then returns to the menu.
class game_scene : public scene
{

public:
    explicit game_scene(bn::sprite_text_generator& text_generator);

    [[nodiscard]] bn::optional<scene_type> update() final;

private:
    bn::sprite_text_generator& _text_generator;
    bn::regular_bg_ptr _bg;            // jar-room backdrop (green leaves), drawn behind
    bn::regular_bg_ptr _game_zone_bg;  // jar overlay, drawn in front of _bg
    bn::random _random;
    fruit_vector _fruits;
    bn::vector<bn::sprite_ptr, 24> _score_sprites;
    bn::vector<bn::sprite_ptr, 40> _msg_sprites;
    bn::vector<bn::sprite_ptr, 6> _next_label;
    bn::vector<bn::sprite_ptr, 6> _hold_label;
    bn::vector<bn::sprite_ptr, 6> _score_label;
    bn::vector<bn::sprite_ptr, 48> _ranking_sprites;  // persisted top-3, bottom-right
    int _current_type;
    int _next_type;
    int _hold_type = -1;   // -1 = nothing held yet
    bool _current_corrupt = false;  // is the fruit in this slot the corrupted one?
    bool _next_corrupt = false;
    bool _hold_corrupt = false;
    bn::sprite_ptr _current_sprite;
    bn::sprite_ptr _next_sprite;
    bn::optional<bn::sprite_ptr> _hold_sprite;
    bn::vector<bn::sprite_ptr, 4> _drop_line;  // aim guide under the current fruit
    bn::vector<bn::sprite_tiles_ptr, 8> _line_tiles;  // pre-built flow-anim frames
    int _line_anim = 0;
    // Only one corrupted fruit ever exists at a time and its type only climbs as
    // it merges, so we keep just the current corrupted type's frames resident and
    // swap them for the next type on demand. This caps corrupted sprite VRAM at a
    // single type's frames instead of pre-building all of them at once.
    bn::vector<bn::sprite_tiles_ptr, CORRUPT_FRAMES> _corrupt_tiles;
    int _loaded_corrupt_type = -1;  // type whose frames are in _corrupt_tiles, or -1
    bool _has_corrupt = false;  // any corrupted fruit possible this run?
    int _corrupt_frame = 0;     // current animation frame, shared by all corrupted fruits
    int _corrupt_timer = 0;     // frames left before the next frame change
    bn::fixed _cursor_x = 0;
    int _drop_cooldown = 0;
    int _score = 0;
    int _last_score = -1;
    int _overflow_frames = 0;
    bool _game_over = false;
    bool _score_saved = false;

    void _refresh_score();

    // Makes type's corrupted frames the ones resident in VRAM, freeing whatever
    // type was loaded before. No-op when that type is already loaded.
    void _ensure_corrupt_tiles(int type);
};

}

#endif
