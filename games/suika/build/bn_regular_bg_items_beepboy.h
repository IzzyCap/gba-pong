#ifndef BN_REGULAR_BG_ITEMS_BEEPBOY_H
#define BN_REGULAR_BG_ITEMS_BEEPBOY_H

#include "bn_regular_bg_item.h"

//{{BLOCK(beepboy_bn_gfx)

//======================================================================
//
//	beepboy_bn_gfx, 256x256@4, 
//	+ palette 16 entries, not compressed
//	+ 43 tiles (t|f|p reduced) not compressed
//	+ regular map (flat), not compressed, 32x32 
//	Total size: 32 + 1376 + 2048 = 3456
//
//	Time-stamp: 2026-07-26, 23:15:11
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_BEEPBOY_BN_GFX_H
#define GRIT_BEEPBOY_BN_GFX_H

#define beepboy_bn_gfxTilesLen 1376
extern const bn::tile beepboy_bn_gfxTiles[43];

#define beepboy_bn_gfxMapLen 2048
extern const bn::regular_bg_map_cell beepboy_bn_gfxMap[1024];

#define beepboy_bn_gfxPalLen 32
extern const bn::color beepboy_bn_gfxPal[16];

#endif // GRIT_BEEPBOY_BN_GFX_H

//}}BLOCK(beepboy_bn_gfx)

namespace bn::regular_bg_items
{
    constexpr inline regular_bg_item beepboy(
            regular_bg_tiles_item(span<const tile>(beepboy_bn_gfxTiles, 43), bpp_mode::BPP_4, compression_type::NONE), 
            bg_palette_item(span<const color>(beepboy_bn_gfxPal, 16), bpp_mode::BPP_4, compression_type::NONE),
            regular_bg_map_item(beepboy_bn_gfxMap[0], size(32, 32), compression_type::NONE));
}

#endif

