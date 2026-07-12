#ifndef BN_REGULAR_BG_ITEMS_SUIKA_BG_H
#define BN_REGULAR_BG_ITEMS_SUIKA_BG_H

#include "bn_regular_bg_item.h"

//{{BLOCK(suika_bg_bn_gfx)

//======================================================================
//
//	suika_bg_bn_gfx, 256x256@4, 
//	+ palette 16 entries, not compressed
//	+ 83 tiles (t|f|p reduced) not compressed
//	+ regular map (flat), not compressed, 32x32 
//	Total size: 32 + 2656 + 2048 = 4736
//
//	Time-stamp: 2026-07-12, 14:12:40
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_SUIKA_BG_BN_GFX_H
#define GRIT_SUIKA_BG_BN_GFX_H

#define suika_bg_bn_gfxTilesLen 2656
extern const bn::tile suika_bg_bn_gfxTiles[83];

#define suika_bg_bn_gfxMapLen 2048
extern const bn::regular_bg_map_cell suika_bg_bn_gfxMap[1024];

#define suika_bg_bn_gfxPalLen 32
extern const bn::color suika_bg_bn_gfxPal[16];

#endif // GRIT_SUIKA_BG_BN_GFX_H

//}}BLOCK(suika_bg_bn_gfx)

namespace bn::regular_bg_items
{
    constexpr inline regular_bg_item suika_bg(
            regular_bg_tiles_item(span<const tile>(suika_bg_bn_gfxTiles, 83), bpp_mode::BPP_4, compression_type::NONE), 
            bg_palette_item(span<const color>(suika_bg_bn_gfxPal, 16), bpp_mode::BPP_4, compression_type::NONE),
            regular_bg_map_item(suika_bg_bn_gfxMap[0], size(32, 32), compression_type::NONE));
}

#endif

