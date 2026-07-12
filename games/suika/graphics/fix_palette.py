#!/usr/bin/env python3
"""Fix BMP sprites exported from Photoshop for GBA/Butano.

Ensures:
  - 4bpp indexed BMP
  - Magenta (255,0,255) at palette index 0 (= transparent on GBA)
  - Pixel indices remapped accordingly

Usage:
  python3 fix_palette.py fruit_1.bmp
  python3 fix_palette.py *.bmp          # fix all BMPs in the folder
"""

import struct
import sys
import glob


def read_bmp(path):
    with open(path, "rb") as f:
        return bytearray(f.read())


def fix_sprite(path):
    d = read_bmp(path)
    if d[0:2] != b"BM":
        print(f"  SKIP {path}: not a BMP")
        return False

    pixoff = struct.unpack_from("<I", d, 10)[0]
    dibsize = struct.unpack_from("<I", d, 14)[0]
    width = struct.unpack_from("<i", d, 18)[0]
    height = struct.unpack_from("<i", d, 22)[0]
    bpp = struct.unpack_from("<H", d, 28)[0]
    ncol = struct.unpack_from("<I", d, 46)[0] or (1 << min(bpp, 8))
    palstart = 14 + dibsize

    if bpp > 8:
        print(f"  ERROR {path}: {bpp}bpp true-color — can't auto-fix, reduce colors first")
        return False

    # Read palette
    palette = []
    for i in range(ncol):
        b, g, r, _ = d[palstart + i * 4 : palstart + i * 4 + 4]
        palette.append((r, g, b))

    # Find magenta in palette
    magenta_idx = None
    for i, (r, g, b) in enumerate(palette):
        if r == 255 and g == 0 and b == 255:
            magenta_idx = i
            break

    has_magenta = magenta_idx is not None

    if has_magenta and magenta_idx == 0 and bpp == 4:
        print(f"  OK   {path}: already correct (4bpp, magenta at idx0)")
        return False

    if not has_magenta and bpp == 4:
        # Opaque image (e.g. a full-screen background) — nothing to remap.
        print(f"  OK   {path}: already 4bpp (opaque, no magenta needed)")
        return False

    # Read all pixel indices
    rowsize = ((bpp * width + 31) // 32) * 4
    pixels = []
    for y in range(abs(height)):
        row = []
        if bpp == 8:
            for x in range(width):
                row.append(d[pixoff + y * rowsize + x])
        elif bpp == 4:
            for xb in range(rowsize):
                byte = d[pixoff + y * rowsize + xb]
                row.append(byte >> 4)
                row.append(byte & 0xF)
            row = row[:width]
        pixels.append(row)

    # Build new palette. If magenta exists it must sit at index 0 (transparent
    # on GBA). Otherwise (e.g. a full-screen background) keep the colours in
    # their original order without reserving a transparent slot.
    used_indices = set()
    for row in pixels:
        used_indices.update(row)

    new_palette = []
    old_to_new = {}
    slot = 0
    if has_magenta:
        new_palette.append(palette[magenta_idx])
        old_to_new[magenta_idx] = 0
        slot = 1
        used_indices.discard(magenta_idx)

    for old_idx in sorted(used_indices):
        if old_idx < len(palette):
            new_palette.append(palette[old_idx])
        else:
            new_palette.append((0, 0, 0))
        old_to_new[old_idx] = slot
        slot += 1

    if len(new_palette) > 16:
        print(f"  ERROR {path}: {len(new_palette)} colors used — too many for 4bpp")
        return False

    # Pad to 16 colors for 4bpp
    while len(new_palette) < 16:
        new_palette.append((0, 0, 0))

    # Remap pixels
    for y in range(len(pixels)):
        for x in range(len(pixels[y])):
            pixels[y][x] = old_to_new.get(pixels[y][x], 0)

    # Write new 4bpp BMP
    new_rowsize = ((4 * width + 31) // 32) * 4
    pixdata = bytearray()
    for y in range(abs(height)):
        rowbytes = bytearray()
        for x in range(0, width, 2):
            hi = pixels[y][x] & 0xF
            lo = pixels[y][x + 1] & 0xF if x + 1 < width else 0
            rowbytes.append((hi << 4) | lo)
        while len(rowbytes) < new_rowsize:
            rowbytes.append(0)
        pixdata += rowbytes

    palbytes = bytearray()
    for r, g, b in new_palette:
        palbytes += bytes([b, g, r, 0])

    new_pixoff = 14 + 40 + len(palbytes)
    filesize = new_pixoff + len(pixdata)

    out = bytearray()
    out += b"BM"
    out += struct.pack("<I", filesize)
    out += struct.pack("<HH", 0, 0)
    out += struct.pack("<I", new_pixoff)
    out += struct.pack("<IiiHHIIiiII", 40, width, abs(height), 1, 4, 0,
                       len(pixdata), 2835, 2835, 16, 0)
    out += palbytes
    out += pixdata

    with open(path, "wb") as f:
        f.write(out)
    if has_magenta:
        print(f"  FIXED {path}: {bpp}bpp→4bpp, magenta moved idx{magenta_idx}→idx0")
    else:
        print(f"  FIXED {path}: {bpp}bpp→4bpp (opaque, no magenta)")
    return True


def main():
    if len(sys.argv) < 2:
        print("Usage: python3 fix_palette.py <file.bmp> [file2.bmp ...]")
        print("       python3 fix_palette.py *.bmp")
        sys.exit(1)

    files = []
    for arg in sys.argv[1:]:
        files.extend(glob.glob(arg))

    if not files:
        print("No matching files found.")
        sys.exit(1)

    fixed = 0
    for path in sorted(set(files)):
        if fix_sprite(path):
            fixed += 1

    print(f"\nDone: {fixed} file(s) fixed.")


if __name__ == "__main__":
    main()
