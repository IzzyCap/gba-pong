
@{{BLOCK(fruit_0_bn_gfx)

@=======================================================================
@
@	fruit_0_bn_gfx, 16x16@4, 
@	+ palette 16 entries, not compressed
@	+ 4 tiles not compressed
@	Total size: 32 + 128 = 160
@
@	Time-stamp: 2026-07-13, 16:55:09
@	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
@	( http://www.coranac.com/projects/#grit )
@
@=======================================================================

	.section .rodata
	.align	2
	.global fruit_0_bn_gfxTiles		@ 128 unsigned chars
	.hidden fruit_0_bn_gfxTiles
fruit_0_bn_gfxTiles:
	.word 0x00000000,0x00000000,0x00000000,0x00000000,0x11000000,0x44100000,0x34410000,0x43410000
	.word 0x00000000,0x00000000,0x00000000,0x00000000,0x00000011,0x00000125,0x00001254,0x00001254
	.word 0x44510000,0x55210000,0x22100000,0x11000000,0x00000000,0x00000000,0x00000000,0x00000000
	.word 0x00001254,0x00001225,0x00000122,0x00000011,0x00000000,0x00000000,0x00000000,0x00000000

	.section .rodata
	.align	2
	.global fruit_0_bn_gfxPal		@ 32 unsigned chars
	.hidden fruit_0_bn_gfxPal
fruit_0_bn_gfxPal:
	.hword 0x7C1F,0x0000,0x2C70,0x2AFF,0x0DBF,0x1CFD,0x0000,0x0000
	.hword 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000

@}}BLOCK(fruit_0_bn_gfx)
