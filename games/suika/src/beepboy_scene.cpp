#include "beepboy_scene.h"

#include "bn_keypad.h"
#include "bn_string.h"
#include "bn_color.h"
#include "bn_sprite_palette_item.h"

#include "dialogs.h"

#include "bn_regular_bg_items_suika_bg.h"
#include "bn_regular_bg_items_beepboy.h"

namespace suika
{

namespace
{
    // Beepboy's dialogue box (left-aligned typewriter, drawn low over the scene).
    constexpr bn::fixed DIALOG_X = -100;
    constexpr bn::fixed DIALOG_Y = 34;
    constexpr bn::fixed DIALOG_LINE_H = 13;
    constexpr int DIALOG_SPEED = 3;        // frames per revealed character

    constexpr int GLITCH_INTERVAL = 4;     // frames between binary re-scrambles

    // Font palette with the glyph fill (index 14) recoloured red for Beepboy.
    constexpr bn::color RED_PALETTE_COLORS[16] = {
        bn::color(0, 0, 0), bn::color(0, 0, 0), bn::color(0, 0, 0),  bn::color(0, 0, 0),
        bn::color(0, 0, 0), bn::color(0, 0, 0), bn::color(0, 0, 0),  bn::color(0, 0, 0),
        bn::color(0, 0, 0), bn::color(0, 0, 0), bn::color(0, 0, 0),  bn::color(0, 0, 0),
        bn::color(0, 0, 0), bn::color(0, 0, 0), bn::color(31, 0, 0), bn::color(0, 0, 0)
    };

    constexpr bn::sprite_palette_item RED_PALETTE_ITEM(
            bn::span<const bn::color>(RED_PALETTE_COLORS, 16),
            bn::bpp_mode::BPP_4, bn::compression_type::NONE);

    // Where the game normally draws UI text; here each slot shows random binary
    // of the same length instead. len is the character count to scramble.
    struct glitch_slot
    {
        bn::fixed x;
        bn::fixed y;
        int len;
    };

    // Centre-aligned UI text (SCORE label + value, NEXT, HOLD).
    constexpr glitch_slot CENTER_SLOTS[] = {
        {-92, -66, 5},   // "SCORE"
        {-92, -53, 4},   // score value
        { 96, -66, 4},   // "NEXT"
        {-96,  12, 4},   // "HOLD"
    };

    // Right-aligned UI text (TOP 3 heading + three ranking rows).
    constexpr glitch_slot RIGHT_SLOTS[] = {
        {116, 36, 5},    // "TOP 3"
        {116, 49, 6},
        {116, 61, 6},
        {116, 73, 6},
    };

    // Builds a string of len random '0'/'1' characters.
    bn::string<8> random_binary(bn::random& random, int len)
    {
        bn::string<8> result;

        for(int i = 0; i < len; ++i)
        {
            result += (random.get_int(0, 2) == 0) ? '0' : '1';
        }

        return result;
    }
}

beepboy_scene::beepboy_scene(bn::sprite_text_generator& text_generator) :
    _text_generator(text_generator),
    _normal_palette(text_generator.palette_item()),
    _bg(bn::regular_bg_items::suika_bg.create_bg(0, 0)),
    _beepboy_bg(bn::regular_bg_items::beepboy.create_bg(0, 0)),
    _dialog(text_generator, DIALOG_X, DIALOG_Y, DIALOG_LINE_H, DIALOG_SPEED)
{
    // Same layering as the game once beepboy is revealed: backdrop behind,
    // beepboy in the middle, jar in front. All share the sprites' backmost
    // priority so the text draws on top.
    _bg.set_priority(3);
    _beepboy_bg.set_priority(3);
    _bg.set_z_order(2);            // furthest back
    _beepboy_bg.set_z_order(1);    // between the backdrop and the jar

    // Beepboy speaks in red. Set the palette only while feeding the typewriter,
    // then restore it so the binary UI keeps the normal font colour.
    _text_generator.set_left_alignment();
    _text_generator.set_palette_item(RED_PALETTE_ITEM);
    _dialog.set_text(dialogs::beepboy());
    _text_generator.set_palette_item(_normal_palette);

    _refresh_glitch();
}

void beepboy_scene::_refresh_glitch()
{
    _glitch_sprites.clear();
    _text_generator.set_palette_item(_normal_palette);

    _text_generator.set_center_alignment();

    for(const glitch_slot& slot : CENTER_SLOTS)
    {
        _text_generator.generate(slot.x, slot.y, random_binary(_random, slot.len), _glitch_sprites);
    }

    _text_generator.set_right_alignment();

    for(const glitch_slot& slot : RIGHT_SLOTS)
    {
        _text_generator.generate(slot.x, slot.y, random_binary(_random, slot.len), _glitch_sprites);
    }
}

bn::optional<scene_type> beepboy_scene::update()
{
    if(++_glitch_timer >= GLITCH_INTERVAL)
    {
        _glitch_timer = 0;
        _refresh_glitch();
    }

    // Keep the generator red for every typewriter (re)generation this frame,
    // including skip()/next_page() below, then restore the normal palette so
    // later scenes (and the binary UI) aren't left red.
    _text_generator.set_left_alignment();
    _text_generator.set_palette_item(RED_PALETTE_ITEM);

    _dialog.update();

    bn::optional<scene_type> next;

    if(bn::keypad::a_pressed() || bn::keypad::start_pressed())
    {
        if(! _dialog.done())
        {
            _dialog.skip();
        }
        else if(_dialog.has_next_page())
        {
            _dialog.next_page();
        }
        else
        {
            next = scene_type::menu;
        }
    }

    _text_generator.set_palette_item(_normal_palette);
    _text_generator.set_center_alignment();

    return next;
}

}
