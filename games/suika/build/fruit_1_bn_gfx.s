
@{{BLOCK(fruit_1_bn_gfx)

@=======================================================================
@
@	fruit_1_bn_gfx, 16x16@4, 
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
	.global fruit_1_bn_gfxTiles		@ 128 unsigned chars
	.hidden fruit_1_bn_gfxTiles
fruit_1_bn_gfxTiles:
	.word 0x00000000,0x11000000,0xA9110000,0x98831000,0x73331000,0x333B3100,0x3B333100,0x33333100
	.word 0x00000000,0x00000011,0x00001189,0x0001287A,0x00014337,0x00124533,0x00143333,0x00124333
	.word 0x33B33100,0x33331000,0x53341000,0x34410000,0x42110000,0x22100000,0x11000000,0x00000000
	.word 0x00124363,0x00012333,0x00012433,0x00001243,0x00001124,0x00000122,0x00000011,0x00000000

	.section .rodata
	.align	2
	.global fruit_1_bn_gfxPal		@ 32 unsigned chars
	.hidden fruit_1_bn_gfxPal
fruit_1_bn_gfxPal:
	.hword 0x7C1F,0x0407,0x1057,0x1C9F,0x20DA,0x461F,0x5ADF,0x0E21
	.hword 0x0669,0x06ED,0x1370,0x435E,0x0000,0x0000,0x0000,0x0000

@}}BLOCK(fruit_1_bn_gfx)
