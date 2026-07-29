#ifndef BN_SPRITE_ITEMS_CORRUPTED_FRUIT_1_H
#define BN_SPRITE_ITEMS_CORRUPTED_FRUIT_1_H

#include "bn_sprite_item.h"

//{{BLOCK(corrupted_fruit_1_bn_gfx)

//======================================================================
//
//	corrupted_fruit_1_bn_gfx, 16x16@4, 
//	+ palette 16 entries, not compressed
//	+ 4 tiles not compressed
//	Total size: 32 + 128 = 160
//
//	Time-stamp: 2026-07-26, 21:46:28
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_CORRUPTED_FRUIT_1_BN_GFX_H
#define GRIT_CORRUPTED_FRUIT_1_BN_GFX_H

#define corrupted_fruit_1_bn_gfxTilesLen 128
extern const bn::tile corrupted_fruit_1_bn_gfxTiles[4];

#define corrupted_fruit_1_bn_gfxPalLen 32
extern const bn::color corrupted_fruit_1_bn_gfxPal[16];

#endif // GRIT_CORRUPTED_FRUIT_1_BN_GFX_H

//}}BLOCK(corrupted_fruit_1_bn_gfx)

namespace bn::sprite_items
{
    constexpr inline sprite_item corrupted_fruit_1(sprite_shape_size(sprite_shape::SQUARE, sprite_size::NORMAL), 
            sprite_tiles_item(span<const tile>(corrupted_fruit_1_bn_gfxTiles, 4), bpp_mode::BPP_4, compression_type::NONE, 1), 
            sprite_palette_item(span<const color>(corrupted_fruit_1_bn_gfxPal, 16), bpp_mode::BPP_4, compression_type::NONE));
}

#endif

