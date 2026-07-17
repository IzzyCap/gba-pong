#ifndef BN_SPRITE_ITEMS_DROP_LINE_H
#define BN_SPRITE_ITEMS_DROP_LINE_H

#include "bn_sprite_item.h"

//{{BLOCK(drop_line_bn_gfx)

//======================================================================
//
//	drop_line_bn_gfx, 8x256@4, 
//	+ palette 16 entries, not compressed
//	+ 32 tiles not compressed
//	Total size: 32 + 1024 = 1056
//
//	Time-stamp: 2026-07-17, 16:54:26
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_DROP_LINE_BN_GFX_H
#define GRIT_DROP_LINE_BN_GFX_H

#define drop_line_bn_gfxTilesLen 1024
extern const bn::tile drop_line_bn_gfxTiles[32];

#define drop_line_bn_gfxPalLen 32
extern const bn::color drop_line_bn_gfxPal[16];

#endif // GRIT_DROP_LINE_BN_GFX_H

//}}BLOCK(drop_line_bn_gfx)

namespace bn::sprite_items
{
    constexpr inline sprite_item drop_line(sprite_shape_size(sprite_shape::TALL, sprite_size::NORMAL), 
            sprite_tiles_item(span<const tile>(drop_line_bn_gfxTiles, 32), bpp_mode::BPP_4, compression_type::NONE, 8), 
            sprite_palette_item(span<const color>(drop_line_bn_gfxPal, 16), bpp_mode::BPP_4, compression_type::NONE));
}

#endif

