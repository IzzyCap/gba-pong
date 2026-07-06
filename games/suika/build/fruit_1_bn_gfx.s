
@{{BLOCK(fruit_1_bn_gfx)

@=======================================================================
@
@	fruit_1_bn_gfx, 16x16@4, 
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
	.global fruit_1_bn_gfxTiles		@ 128 unsigned chars
	.hidden fruit_1_bn_gfxTiles
fruit_1_bn_gfxTiles:
	.word 0x00000000,0x33330000,0x13333000,0x11233300,0x12223330,0x12222330,0x12221330,0x11111130
	.word 0x00000000,0x00003333,0x00033331,0x00333111,0x03331111,0x03311111,0x03311111,0x03111111
	.word 0x11111130,0x11111330,0x11111330,0x11113330,0x11133300,0x13333000,0x33330000,0x00000000
	.word 0x03111111,0x03311111,0x03311111,0x03331111,0x00333111,0x00033331,0x00003333,0x00000000

	.section .rodata
	.align	2
	.global fruit_1_bn_gfxPal		@ 32 unsigned chars
	.hidden fruit_1_bn_gfxPal
fruit_1_bn_gfxPal:
	.hword 0x7C1F,0x355D,0x565F,0x24B4,0x0000,0x0000,0x0000,0x0000
	.hword 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000

@}}BLOCK(fruit_1_bn_gfx)
