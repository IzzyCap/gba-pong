#ifndef SUIKA_MENU_SCENE_H
#define SUIKA_MENU_SCENE_H

#include "bn_vector.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"

#include "scene.h"

namespace suika
{

// Title screen: shows the game name, the controls and a blinking prompt.
// Pressing A or START starts a new game.
class menu_scene : public scene
{

public:
    explicit menu_scene(bn::sprite_text_generator& text_generator);

    [[nodiscard]] bn::optional<scene_type> update() final;

private:
    bn::vector<bn::sprite_ptr, 80> _text_sprites;
    bn::vector<bn::sprite_ptr, 16> _prompt_sprites;
    int _blink = 0;
};

}

#endif
