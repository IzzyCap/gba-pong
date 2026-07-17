#ifndef SUIKA_SCENE_H
#define SUIKA_SCENE_H

#include "bn_optional.h"

namespace suika
{

enum class scene_type
{
    menu,
    settings,
    creepy_game_over,
    creepy_admin,
    game
};

// Base class for every game scene. A scene advances one frame at a time and
// tells the main loop which scene to switch to next (or to stay put).
class scene
{

public:
    virtual ~scene() = default;

    // Advances one frame. Returns the next scene to switch to, or nothing to stay.
    [[nodiscard]] virtual bn::optional<scene_type> update() = 0;
};

}

#endif
