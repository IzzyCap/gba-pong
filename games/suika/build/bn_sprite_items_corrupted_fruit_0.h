#ifndef BN_SPRITE_ITEMS_CORRUPTED_FRUIT_0_H
#define BN_SPRITE_ITEMS_CORRUPTED_FRUIT_0_H

#include "bn_sprite_item.h"

//{{BLOCK(corrupted_fruit_0_bn_gfx)

//======================================================================
//
//	corrupted_fruit_0_bn_gfx, 16x64@4, 
//	+ palette 16 entries, not compressed
//	+ 16 tiles not compressed
//	Total size: 32 + 512 = 544
//
//	Time-stamp: 2026-07-23, 02:11:15
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_CORRUPTED_FRUIT_0_BN_GFX_H
#define GRIT_CORRUPTED_FRUIT_0_BN_GFX_H

#define corrupted_fruit_0_bn_gfxTilesLen 512
extern const bn::tile corrupted_fruit_0_bn_gfxTiles[16];

#define corrupted_fruit_0_bn_gfxPalLen 32
extern const bn::color corrupted_fruit_0_bn_gfxPal[16];

#endif // GRIT_CORRUPTED_FRUIT_0_BN_GFX_H

//}}BLOCK(corrupted_fruit_0_bn_gfx)

namespace bn::sprite_items
{
    constexpr inline sprite_item corrupted_fruit_0(sprite_shape_size(sprite_shape::SQUARE, sprite_size::NORMAL), 
            sprite_tiles_item(span<const tile>(corrupted_fruit_0_bn_gfxTiles, 16), bpp_mode::BPP_4, compression_type::NONE, 4), 
            sprite_palette_item(span<const color>(corrupted_fruit_0_bn_gfxPal, 16), bpp_mode::BPP_4, compression_type::NONE));
}

#endif

