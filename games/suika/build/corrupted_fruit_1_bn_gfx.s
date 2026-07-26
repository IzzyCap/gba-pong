
@{{BLOCK(corrupted_fruit_1_bn_gfx)

@=======================================================================
@
@	corrupted_fruit_1_bn_gfx, 16x16@4, 
@	+ palette 16 entries, not compressed
@	+ 4 tiles not compressed
@	Total size: 32 + 128 = 160
@
@	Time-stamp: 2026-07-26, 17:03:12
@	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
@	( http://www.coranac.com/projects/#grit )
@
@=======================================================================

	.section .rodata
	.align	2
	.global corrupted_fruit_1_bn_gfxTiles		@ 128 unsigned chars
	.hidden corrupted_fruit_1_bn_gfxTiles
corrupted_fruit_1_bn_gfxTiles:
	.word 0x00000000,0x11000000,0x76110000,0x65551000,0x45551000,0x55525100,0x52555100,0x55555100
	.word 0x00000000,0x00000011,0x00001156,0x00013547,0x00014554,0x00134855,0x00145555,0x00134555
	.word 0x55255100,0x55551000,0x85541000,0x54410000,0x43110000,0x33100000,0x11000000,0x00000000
	.word 0x00134585,0x00013555,0x00013455,0x00001345,0x00001134,0x00000133,0x00000011,0x00000000

	.section .rodata
	.align	2
	.global corrupted_fruit_1_bn_gfxPal		@ 32 unsigned chars
	.hidden corrupted_fruit_1_bn_gfxPal
corrupted_fruit_1_bn_gfxPal:
	.hword 0x7C1F,0x0000,0x7FFF,0x24E3,0x3161,0x4621,0x3AE3,0x3772
	.hword 0x6F7D,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000

@}}BLOCK(corrupted_fruit_1_bn_gfx)
