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

        // Contact stabilisation (keeps resting fruits from jittering too hard).
        constexpr bn::fixed PENETRATION_SLOP = 2;  // overlap allowed before correcting
        constexpr bn::fixed CORRECTION = 0.8;          // fraction of excess overlap resolved
        constexpr bn::fixed REST_MIN_SPEED = 1;        // min approach speed for a real bounce
        constexpr bn::fixed REST_EPS = 0.15;           // below this speed a fruit is snapped to rest

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
        // Gravity is applied to every fruit every frame, even ones resting on the
        // pile, so the whole simulation stays live: stacks keep settling, spreading
        // and toppling for as long as they are on screen.
        for(int i = 0; i < n; ++i)
        {
            fruit_t& f = fruits[i];

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

                        bn::fixed nx = dx / d;
                        bn::fixed ny = dy / d;
                        bn::fixed total = (pen - PENETRATION_SLOP) * CORRECTION;
                        bn::fixed half = total / 2;
                        a.x -= nx * half;
                        a.y -= ny * half;
                        b.x += nx * half;
                        b.y += ny * half;

                        bn::fixed vn = (b.vx - a.vx) * nx + (b.vy - a.vy) * ny;

                        if(vn < 0)  // only respond when the fruits are approaching
                        {
                            // Bounce only on fast impacts; slow contacts are inelastic
                            // so resting stacks don't jiggle.
                            bn::fixed rest = (vn < -REST_MIN_SPEED) ? FRUIT_REST : bn::fixed(0);
                            bn::fixed imp = -(1 + rest) * vn / 2;
                            a.vx -= imp * nx;
                            a.vy -= imp * ny;
                            b.vx += imp * nx;
                            b.vy += imp * ny;
                        }
                    }
                }
            }

            for(int i = 0; i < n; ++i)
            {
                clamp_to_walls(fruits[i]);
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
                            fruits.push_back({nx, ny, nvx, nvy, nt,
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
