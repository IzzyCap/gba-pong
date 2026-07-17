#include "creepy_scene.h"

#include "bn_regular_bg_items_hypno_5.h"
#include "bn_regular_bg_items_hypno_6.h"

namespace suika
{
  namespace
  {
    // Show each of the two final frames for this many frames, once, no loop.
    constexpr int REVEAL_FRAMES = 6;
  }

  creepy_scene::creepy_scene(bn::sprite_text_generator&, scene_type next) :
    _bg(bn::regular_bg_items::hypno_5.create_bg(0, 0)),
    _next(next)
  {
  }

  bn::optional<scene_type> creepy_scene::update()
  {
    ++_timer;

    if(_timer == REVEAL_FRAMES)
    {
      _bg.set_item(bn::regular_bg_items::hypno_6);
    }
    else if(_timer >= 2 * REVEAL_FRAMES)
    {
      return _next;   // both frames shown: move on
    }

    return bn::nullopt;
  }

}
