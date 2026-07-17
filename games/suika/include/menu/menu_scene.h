#ifndef SUIKA_MENU_SCENE_H
#define SUIKA_MENU_SCENE_H

#include "bn_vector.h"
#include "bn_sprite_ptr.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sprite_text_generator.h"

#include "scene.h"

namespace suika
{

// Title screen with a small menu: "Start" launches a game and "Settings" opens
// the settings screen. Move with UP/DOWN and confirm with A or START.
class menu_scene : public scene
{

public:
    explicit menu_scene(bn::sprite_text_generator& text_generator);

    [[nodiscard]] bn::optional<scene_type> update() final;

private:
    bn::sprite_text_generator& _text_generator;
    bn::regular_bg_ptr _bg;
    bn::vector<bn::sprite_ptr, 32> _title_sprites;
    bn::vector<bn::sprite_ptr, 32> _option_sprites;
    int _selected = 0;   // 0 = Start, 1 = Settings

    void _refresh_options();
};

}

#endif
