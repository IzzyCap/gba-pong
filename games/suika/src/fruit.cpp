#include "fruit.h"

#include "bn_math.h"

#include "bn_sprite_items_fruit_0.h"
#include "bn_sprite_items_fruit_1.h"
#include "bn_sprite_items_fruit_2.h"
#include "bn_sprite_items_fruit_3.h"
#include "bn_sprite_items_fruit_4.h"
#include "bn_sprite_items_fruit_5.h"
#include "bn_sprite_items_fruit_6.h"
#include "bn_sprite_items_fruit_7.h"
#include "bn_sprite_items_fruit_8.h"
#include "bn_sprite_items_fruit_9.h"

namespace suika
{

namespace
{
    // Play area / physics tuning kept private to the simulation.
    constexpr bn::fixed FLOOR = 70;
    constexpr bn::fixed DANGER_Y = -52;

    constexpr bn::fixed GRAVITY = 0.30;
    constexpr bn::fixed MAX_FALL = 5;
    constexpr bn::fixed WALL_REST = 0.8;    // energy kept when bouncing off a wall
    constexpr bn::fixed FRUIT_REST = 0.7;  // energy kept when fruits bounce apart
    constexpr bn::fixed AIR_DAMP = 0.99;
    constexpr int COLLISION_ITERS = 4;

    // Contact stabilisation (prevents resting fruits from trembling).
    constexpr bn::fixed PENETRATION_SLOP = 0.75;  // overlap allowed before correcting
    constexpr bn::fixed CORRECTION = 0.8;          // fraction of excess overlap resolved
    constexpr bn::fixed REST_MIN_SPEED = 1;        // min approach speed for a real bounce
    // Settle "sensibility": lower SLEEP_DRIFT_EPS / higher SLEEP_FRAMES keep
    // fruits lively for longer before they freeze; lower WAKE_PENETRATION lets a
    // new drop disturb an already-settled pile more easily.
    constexpr bn::fixed SLEEP_DRIFT_EPS = 2;       // wander allowed from the settle anchor
    constexpr int SLEEP_FRAMES = 24;               // frames staying put before a fruit sleeps
    constexpr bn::fixed WAKE_PENETRATION = 1.8;      // overlap big enough to wake a sleeper

    void clamp_to_walls(fruit_t& f)
    {
        bn::fixed r = fruit_radius(f.type);

        if(f.x < LEFT + r)
        {
            f.x = LEFT + r;

            if(f.vx < 0)
            {
                f.vx = -f.vx * WALL_REST;
            }
        }
        else if(f.x > RIGHT - r)
        {
            f.x = RIGHT - r;

            if(f.vx > 0)
            {
                f.vx = -f.vx * WALL_REST;
            }
        }

        if(f.y > FLOOR - r)
        {
            f.y = FLOOR - r;

            if(f.vy > 0)
            {
                // Only bounce if moving fast enough, otherwise just stop
                if(f.vy > 1)
                {
                    f.vy = -f.vy * WALL_REST;
                }
                else
                {
                    f.vy = 0;
                }
            }

            f.vx *= bn::fixed(0.9);  // friction against the floor
        }
    }

    // Remove a fruit by swapping the last element into its slot (order-independent).
    void remove_fruit(fruit_vector& fruits, int index)
    {
        int last = fruits.size() - 1;

        if(index != last)
        {
            fruits[index] = fruits[last];
        }

        fruits.pop_back();
    }
}

bn::fixed fruit_radius(int type)
{
    static const bn::fixed radii[MAX_TYPE + 1] =
    {
        4, 7, 7, 10, 11, 17, 18, 18, 21, 28
    };

    return radii[type];
}

bn::sprite_ptr create_fruit_sprite(int type, bn::fixed x, bn::fixed y)
{
    switch(type)
    {
    case 0:  return bn::sprite_items::fruit_0.create_sprite(x, y);
    case 1:  return bn::sprite_items::fruit_1.create_sprite(x, y);
    case 2:  return bn::sprite_items::fruit_2.create_sprite(x, y);
    case 3:  return bn::sprite_items::fruit_3.create_sprite(x, y);
    case 4:  return bn::sprite_items::fruit_4.create_sprite(x, y);
    case 5:  return bn::sprite_items::fruit_5.create_sprite(x, y);
    case 6:  return bn::sprite_items::fruit_6.create_sprite(x, y);
    case 7:  return bn::sprite_items::fruit_7.create_sprite(x, y);
    case 8:  return bn::sprite_items::fruit_8.create_sprite(x, y);
    default: return bn::sprite_items::fruit_9.create_sprite(x, y);
    }
}

void step_physics(fruit_vector& fruits)
{
    int n = fruits.size();

    // Integrate gravity + velocity, then keep everyone inside the jar.
    // Gravity is applied to every awake fruit, even ones resting on the pile,
    // so an off-centre stack topples and spreads out naturally instead of
    // freezing into a rigid tower. The trembling this constant downward
    // pressure would otherwise cause is dealt with by the sleep pass below,
    // not by cancelling gravity.
    for(int i = 0; i < n; ++i)
    {
        fruit_t& f = fruits[i];

        if(f.asleep)
        {
            continue;  // resting fruits are frozen until disturbed
        }

        f.vy += GRAVITY;

        if(f.vy > MAX_FALL)
        {
            f.vy = MAX_FALL;
        }

        f.vx *= AIR_DAMP;
        f.x += f.vx;
        f.y += f.vy;
        clamp_to_walls(f);
    }

    // Resolve overlaps a few times so stacks settle nicely.
    for(int iter = 0; iter < COLLISION_ITERS; ++iter)
    {
        for(int i = 0; i < n; ++i)
        {
            for(int j = i + 1; j < n; ++j)
            {
                fruit_t& a = fruits[i];
                fruit_t& b = fruits[j];
                bn::fixed dx = b.x - a.x;
                bn::fixed dy = b.y - a.y;
                bn::fixed rr = fruit_radius(a.type) + fruit_radius(b.type);
                bn::fixed d2 = dx * dx + dy * dy;

                if(d2 < rr * rr && d2 > bn::fixed(0.01))
                {
                    bn::fixed d = bn::sqrt(d2);
                    bn::fixed pen = rr - d;

                    // Ignore tiny overlaps: this "slop" is what stops the
                    // gravity/resolve tug-of-war that causes trembling.
                    if(pen <= PENETRATION_SLOP)
                    {
                        continue;
                    }

                    // Only a hard impact wakes a sleeping fruit; gentle resting
                    // contact leaves settled fruits asleep and immovable.
                    if(pen > WAKE_PENETRATION)
                    {
                        a.asleep = false;
                        b.asleep = false;
                        a.settle_frames = 0;
                        b.settle_frames = 0;
                        a.anchor_x = a.x;
                        a.anchor_y = a.y;
                        b.anchor_x = b.x;
                        b.anchor_y = b.y;
                    }

                    // Sleeping fruits act as immovable ground (weight 0).
                    bn::fixed wa = a.asleep ? bn::fixed(0) : bn::fixed(1);
                    bn::fixed wb = b.asleep ? bn::fixed(0) : bn::fixed(1);
                    bn::fixed wsum = wa + wb;

                    if(wsum <= bn::fixed(0))
                    {
                        continue;  // both asleep: leave the contact be
                    }

                    bn::fixed nx = dx / d;
                    bn::fixed ny = dy / d;
                    bn::fixed total = (pen - PENETRATION_SLOP) * CORRECTION;
                    a.x -= nx * total * wa / wsum;
                    a.y -= ny * total * wa / wsum;
                    b.x += nx * total * wb / wsum;
                    b.y += ny * total * wb / wsum;

                    bn::fixed vn = (b.vx - a.vx) * nx + (b.vy - a.vy) * ny;

                    if(vn < 0)  // only respond when the fruits are approaching
                    {
                        // Bounce only on fast impacts; slow contacts are inelastic
                        // so resting stacks don't jiggle.
                        bn::fixed rest = (vn < -REST_MIN_SPEED) ? FRUIT_REST : bn::fixed(0);
                        bn::fixed imp = -(1 + rest) * vn / wsum;
                        a.vx -= imp * nx * wa;
                        a.vy -= imp * ny * wa;
                        b.vx += imp * nx * wb;
                        b.vy += imp * ny * wb;
                    }
                }
            }
        }

        for(int i = 0; i < n; ++i)
        {
            if(! fruits[i].asleep)
            {
                clamp_to_walls(fruits[i]);
            }
        }
    }

    // Put settled fruits to sleep so they stop trembling. A fruit squeezed by
    // several neighbours at once is shoved in conflicting directions every
    // frame, so its instantaneous speed never reaches zero even though it is
    // going nowhere: it just oscillates around a fixed point. Rather than look
    // at speed, anchor each fruit and watch how far it wanders from the anchor.
    // A trembling or resting fruit stays within a tiny radius and falls asleep;
    // a fruit that is really falling, rolling or spreading keeps moving away,
    // re-anchors, and stays awake. Sleeping fruits act as immovable ground, so
    // once one settles its neighbours settle against it and the pile locks in
    // place until the next drop or merge disturbs it.
    for(int i = 0; i < n; ++i)
    {
        fruit_t& f = fruits[i];

        if(f.asleep)
        {
            continue;
        }

        bn::fixed drift = bn::abs(f.x - f.anchor_x) + bn::abs(f.y - f.anchor_y);

        if(drift < SLEEP_DRIFT_EPS)
        {
            if(++f.settle_frames >= SLEEP_FRAMES)
            {
                f.vx = 0;  // rest exactly where it is, no teleport to the anchor
                f.vy = 0;
                f.asleep = true;
            }
        }
        else
        {
            // Really moving: restart the timer and re-anchor here.
            f.settle_frames = 0;
            f.anchor_x = f.x;
            f.anchor_y = f.y;
        }
    }
}

bool try_merge(fruit_vector& fruits, int& score)
{
    int n = fruits.size();

    for(int i = 0; i < n; ++i)
    {
        for(int j = i + 1; j < n; ++j)
        {
            fruit_t& a = fruits[i];
            fruit_t& b = fruits[j];

            if(a.type == b.type)
            {
                bn::fixed dx = b.x - a.x;
                bn::fixed dy = b.y - a.y;
                bn::fixed rr = fruit_radius(a.type) + fruit_radius(b.type);
                bn::fixed d2 = dx * dx + dy * dy;

                if(d2 <= rr * rr + 2)
                {
                    int type = a.type;
                    bn::fixed nx = (a.x + b.x) / 2;
                    bn::fixed ny = (a.y + b.y) / 2;
                    bn::fixed nvx = (a.vx + b.vx) / 2;
                    bn::fixed nvy = (a.vy + b.vy) / 2;

                    remove_fruit(fruits, j);  // j > i, so remove it first
                    remove_fruit(fruits, i);

                    if(type < MAX_TYPE)
                    {
                        int nt = type + 1;
                        fruits.push_back({nx, ny, nvx, nvy, nx, ny, nt, 0, false,
                                          create_fruit_sprite(nt, nx, ny)});
                        score += nt;
                    }
                    else
                    {
                        score += 100;  // two biggest fruits pop for a big bonus
                    }

                    return true;
                }
            }
        }
    }

    return false;
}

bool is_overflowing(const fruit_vector& fruits)
{
    for(const fruit_t& f : fruits)
    {
        if(f.y < DANGER_Y)
        {
            return true;
        }
    }

    return false;
}

}
