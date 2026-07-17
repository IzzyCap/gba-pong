#ifndef SUIKA_CREEPY_SCENE_H
#define SUIKA_CREEPY_SCENE_H

#include "bn_regular_bg_ptr.h"
#include "bn_sprite_text_generator.h"

#include "scene.h"

namespace suika
{

// Creepy end sequence: shows the two final hypnosis frames (hypno_5 then
// hypno_6) once each, then switches to the scene given by the constructor's
// next argument.
class creepy_scene : public scene
{

public:
    creepy_scene(bn::sprite_text_generator& text_generator, scene_type next);

    [[nodiscard]] bn::optional<scene_type> update() final;

private:
    bn::regular_bg_ptr _bg;
    int _timer = 0;
    scene_type _next;  // scene to switch to once the sequence finishes
};

}

#endif
