#ifndef BN_REGULAR_BG_ITEMS_SUIKA_GAME_ZONE_H
#define BN_REGULAR_BG_ITEMS_SUIKA_GAME_ZONE_H

#include "bn_regular_bg_item.h"

//{{BLOCK(suika_game_zone_bn_gfx)

//======================================================================
//
//	suika_game_zone_bn_gfx, 256x256@4, 
//	+ palette 16 entries, not compressed
//	+ 12 tiles (t|f|p reduced) not compressed
//	+ regular map (flat), not compressed, 32x32 
//	Total size: 32 + 384 + 2048 = 2464
//
//	Time-stamp: 2026-07-22, 01:33:00
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_SUIKA_GAME_ZONE_BN_GFX_H
#define GRIT_SUIKA_GAME_ZONE_BN_GFX_H

#define suika_game_zone_bn_gfxTilesLen 384
extern const bn::tile suika_game_zone_bn_gfxTiles[12];

#define suika_game_zone_bn_gfxMapLen 2048
extern const bn::regular_bg_map_cell suika_game_zone_bn_gfxMap[1024];

#define suika_game_zone_bn_gfxPalLen 32
extern const bn::color suika_game_zone_bn_gfxPal[16];

#endif // GRIT_SUIKA_GAME_ZONE_BN_GFX_H

//}}BLOCK(suika_game_zone_bn_gfx)

namespace bn::regular_bg_items
{
    constexpr inline regular_bg_item suika_game_zone(
            regular_bg_tiles_item(span<const tile>(suika_game_zone_bn_gfxTiles, 12), bpp_mode::BPP_4, compression_type::NONE), 
            bg_palette_item(span<const color>(suika_game_zone_bn_gfxPal, 16), bpp_mode::BPP_4, compression_type::NONE),
            regular_bg_map_item(suika_game_zone_bn_gfxMap[0], size(32, 32), compression_type::NONE));
}

#endif

