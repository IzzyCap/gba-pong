/*
 * Suika (watermelon) game for the GBA, built with Butano.
 *
 * Drop fruits into the jar. When two fruits of the same kind touch they merge
 * into the next, bigger fruit. Fill the jar past the line and it's game over.
 * The goal is to make the biggest fruit and score points.
 *
 * Each screen lives in its own scene (see menu_scene / game_scene); this file
 * just owns the shared font and swaps between scenes. Each scene creates its
 * own background (menu: suika_bg, game: suika_game_zone).
 */

#include "bn_core.h"
#include "bn_unique_ptr.h"
#include "bn_sprite_text_generator.h"

#include "common_variable_8x8_sprite_font.h"

#include "scene.h"
#include "menu_scene.h"
#include "game_scene.h"

namespace
{
    bn::unique_ptr<suika::scene> make_scene(suika::scene_type type,
                                            bn::sprite_text_generator& text_generator)
    {
        switch(type)
        {

        case suika::scene_type::game:
            return bn::unique_ptr<suika::scene>(new suika::game_scene(text_generator));

        default:
            return bn::unique_ptr<suika::scene>(new suika::menu_scene(text_generator));
        }
    }
}

int main()
{
    bn::core::init();

    bn::sprite_text_generator text_generator(common::variable_8x8_sprite_font);

    bn::unique_ptr<suika::scene> scene = make_scene(suika::scene_type::menu, text_generator);

    while(true)
    {
        if(bn::optional<suika::scene_type> next = scene->update())
        {
            scene.reset();  // free the old scene's sprites before building the next
            scene = make_scene(*next, text_generator);
        }

        bn::core::update();
    }
}
