#include "typewriter_text.h"

namespace suika
{

typewriter_text::typewriter_text(bn::sprite_text_generator& text_generator, bn::fixed x,
                                 bn::fixed top_y, bn::fixed line_height, int frames_per_char) :
    _text_generator(text_generator),
    _x(x),
    _top_y(top_y),
    _line_height(line_height),
    _frames_per_char(frames_per_char > 1 ? frames_per_char : 1)
{
}

void typewriter_text::set_text(const bn::string_view& text)
{
    _pages.clear();
    _page_index = 0;

    bn::string<max_page_length> current;

    for(char character : text)
    {
        if(character == page_break)
        {
            _pages.push_back(current);
            current.clear();

            if(_pages.full())
            {
                current.clear();
                break;
            }

            continue;
        }

        if(current.full())
        {
            // Page buffer is full: start a new page rather than drop characters.
            _pages.push_back(current);
            current.clear();

            if(_pages.full())
            {
                break;
            }
        }

        current.push_back(character);
    }

    if(! current.empty() && ! _pages.full())
    {
        _pages.push_back(current);
    }

    _load_page();
}

void typewriter_text::_load_page()
{
    _lines.clear();

    if(_page_index >= 0 && _page_index < _pages.size())
    {
        const bn::string<max_page_length>& page = _pages[_page_index];
        bn::string<max_line_length> current;

        for(char character : page)
        {
            if(character == '\n')
            {
                _lines.push_back(current);
                current.clear();

                if(_lines.full())
                {
                    current.clear();
                    break;
                }

                continue;
            }

            if(current.full())
            {
                _lines.push_back(current);
                current.clear();

                if(_lines.full())
                {
                    break;
                }
            }

            current.push_back(character);
        }

        if(! current.empty() && ! _lines.full())
        {
            _lines.push_back(current);
        }
    }

    _total_chars = 0;

    for(const bn::string<max_line_length>& line : _lines)
    {
        _total_chars += line.size();
    }

    _visible_chars = 0;
    _frame_counter = 0;
    _redraw();
}

bool typewriter_text::next_page()
{
    if(! has_next_page())
    {
        return false;
    }

    ++_page_index;
    _load_page();
    return true;
}

void typewriter_text::update()
{
    if(done())
    {
        return;
    }

    ++_frame_counter;

    if(_frame_counter >= _frames_per_char)
    {
        _frame_counter = 0;
        ++_visible_chars;
        _redraw();
    }
}

void typewriter_text::skip()
{
    if(! done())
    {
        _visible_chars = _total_chars;
        _redraw();
    }
}

void typewriter_text::_redraw()
{
    _sprites.clear();
    _text_generator.set_left_alignment();

    bn::fixed y = _top_y;
    int remaining = _visible_chars;

    for(const bn::string<max_line_length>& line : _lines)
    {
        int count = line.size();

        if(count > remaining)
        {
            count = remaining;
        }

        if(count > 0)
        {
            _text_generator.generate(_x, y, bn::string_view(line.data(), count), _sprites);
            remaining -= count;
        }

        y += _line_height;

        if(remaining <= 0)
        {
            break;
        }
    }
}

}
