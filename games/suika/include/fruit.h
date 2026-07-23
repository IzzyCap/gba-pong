#ifndef SUIKA_FRUIT_H
#define SUIKA_FRUIT_H

#include "bn_fixed.h"
#include "bn_vector.h"
#include "bn_sprite_ptr.h"

namespace suika
{

// ---- gameplay tuning ------------------------------------------------------
constexpr int MAX_TYPE = 10;       // fruit_10 is the biggest fruit
constexpr int SPAWN_MAX_TYPE = 4;  // only the 5 smallest fruits can be dropped
constexpr int MAX_FRUITS = 48;

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
    bool corrupted = false;   // true only for the single special first fruit
};

using fruit_vector = bn::vector<fruit_t, MAX_FRUITS>;

[[nodiscard]] bn::fixed fruit_radius(int type);

// Butano sprite items are separate symbols, so map a fruit type to one here.
// When corrupted is true (only the special first fruit), a type-0 fruit uses the
// animated corrupted_fruit_0 sprite instead of the normal design.
[[nodiscard]] bn::sprite_ptr create_fruit_sprite(int type, bn::fixed x, bn::fixed y,
                                                 bool corrupted = false);

// Integrates one frame of the fruit simulation (gravity, collisions, sleeping).
void step_physics(fruit_vector& fruits);

// Merges one touching same-type pair (if any) and returns true when it did.
bool try_merge(fruit_vector& fruits, int& score);

[[nodiscard]] bool is_overflowing(const fruit_vector& fruits);

}

#endif
