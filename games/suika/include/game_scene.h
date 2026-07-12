#ifndef SUIKA_GAME_SCENE_H
#define SUIKA_GAME_SCENE_H

#include "bn_random.h"
#include "bn_vector.h"
#include "bn_sprite_ptr.h"
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
    bn::random _random;
    fruit_vector _fruits;
    bn::vector<bn::sprite_ptr, 24> _score_sprites;
    bn::vector<bn::sprite_ptr, 40> _msg_sprites;
    bn::vector<bn::sprite_ptr, 6> _next_label;
    int _current_type;
    int _next_type;
    bn::sprite_ptr _current_sprite;
    bn::sprite_ptr _next_sprite;
    bn::fixed _cursor_x = 0;
    int _drop_cooldown = 0;
    int _score = 0;
    int _last_score = -1;
    int _overflow_frames = 0;
    bool _game_over = false;
    bool _score_saved = false;

    void _refresh_score();
};

}

#endif
