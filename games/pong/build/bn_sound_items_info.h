#ifndef BN_SOUND_ITEMS_INFO_H
#define BN_SOUND_ITEMS_INFO_H

#include "bn_span.h"
#include "bn_sound_item.h"
#include "bn_string_view.h"

namespace bn::sound_items_info
{
    constexpr inline pair<sound_item, string_view> array[] = {
        make_pair(sound_item(16), string_view("ping")),
        make_pair(sound_item(17), string_view("pong")),
    };

    constexpr inline span<const pair<sound_item, string_view>> span(array);
}

#endif

