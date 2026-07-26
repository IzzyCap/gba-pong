#ifndef BN_SPRITE_ITEMS_CORRUPTED_FRUIT_3_H
#define BN_SPRITE_ITEMS_CORRUPTED_FRUIT_3_H

#include "bn_sprite_item.h"

//{{BLOCK(corrupted_fruit_3_bn_gfx)

//======================================================================
//
//	corrupted_fruit_3_bn_gfx, 32x96@4, 
//	+ palette 16 entries, not compressed
//	+ 48 tiles not compressed
//	Total size: 32 + 1536 = 1568
//
//	Time-stamp: 2026-07-26, 17:03:12
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_CORRUPTED_FRUIT_3_BN_GFX_H
#define GRIT_CORRUPTED_FRUIT_3_BN_GFX_H

#define corrupted_fruit_3_bn_gfxTilesLen 1536
extern const bn::tile corrupted_fruit_3_bn_gfxTiles[48];

#define corrupted_fruit_3_bn_gfxPalLen 32
extern const bn::color corrupted_fruit_3_bn_gfxPal[16];

#endif // GRIT_CORRUPTED_FRUIT_3_BN_GFX_H

//}}BLOCK(corrupted_fruit_3_bn_gfx)

namespace bn::sprite_items
{
    constexpr inline sprite_item corrupted_fruit_3(sprite_shape_size(sprite_shape::SQUARE, sprite_size::BIG), 
            sprite_tiles_item(span<const tile>(corrupted_fruit_3_bn_gfxTiles, 48), bpp_mode::BPP_4, compression_type::NONE, 3), 
            sprite_palette_item(span<const color>(corrupted_fruit_3_bn_gfxPal, 16), bpp_mode::BPP_4, compression_type::NONE));
}

#endif

