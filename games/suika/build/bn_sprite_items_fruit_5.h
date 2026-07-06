#ifndef BN_SPRITE_ITEMS_FRUIT_5_H
#define BN_SPRITE_ITEMS_FRUIT_5_H

#include "bn_sprite_item.h"

//{{BLOCK(fruit_5_bn_gfx)

//======================================================================
//
//	fruit_5_bn_gfx, 64x64@4, 
//	+ palette 16 entries, not compressed
//	+ 64 tiles not compressed
//	Total size: 32 + 2048 = 2080
//
//	Time-stamp: 2026-07-06, 21:48:37
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_FRUIT_5_BN_GFX_H
#define GRIT_FRUIT_5_BN_GFX_H

#define fruit_5_bn_gfxTilesLen 2048
extern const bn::tile fruit_5_bn_gfxTiles[64];

#define fruit_5_bn_gfxPalLen 32
extern const bn::color fruit_5_bn_gfxPal[16];

#endif // GRIT_FRUIT_5_BN_GFX_H

//}}BLOCK(fruit_5_bn_gfx)

namespace bn::sprite_items
{
    constexpr inline sprite_item fruit_5(sprite_shape_size(sprite_shape::SQUARE, sprite_size::HUGE), 
            sprite_tiles_item(span<const tile>(fruit_5_bn_gfxTiles, 64), bpp_mode::BPP_4, compression_type::NONE, 1), 
            sprite_palette_item(span<const color>(fruit_5_bn_gfxPal, 16), bpp_mode::BPP_4, compression_type::NONE));
}

#endif

