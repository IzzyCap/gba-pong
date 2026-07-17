#ifndef BN_REGULAR_BG_ITEMS_HYPNO_3_H
#define BN_REGULAR_BG_ITEMS_HYPNO_3_H

#include "bn_regular_bg_item.h"

//{{BLOCK(hypno_3_bn_gfx)

//======================================================================
//
//	hypno_3_bn_gfx, 256x256@4, 
//	+ palette 16 entries, not compressed
//	+ 7 tiles (t|f|p reduced) not compressed
//	+ regular map (flat), not compressed, 32x32 
//	Total size: 32 + 224 + 2048 = 2304
//
//	Time-stamp: 2026-07-17, 16:54:26
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_HYPNO_3_BN_GFX_H
#define GRIT_HYPNO_3_BN_GFX_H

#define hypno_3_bn_gfxTilesLen 224
extern const bn::tile hypno_3_bn_gfxTiles[7];

#define hypno_3_bn_gfxMapLen 2048
extern const bn::regular_bg_map_cell hypno_3_bn_gfxMap[1024];

#define hypno_3_bn_gfxPalLen 32
extern const bn::color hypno_3_bn_gfxPal[16];

#endif // GRIT_HYPNO_3_BN_GFX_H

//}}BLOCK(hypno_3_bn_gfx)

namespace bn::regular_bg_items
{
    constexpr inline regular_bg_item hypno_3(
            regular_bg_tiles_item(span<const tile>(hypno_3_bn_gfxTiles, 7), bpp_mode::BPP_4, compression_type::NONE), 
            bg_palette_item(span<const color>(hypno_3_bn_gfxPal, 16), bpp_mode::BPP_4, compression_type::NONE),
            regular_bg_map_item(hypno_3_bn_gfxMap[0], size(32, 32), compression_type::NONE));
}

#endif

