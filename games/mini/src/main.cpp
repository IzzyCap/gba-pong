#include "bn_core.h"
#include "bn_keypad.h"
#include "bn_display.h"
#include "bn_fixed.h"
#include "bn_string.h"
#include "bn_vector.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "common_variable_8x8_sprite_font.h"

// A super tiny game: move a text cursor around the screen with the D-pad.
// Collect "coins" (text characters) that appear at fixed positions.
// Score goes up each time you reach one.

int main()
{
    bn::core::init();

    bn::sprite_text_generator text_generator(common::variable_8x8_sprite_font);

    // Player position (center of screen = 0,0)
    bn::fixed player_x = 0;
    bn::fixed player_y = 0;

    // "Coin" positions (there are 5 fixed spots to visit)
    bn::fixed coin_x[] = {-80, 60, -40, 90, 0};
    bn::fixed coin_y[] = {-30, 50, 40, -50, 0};
    int current_coin = 0;
    int score = 0;
    const int total_coins = 5;

    // Movement speed
    bn::fixed speed = 1.5;

    // Sprite storage for text rendering
    bn::vector<bn::sprite_ptr, 32> player_sprites;
    bn::vector<bn::sprite_ptr, 32> coin_sprites;
    bn::vector<bn::sprite_ptr, 32> hud_sprites;

    while(true)
    {
        // --- Input ---
        if(bn::keypad::left_held() && player_x > -112)
        {
            player_x -= speed;
        }
        if(bn::keypad::right_held() && player_x < 112)
        {
            player_x += speed;
        }
        if(bn::keypad::up_held() && player_y > -72)
        {
            player_y -= speed;
        }
        if(bn::keypad::down_held() && player_y < 72)
        {
            player_y += speed;
        }

        // --- Collision: check if player reached the coin ---
        if(current_coin < total_coins)
        {
            bn::fixed dx = player_x - coin_x[current_coin];
            bn::fixed dy = player_y - coin_y[current_coin];

            // Simple distance check (squared, to avoid sqrt)
            if(dx * dx + dy * dy < 100)
            {
                score++;
                current_coin++;
            }
        }

        // --- Rendering (text-based, no sprite art needed) ---
        player_sprites.clear();
        coin_sprites.clear();
        hud_sprites.clear();

        // Draw player as "@"
        text_generator.generate(player_x.integer(), player_y.integer(), "@", player_sprites);

        // Draw current coin as "*"
        if(current_coin < total_coins)
        {
            text_generator.generate(
                coin_x[current_coin].integer(),
                coin_y[current_coin].integer(),
                "*", coin_sprites);
        }

        // Draw HUD
        if(current_coin >= total_coins)
        {
            text_generator.generate(-48, -72, "You win! Nice job!", hud_sprites);
        }
        else
        {
            bn::string<32> hud_text = "Score: " + bn::to_string<32>(score);
            text_generator.generate(-48, -72, hud_text, hud_sprites);
        }

        bn::core::update();
    }
}
