#!/usr/bin/env python3
"""Generates the fruit sprites and container background for the Suika game.

Butano imports 4bpp (16 colour) indexed BMP files where palette index 0 is the
transparent colour. This script writes those BMPs directly (no Pillow needed).

Run from this folder:  python3 generate.py
"""

import math
import struct

TRANSPARENT = (255, 0, 255)  # palette index 0 (never drawn -> transparent)


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


def write_json(path, text):
    with open(path, 'w') as f:
        f.write(text)


def make_fruit(canvas, r, main, hl, sh, stripe=None):
    palette = [TRANSPARENT, main, hl, sh]
    if stripe is not None:
        palette.append(stripe)
    stripe_idx = 4 if stripe is not None else None

    px = [[0] * canvas for _ in range(canvas)]
    c = (canvas - 1) / 2.0
    hoff = r * 0.38
    hrad = r * 0.30
    for y in range(canvas):
        for x in range(canvas):
            dx = x - c
            dy = y - c
            d = math.hypot(dx, dy)
            if d <= r + 0.5:
                if d >= r - 1.3:
                    px[y][x] = 3  # dark rim
                else:
                    idx = 1
                    # soft highlight in the upper-left
                    if math.hypot(x - (c - hoff), y - (c - hoff)) <= hrad:
                        idx = 2
                    elif stripe_idx is not None and (x % 7) < 3:
                        idx = stripe_idx
                    px[y][x] = idx
    return palette, px


# canvas, radius, main, highlight, shadow, [stripe]
FRUITS = [
    (16, 5,  (220, 50, 50),   (255, 120, 120), (150, 30, 40)),                  # 0 cherry
    (16, 7,  (235, 80, 110),  (255, 150, 175), (160, 45, 75)),                  # 1 strawberry
    (32, 10, (150, 90, 205),  (195, 145, 235), (95, 55, 145)),                  # 2 grape
    (32, 13, (250, 175, 60),  (255, 215, 130), (190, 120, 30)),                 # 3 dekopon
    (32, 15, (240, 120, 45),  (255, 175, 110), (180, 80, 25)),                  # 4 persimmon
    (64, 19, (225, 50, 60),   (255, 120, 120), (150, 30, 40)),                  # 5 apple
    (64, 22, (200, 220, 95),  (230, 245, 160), (140, 160, 55)),                 # 6 pear
    (64, 25, (255, 180, 170), (255, 220, 210), (210, 130, 120)),                # 7 peach
    (64, 28, (245, 215, 75),  (255, 235, 150), (190, 160, 35)),                 # 8 pineapple
    (64, 30, (150, 215, 120), (195, 245, 165), (100, 160, 80)),                 # 9 melon
    (64, 31, (65, 155, 75),   (110, 200, 110), (30, 100, 45), (35, 110, 55)),   # 10 watermelon
]


def main():
    for i, spec in enumerate(FRUITS):
        canvas, r = spec[0], spec[1]
        main_c, hl, sh = spec[2], spec[3], spec[4]
        stripe = spec[5] if len(spec) > 5 else None
        palette, px = make_fruit(canvas, r, main_c, hl, sh, stripe)
        write_bmp_4bpp('fruit_%d.bmp' % i, canvas, canvas, palette, px)
        write_json('fruit_%d.json' % i, '{\n    "type": "sprite",\n    "height": %d\n}\n' % canvas)
        print('fruit_%d: %dx%d r=%d' % (i, canvas, canvas, r))

    make_background()


def make_background():
    W = H = 256
    palette = [
        TRANSPARENT,       # 0 unused
        (22, 26, 48),      # 1 outer background
        (95, 110, 165),    # 2 wall
        (210, 70, 80),     # 3 danger line
        (34, 40, 68),      # 4 inner play area
        (60, 72, 120),     # 5 wall inner edge
    ]
    px = [[1] * W for _ in range(H)]

    def fill(x0, y0, x1, y1, idx):
        for y in range(max(0, y0), min(H, y1)):
            for x in range(max(0, x0), min(W, x1)):
                px[y][x] = idx

    # screen->bg: bg = screen + 128
    # inner play area  (screen x [-58,58], y [-70,70])
    fill(70, 58, 186, 198, 4)
    # walls
    fill(66, 54, 70, 202, 2)
    fill(186, 54, 190, 202, 2)
    # floor
    fill(66, 198, 190, 202, 2)
    # inner wall edges
    fill(70, 58, 71, 198, 5)
    fill(185, 58, 186, 198, 5)
    # danger line (dashed, screen y = -56 -> bg 72)
    for x in range(72, 184):
        if (x // 6) % 2 == 0:
            px[72][x] = 3
            px[73][x] = 3

    write_bmp_4bpp('suika_bg.bmp', W, H, palette, px)
    write_json('suika_bg.json', '{\n    "type": "regular_bg"\n}\n')
    print('suika_bg: %dx%d' % (W, H))


if __name__ == '__main__':
    main()
