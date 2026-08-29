#!/usr/bin/env python3
"""Generates number_bg: the red binary (0/1) background shown through the
silhouette of the "bugged" fruits (the third developer tool).

It is a full 256x256 4bpp regular background whose whole surface is opaque, so
that when it is masked to a bugged fruit's sprite window the fruit body is filled
edge to edge with streaming red digits. The game scrolls it vertically so the
numbers appear to change/flow.

Butano imports 4bpp (16 colour) indexed BMP files; palette index 0 is the
transparent colour (unused here, the surface is fully opaque). This script writes
the BMP directly, matching graphics/generate.py (no Pillow needed).

Run from this folder:  python generate_number_bg.py
"""

import random
import struct

# Palette (index 0 is the transparent colour Butano never draws; unused here).
TRANSPARENT = (255, 0, 255)   # 0
DARK_RED = (32, 0, 0)         # 1: fruit body fill between digits
BRIGHT_RED = (255, 0, 0)      # 2: lit digit
DIM_RED = (140, 0, 0)         # 3: dim digit
PALETTE = [TRANSPARENT, DARK_RED, BRIGHT_RED, DIM_RED]

WIDTH = 256
HEIGHT = 256
TILE = 8

# 8x8 glyphs: '.' = body fill (index 1), '#' = digit pixel (index 2 or 3).
GLYPH_0 = [
    "..####..",
    ".#....#.",
    ".#....#.",
    ".#....#.",
    ".#....#.",
    ".#....#.",
    "..####..",
    "........",
]
GLYPH_1 = [
    "...##...",
    "..#.#...",
    "....#...",
    "....#...",
    "....#...",
    "....#...",
    "..####..",
    "........",
]


def write_bmp_4bpp(path, width, height, palette, pixels):
    """palette: list of (r,g,b) (index 0 first). pixels: pixels[y][x] -> index."""
    assert width % 8 == 0 and height % 8 == 0
    pal = list(palette)
    while len(pal) < 16:
        pal.append((0, 0, 0))

    row_bytes = ((width + 1) // 2 + 3) & ~3  # 4bpp packed rows padded to 4 bytes
    pixel_data_size = row_bytes * height
    pixels_offset = 14 + 40 + 16 * 4
    filesize = pixels_offset + pixel_data_size

    with open(path, 'wb') as f:
        # BITMAPFILEHEADER
        f.write(b'BM')
        f.write(struct.pack('<I', filesize))
        f.write(struct.pack('<HH', 0, 0))
        f.write(struct.pack('<I', pixels_offset))
        # BITMAPINFOHEADER
        f.write(struct.pack('<I', 40))
        f.write(struct.pack('<i', width))
        f.write(struct.pack('<i', height))
        f.write(struct.pack('<H', 1))
        f.write(struct.pack('<H', 4))
        f.write(struct.pack('<I', 0))
        f.write(struct.pack('<I', pixel_data_size))
        f.write(struct.pack('<i', 2835))
        f.write(struct.pack('<i', 2835))
        f.write(struct.pack('<I', 16))
        f.write(struct.pack('<I', 0))
        # palette (BGRA)
        for (r, g, b) in pal:
            f.write(struct.pack('<BBBB', b, g, r, 0))
        # pixel rows, bottom-up
        for y in range(height - 1, -1, -1):
            row = pixels[y]
            out = bytearray()
            for x in range(0, width, 2):
                hi = row[x] & 0xF
                lo = (row[x + 1] & 0xF) if (x + 1) < width else 0
                out.append((hi << 4) | lo)
            while len(out) < row_bytes:
                out.append(0)
            f.write(out)


def blit_glyph(px, tx, ty, glyph, digit_index):
    for gy in range(TILE):
        row = glyph[gy]
        for gx in range(TILE):
            px[ty * TILE + gy][tx * TILE + gx] = digit_index if row[gx] == '#' else 1


def main():
    rng = random.Random(0xB0BB1E)  # fixed seed -> reproducible art

    px = [[1] * WIDTH for _ in range(HEIGHT)]  # start fully filled with dark red
    tiles_x = WIDTH // TILE
    tiles_y = HEIGHT // TILE

    for ty in range(tiles_y):
        for tx in range(tiles_x):
            roll = rng.random()
            if roll < 0.20:
                continue  # blank body cell, keeps the field from feeling solid
            glyph = GLYPH_1 if rng.random() < 0.5 else GLYPH_0
            digit_index = 2 if rng.random() < 0.55 else 3  # bright vs dim flicker
            blit_glyph(px, tx, ty, glyph, digit_index)

    write_bmp_4bpp('number_bg.bmp', WIDTH, HEIGHT, PALETTE, px)
    with open('number_bg.json', 'w') as f:
        f.write('{\n    "type": "regular_bg"\n}\n')
    print('number_bg: %dx%d 4bpp' % (WIDTH, HEIGHT))


if __name__ == '__main__':
    main()
