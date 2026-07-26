#ifndef SUIKA_FRUIT_H
#define SUIKA_FRUIT_H

#include "bn_fixed.h"
#include "bn_vector.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_tiles_ptr.h"

namespace suika
{

// ---- gameplay tuning ------------------------------------------------------
constexpr int MAX_TYPE = 9;        // fruit_9 is the biggest fruit
constexpr int SPAWN_MAX_TYPE = 4;  // only the 5 smallest fruits can be dropped
constexpr int MAX_FRUITS = 48;

// ---- corrupted fruits -----------------------------------------------------
// Corruption spreads through merges: whenever a corrupted fruit takes part in a
// merge, the fruit it becomes is corrupted too. A corrupted fruit of type N is
// drawn with the animated corrupted_fruit_<N> sprite when that art exists.
//
// Only the first CORRUPTED_TYPES fruit types have corrupted art so far. To add
// the next one: drop corrupted_fruit_<N>.bmp/.json into graphics/, add its
// include and case to corrupted_sprite_item() in fruit.cpp, then bump this.
constexpr int CORRUPTED_TYPES = 10;   // corrupted_fruit_0 .. corrupted_fruit_9
constexpr int CORRUPT_FRAMES = 3;    // max vertical frames in a corrupted_fruit_<n>.bmp
                                     // (some, like corrupted_fruit_1, have fewer and stay static)

// Play area, in screen coordinates (centre of the screen is 0,0).
constexpr bn::fixed LEFT = -58;
constexpr bn::fixed RIGHT = 58;
constexpr bn::fixed SPAWN_Y = -70;

// Controls / rules used by the game scene.
constexpr bn::fixed CURSOR_SPEED = 2;
constexpr int DROP_COOLDOWN = 24;
constexpr int OVERFLOW_LIMIT = 110;

struct fruit_t
{
    bn::fixed x;
    bn::fixed y;
    bn::fixed vx;
    bn::fixed vy;
    int type;
    bn::sprite_ptr sprite;
    bool corrupted = false;   // true for the special first fruit and anything it merges into
};

using fruit_vector = bn::vector<fruit_t, MAX_FRUITS>;

[[nodiscard]] bn::fixed fruit_radius(int type);

// Butano sprite items are separate symbols, so map a fruit type to one here.
// When corrupted is true, a fruit whose type has corrupted art uses the animated
// corrupted_fruit_<type> sprite; types without corrupted art fall back to the
// normal design (but still spread corruption when they merge).
[[nodiscard]] bn::sprite_ptr create_fruit_sprite(int type, bn::fixed x, bn::fixed y,
                                                 bool corrupted = false);

// True when a fruit type has an animated corrupted_fruit_<type> sprite.
[[nodiscard]] bool type_has_corrupted(int type);

// Fills out with the CORRUPT_FRAMES animation-frame tiles for a corrupted fruit
// of the given type. out is cleared and left empty when the type has no art.
void create_corrupted_tiles(int type,
                            bn::vector<bn::sprite_tiles_ptr, CORRUPT_FRAMES>& out);

// Integrates one frame of the fruit simulation (gravity, collisions, sleeping).
void step_physics(fruit_vector& fruits);

// Merges one touching same-type pair (if any) and returns true when it did.
bool try_merge(fruit_vector& fruits, int& score);

[[nodiscard]] bool is_overflowing(const fruit_vector& fruits);

}

#endif
