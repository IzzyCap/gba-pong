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
    // Hidden reveal: once the corrupted fruit_8 story beat is done, beepboy lurks
    // between the jar-room backdrop and the jar.
    bn::optional<bn::regular_bg_ptr> _beepboy_bg;
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
    int _merge_cooldown = 0;   // short pause between merges so each stays visible
    int _score = 0;
    int _last_score = -1;
    int _overflow_frames = 0;
    bool _game_over = false;
    bool _score_saved = false;

    // Combo: chaining merges. Each merge within COMBO_WINDOW frames of the
    // previous one extends the combo; letting that window lapse ends it. Each
    // merge also spawns a "<points> x<multiplier>" popup (_combo_sprites) centred
    // on the merge that drifts up and fades out over _combo_life frames.
    int _combo_count = 0;      // 0 = no active combo; otherwise the x1, x2, ... multiplier
    int _combo_timer = 0;      // frames left in the current combo (chain) window
    int _combo_life = 0;       // frames left in the current popup's fade/rise
    bn::fixed _combo_x = 0;    // where the latest merge happened (anchor)
    bn::fixed _combo_y = 0;
    bn::vector<bn::sprite_ptr, 4> _combo_sprites;  // the "<points> x<multiplier>" popup

    void _refresh_score();

    // Makes type's corrupted frames the ones resident in VRAM, freeing whatever
    // type was loaded before. No-op when that type is already loaded.
    void _ensure_corrupt_tiles(int type);

    // Starts or extends the combo at a merge that just scored points at (x, y).
    void _register_combo_merge(int points, bn::fixed x, bn::fixed y);

    // Rebuilds the "<points> x<multiplier>" combo readout at the current anchor.
    void _refresh_combo_text(int points);

    // Advances the active combo one frame: fades the readout out and ends the
    // combo when the window runs out.
    void _update_combo();

    // Clears the combo readout once the window has lapsed.
    void _end_combo();
};

}

#endif
