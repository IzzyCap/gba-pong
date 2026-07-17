
@{{BLOCK(fruit_1_bn_gfx)

@=======================================================================
@
@	fruit_1_bn_gfx, 16x16@4, 
@	+ palette 16 entries, not compressed
@	+ 4 tiles not compressed
@	Total size: 32 + 128 = 160
@
@	Time-stamp: 2026-07-17, 16:54:26
@	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
@	( http://www.coranac.com/projects/#grit )
@
@=======================================================================

	.section .rodata
	.align	2
	.global fruit_1_bn_gfxTiles		@ 128 unsigned chars
	.hidden fruit_1_bn_gfxTiles
fruit_1_bn_gfxTiles:
	.word 0x00000000,0x11000000,0x87110000,0x76621000,0x52221000,0x22292100,0x29222100,0x22222100
	.word 0x00000000,0x00000011,0x00001167,0x00014658,0x00013225,0x00143A22,0x00132222,0x00143222
	.word 0x22922100,0x22221000,0xA2231000,0x23310000,0x34110000,0x44100000,0x11000000,0x00000000
	.word 0x001432A2,0x00014222,0x00014322,0x00001432,0x00001143,0x00000144,0x00000011,0x00000000

	.section .rodata
	.align	2
	.global fruit_1_bn_gfxPal		@ 32 unsigned chars
	.hidden fruit_1_bn_gfxPal
fruit_1_bn_gfxPal:
	.hword 0x7C1F,0x0000,0x3D3E,0x2898,0x2C70,0x3161,0x4621,0x3AE3
	.hword 0x3772,0x4A9F,0x421E,0x0000,0x0000,0x0000,0x0000,0x0000

@}}BLOCK(fruit_1_bn_gfx)
