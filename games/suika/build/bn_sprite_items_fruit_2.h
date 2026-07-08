#ifndef BN_SPRITE_ITEMS_FRUIT_2_H
#define BN_SPRITE_ITEMS_FRUIT_2_H

#include "bn_sprite_item.h"

//{{BLOCK(fruit_2_bn_gfx)

//======================================================================
//
//	fruit_2_bn_gfx, 32x32@4, 
//	+ palette 16 entries, not compressed
//	+ 16 tiles not compressed
//	Total size: 32 + 512 = 544
//
//	Time-stamp: 2026-07-08, 21:03:00
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_FRUIT_2_BN_GFX_H
#define GRIT_FRUIT_2_BN_GFX_H

#define fruit_2_bn_gfxTilesLen 512
extern const bn::tile fruit_2_bn_gfxTiles[16];

#define fruit_2_bn_gfxPalLen 32
extern const bn::color fruit_2_bn_gfxPal[16];

#endif // GRIT_FRUIT_2_BN_GFX_H

//}}BLOCK(fruit_2_bn_gfx)

namespace bn::sprite_items
{
    constexpr inline sprite_item fruit_2(sprite_shape_size(sprite_shape::SQUARE, sprite_size::BIG), 
            sprite_tiles_item(span<const tile>(fruit_2_bn_gfxTiles, 16), bpp_mode::BPP_4, compression_type::NONE, 1), 
            sprite_palette_item(span<const color>(fruit_2_bn_gfxPal, 16), bpp_mode::BPP_4, compression_type::NONE));
}

#endif

