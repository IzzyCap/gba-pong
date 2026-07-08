/*
 * Suika (watermelon) game for the GBA, built with Butano.
 *
 * Drop fruits into the jar. When two fruits of the same kind touch they merge
 * into the next, bigger fruit. Fill the jar past the line and it's game over.
 * The goal is to make the biggest fruit (the watermelon) and score points.
 */

#include "bn_core.h"
#include "bn_keypad.h"
#include "bn_math.h"
#include "bn_fixed.h"
#include "bn_random.h"
#include "bn_vector.h"
#include "bn_string.h"
#include "bn_algorithm.h"
#include "bn_sprite_ptr.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sprite_text_generator.h"

#include "common_variable_8x8_sprite_font.h"

#include "bn_regular_bg_items_suika_bg.h"
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
#include "bn_sprite_items_fruit_10.h"

namespace
{
    // ---- tuning constants -------------------------------------------------
    constexpr int MAX_TYPE = 10;       // fruit_10 == watermelon (biggest)
    constexpr int SPAWN_MAX_TYPE = 4;  // only the 5 smallest fruits can be dropped
    constexpr int MAX_FRUITS = 48;

    // Play area, in screen coordinates (centre of the screen is 0,0).
    constexpr bn::fixed LEFT = -58;
    constexpr bn::fixed RIGHT = 58;
    constexpr bn::fixed FLOOR = 70;
    constexpr bn::fixed SPAWN_Y = -70;
    constexpr bn::fixed DANGER_Y = -52;

    // Physics.
    constexpr bn::fixed GRAVITY = 0.30;
    constexpr bn::fixed MAX_FALL = 5;
    constexpr bn::fixed WALL_REST = 0.8;    // energy kept when bouncing off a wall
    constexpr bn::fixed FRUIT_REST = 0.7;  // energy kept when fruits bounce apart
    constexpr bn::fixed AIR_DAMP = 0.99;
    constexpr bn::fixed CURSOR_SPEED = 2;
    constexpr int DROP_COOLDOWN = 24;
    constexpr int OVERFLOW_LIMIT = 110;
    constexpr int COLLISION_ITERS = 3;

    struct fruit_t
    {
        bn::fixed x;
        bn::fixed y;
        bn::fixed vx;
        bn::fixed vy;
        int type;
        bn::sprite_ptr sprite;
    };

    using fruit_vector = bn::vector<fruit_t, MAX_FRUITS>;

    [[nodiscard]] bn::fixed fruit_radius(int type)
    {
        static const bn::fixed radii[MAX_TYPE + 1] =
        {
            4, 7, 7, 9, 11, 17, 20, 25, 28, 30, 31
        };

        return radii[type];
    }

    // Butano sprite items are separate symbols, so map a fruit type to one here.
    [[nodiscard]] bn::sprite_ptr create_fruit_sprite(int type, bn::fixed x, bn::fixed y)
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
        case 9:  return bn::sprite_items::fruit_9.create_sprite(x, y);
        default: return bn::sprite_items::fruit_10.create_sprite(x, y);
        }
    }

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
                f.vy = -f.vy * WALL_REST;
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

    void step_physics(fruit_vector& fruits)
    {
        int n = fruits.size();

        // Integrate gravity + velocity, then keep everyone inside the jar.
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
                        bn::fixed nx = dx / d;
                        bn::fixed ny = dy / d;
                        bn::fixed push = (rr - d) / 2;
                        a.x -= nx * push;
                        a.y -= ny * push;
                        b.x += nx * push;
                        b.y += ny * push;

                        bn::fixed vn = (b.vx - a.vx) * nx + (b.vy - a.vy) * ny;

                        if(vn < 0)  // only respond when the fruits are approaching
                        {
                            bn::fixed imp = -(1 + FRUIT_REST) * vn / 2;
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

    // Merges one touching same-type pair (if any) and returns true when it did.
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
                            score += 100;  // two watermelons pop for a big bonus
                        }

                        return true;
                    }
                }
            }
        }

        return false;
    }

    [[nodiscard]] bool is_overflowing(const fruit_vector& fruits)
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

int main()
{
    bn::core::init();

    bn::regular_bg_ptr bg = bn::regular_bg_items::suika_bg.create_bg(0, 0);
    bn::sprite_text_generator text_generator(common::variable_8x8_sprite_font);

    bn::random random;
    fruit_vector fruits;

    bn::vector<bn::sprite_ptr, 24> score_sprites;
    bn::vector<bn::sprite_ptr, 32> msg_sprites;

    int current_type = random.get_int(0, SPAWN_MAX_TYPE + 1);
    int next_type = random.get_int(0, SPAWN_MAX_TYPE + 1);
    bn::fixed cursor_x = 0;
    int drop_cooldown = 0;
    int score = 0;
    int last_score = -1;
    int overflow_frames = 0;
    bool game_over = false;

    bn::sprite_ptr current_sprite = create_fruit_sprite(current_type, cursor_x, SPAWN_Y);
    bn::sprite_ptr next_sprite = create_fruit_sprite(next_type, 96, -44);

    bn::vector<bn::sprite_ptr, 6> next_label;
    text_generator.set_center_alignment();
    text_generator.generate(96, -66, "NEXT", next_label);

    while(true)
    {
        if(! game_over)
        {
            bn::fixed r = fruit_radius(current_type);

            if(bn::keypad::left_held())
            {
                cursor_x -= CURSOR_SPEED;
            }

            if(bn::keypad::right_held())
            {
                cursor_x += CURSOR_SPEED;
            }

            cursor_x = bn::clamp(cursor_x, LEFT + r, RIGHT - r);

            if(drop_cooldown > 0)
            {
                --drop_cooldown;
            }

            if(bn::keypad::a_pressed() && drop_cooldown == 0 && fruits.size() < MAX_FRUITS - 1)
            {
                fruits.push_back({cursor_x, SPAWN_Y, bn::fixed(0), bn::fixed(0.5),
                                  current_type, create_fruit_sprite(current_type, cursor_x, SPAWN_Y)});
                current_type = next_type;
                next_type = random.get_int(0, SPAWN_MAX_TYPE + 1);
                next_sprite = create_fruit_sprite(next_type, 96, -44);
                current_sprite = create_fruit_sprite(current_type, cursor_x, SPAWN_Y);
                drop_cooldown = DROP_COOLDOWN;

                r = fruit_radius(current_type);
                cursor_x = bn::clamp(cursor_x, LEFT + r, RIGHT - r);
            }

            step_physics(fruits);

            int guard = 0;

            while(try_merge(fruits, score) && guard < 32)
            {
                ++guard;
            }

            for(fruit_t& f : fruits)
            {
                f.sprite.set_position(f.x, f.y);
            }

            current_sprite.set_position(cursor_x, SPAWN_Y);
            current_sprite.set_visible(drop_cooldown == 0);

            if(is_overflowing(fruits))
            {
                if(++overflow_frames > OVERFLOW_LIMIT)
                {
                    game_over = true;
                }
            }
            else
            {
                overflow_frames = 0;
            }
        }
        else
        {
            current_sprite.set_visible(false);

            if(msg_sprites.empty())
            {
                text_generator.set_center_alignment();
                text_generator.generate(0, -8, "GAME OVER", msg_sprites);
                text_generator.generate(0, 6, "Press START", msg_sprites);
            }

            if(bn::keypad::start_pressed())
            {
                fruits.clear();
                msg_sprites.clear();
                score = 0;
                last_score = -1;
                overflow_frames = 0;
                drop_cooldown = 0;
                cursor_x = 0;
                current_type = random.get_int(0, SPAWN_MAX_TYPE + 1);
                next_type = random.get_int(0, SPAWN_MAX_TYPE + 1);
                current_sprite = create_fruit_sprite(current_type, cursor_x, SPAWN_Y);
                next_sprite = create_fruit_sprite(next_type, 96, -44);
                game_over = false;
            }
        }

        if(score != last_score)
        {
            score_sprites.clear();
            text_generator.set_left_alignment();
            bn::string<32> text = "Score: " + bn::to_string<32>(score);
            text_generator.generate(-116, -76, text, score_sprites);
            last_score = score;
        }

        bn::core::update();
    }
}
