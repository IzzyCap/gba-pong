
@{{BLOCK(fruit_0_bn_gfx)

@=======================================================================
@
@	fruit_0_bn_gfx, 16x16@4, 
@	+ palette 16 entries, not compressed
@	+ 4 tiles not compressed
@	Total size: 32 + 128 = 160
@
@	Time-stamp: 2026-07-06, 21:48:37
@	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
@	( http://www.coranac.com/projects/#grit )
@
@=======================================================================

	.section .rodata
	.align	2
	.global fruit_0_bn_gfxTiles		@ 128 unsigned chars
	.hidden fruit_0_bn_gfxTiles
fruit_0_bn_gfxTiles:
	.word 0x00000000,0x00000000,0x00000000,0x33300000,0x13330000,0x12233000,0x22233000,0x12113000
	.word 0x00000000,0x00000000,0x00000000,0x00000333,0x00003331,0x00033111,0x00033111,0x00031111
	.word 0x11113000,0x11133000,0x11133000,0x13330000,0x33300000,0x00000000,0x00000000,0x00000000
	.word 0x00031111,0x00033111,0x00033111,0x00003331,0x00000333,0x00000000,0x00000000,0x00000000

	.section .rodata
	.align	2
	.global fruit_0_bn_gfxPal		@ 32 unsigned chars
	.hidden fruit_0_bn_gfxPal
fruit_0_bn_gfxPal:
	.hword 0x7C1F,0x18DB,0x3DFF,0x1472,0x0000,0x0000,0x0000,0x0000
	.hword 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000

@}}BLOCK(fruit_0_bn_gfx)
