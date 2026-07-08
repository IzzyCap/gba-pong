# GBA Pong

## Editing Sprites

GBA sprites must be **4bpp indexed BMP** files with **magenta (255, 0, 255) at
palette index 0** — this is the transparent color on GBA.

**Problem:** Photoshop's BMP export inserts "system colors" (black, white) at
palette indices 0–1, pushing magenta to index 2+. This breaks transparency
in-game.

**Fix:** After saving from Photoshop, run the palette fix script:

```bash
cd games/suika/graphics

# Fix a single sprite
python3 fix_palette.py fruit_1.bmp

# Fix all sprites at once
python3 fix_palette.py *.bmp
```

The script detects magenta wherever it ended up in the palette, moves it to
index 0, remaps all pixels, and outputs a correct 4bpp BMP.

**Alternative:** Use [Aseprite](https://www.aseprite.org/), GIMP, or Usenti —
these tools let you control palette index order directly and don't require
post-processing.
