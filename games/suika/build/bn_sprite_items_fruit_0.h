#ifndef BN_SPRITE_ITEMS_FRUIT_0_H
#define BN_SPRITE_ITEMS_FRUIT_0_H

#include "bn_sprite_item.h"

//{{BLOCK(fruit_0_bn_gfx)

//======================================================================
//
//	fruit_0_bn_gfx, 16x16@4, 
//	+ palette 16 entries, not compressed
//	+ 4 tiles not compressed
//	Total size: 32 + 128 = 160
//
//	Time-stamp: 2026-07-13, 16:55:09
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_FRUIT_0_BN_GFX_H
#define GRIT_FRUIT_0_BN_GFX_H

#define fruit_0_bn_gfxTilesLen 128
extern const bn::tile fruit_0_bn_gfxTiles[4];

#define fruit_0_bn_gfxPalLen 32
extern const bn::color fruit_0_bn_gfxPal[16];

#endif // GRIT_FRUIT_0_BN_GFX_H

//}}BLOCK(fruit_0_bn_gfx)

namespace bn::sprite_items
{
    constexpr inline sprite_item fruit_0(sprite_shape_size(sprite_shape::SQUARE, sprite_size::NORMAL), 
            sprite_tiles_item(span<const tile>(fruit_0_bn_gfxTiles, 4), bpp_mode::BPP_4, compression_type::NONE, 1), 
            sprite_palette_item(span<const color>(fruit_0_bn_gfxPal, 16), bpp_mode::BPP_4, compression_type::NONE));
}

#endif

