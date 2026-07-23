#ifndef SUIKA_TYPEWRITER_TEXT_H
#define SUIKA_TYPEWRITER_TEXT_H

#include "bn_vector.h"
#include "bn_string.h"
#include "bn_string_view.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"

namespace suika
{

// Reveals a message one character at a time, like the dialogue boxes in classic
// RPGs. Feed it some text (split into lines with '\n'), then call update() once
// per frame: characters appear left-to-right, top-to-bottom, at a fixed speed.
//
// Long messages can be split into PAGES with the '\f' character. Only one page
// shows at a time; call next_page() (typically on a button press) to continue.
//
// Usage:
//     typewriter_text dialog(text_generator, -100, -20, 14, 3);
//     dialog.set_text("Hello.\nCan you hear me?\fSecond page here.");
//     // every frame:
//     dialog.update();               // advances the reveal of the current page
//     if(a_pressed) {
//         if(! dialog.done())        dialog.skip();        // finish this page
//         else if(dialog.has_next_page()) dialog.next_page();  // continue
//         else                       /* close the box */;
//     }
class typewriter_text
{

public:
    static constexpr int max_lines = 6;
    static constexpr int max_line_length = 30;
    static constexpr int max_pages = 8;
    static constexpr int max_page_length = max_lines * max_line_length;
    static constexpr int max_sprites = 64;

    // The character used in a message to mark a page break.
    static constexpr char page_break = '\f';

    // text_generator: font/renderer to draw with (left alignment is used).
    // x / top_y:      top-left corner of the first line.
    // line_height:    vertical spacing between lines.
    // frames_per_char: how many frames each new character waits (higher = slower).
    typewriter_text(bn::sprite_text_generator& text_generator, bn::fixed x, bn::fixed top_y,
                    bn::fixed line_height, int frames_per_char);

    // Sets the message and restarts from the first page's first character.
    // Pages are split on '\f'; within a page, lines are split on '\n' and
    // over-long lines are wrapped at max_line_length.
    void set_text(const bn::string_view& text);

    // Advances the reveal by one frame. Call once per frame.
    void update();

    // Reveals the whole current page immediately.
    void skip();

    // True once every character of the CURRENT page is visible.
    [[nodiscard]] bool done() const
    {
        return _visible_chars >= _total_chars;
    }

    // True if there is another page after the current one.
    [[nodiscard]] bool has_next_page() const
    {
        return _page_index + 1 < _pages.size();
    }

    // Moves to the next page and restarts the reveal for it. Returns false when
    // already on the last page.
    bool next_page();

private:
    bn::sprite_text_generator& _text_generator;
    bn::fixed _x;
    bn::fixed _top_y;
    bn::fixed _line_height;
    int _frames_per_char;
    int _frame_counter = 0;
    int _visible_chars = 0;
    int _total_chars = 0;
    int _page_index = 0;
    bn::vector<bn::string<max_page_length>, max_pages> _pages;
    bn::vector<bn::string<max_line_length>, max_lines> _lines;
    bn::vector<bn::sprite_ptr, max_sprites> _sprites;

    void _load_page();
    void _redraw();
};

}

#endif
