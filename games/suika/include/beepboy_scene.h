#ifndef SUIKA_BEEPBOY_SCENE_H
#define SUIKA_BEEPBOY_SCENE_H

#include "bn_random.h"
#include "bn_vector.h"
#include "bn_sprite_ptr.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sprite_palette_item.h"
#include "bn_sprite_text_generator.h"

#include "scene.h"
#include "typewriter_text.h"

namespace suika
{

// Corrupted "talk with Beepboy" event, entered instead of the game while the
// player sits at STORY_CORRUPTED_FRUITS_DONE. It reuses the game's three
// backgrounds (suika_bg, beepboy, suika_game_zone), scrambles the on-screen UI
// text into fast-flickering binary (0/1), and has Beepboy speak in red.
class beepboy_scene : public scene
{

public:
    explicit beepboy_scene(bn::sprite_text_generator& text_generator);

    [[nodiscard]] bn::optional<scene_type> update() final;

private:
    bn::sprite_text_generator& _text_generator;
    bn::sprite_palette_item _normal_palette;  // font palette to restore on exit
    bn::regular_bg_ptr _bg;            // jar-room backdrop (back)
    bn::regular_bg_ptr _beepboy_bg;    // beepboy, between the backdrop and the jar
    bn::random _random;
    bn::vector<bn::sprite_ptr, 32> _glitch_sprites;  // flickering binary UI text
    typewriter_text _dialog;
    int _glitch_timer = 0;

    // Re-scrambles the binary UI text into fresh random 0/1 digits.
    void _refresh_glitch();
};

}

#endif
