
@{{BLOCK(fruit_0_bn_gfx)

@=======================================================================
@
@	fruit_0_bn_gfx, 16x16@4, 
@	+ palette 16 entries, not compressed
@	+ 4 tiles not compressed
@	Total size: 32 + 128 = 160
@
@	Time-stamp: 2026-07-08, 21:03:00
@	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
@	( http://www.coranac.com/projects/#grit )
@
@=======================================================================

	.section .rodata
	.align	2
	.global fruit_0_bn_gfxTiles		@ 128 unsigned chars
	.hidden fruit_0_bn_gfxTiles
fruit_0_bn_gfxTiles:
	.word 0x00000000,0x00000000,0x00000000,0x00000000,0x22000000,0x11200000,0x51120000,0x15120000
	.word 0x00000000,0x00000000,0x00000000,0x00000000,0x00000022,0x00000234,0x00002341,0x00002341
	.word 0x11420000,0x44320000,0x33200000,0x22000000,0x00000000,0x00000000,0x00000000,0x00000000
	.word 0x00002341,0x00002334,0x00000233,0x00000022,0x00000000,0x00000000,0x00000000,0x00000000

	.section .rodata
	.align	2
	.global fruit_0_bn_gfxPal		@ 32 unsigned chars
	.hidden fruit_0_bn_gfxPal
fruit_0_bn_gfxPal:
	.hword 0x7C1F,0x0C1C,0x0407,0x0C12,0x1859,0x4A9F,0x0000,0x0000
	.hword 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000

@}}BLOCK(fruit_0_bn_gfx)
