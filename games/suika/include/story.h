#ifndef SUIKA_STORY_H
#define SUIKA_STORY_H

#include "bn_sram.h"

namespace suika
{

// The player's position in the hidden storyline, kept as a single number so it
// is trivial to save and to branch on. Read `story_progress` to decide which
// story-gated content to show (for example, which text Player 1 says); use
// story_advance() / story_set_progress() to change it so the value is persisted.
//
// Add new beats to the end of this enum as the story grows. Never reorder or
// remove existing values: the raw number is written to SRAM, so changing the
// meaning of a value would misread existing saves.
enum story_stage
{
    STORY_START = 0,        // first, tentative contact with Player 1
    STORY_P1_INTRO,         // Player 1 has introduced themselves
    STORY_P1_WARNING,
    STORY_CORRUPTED_FRUITS, // the player activated the Corrupted Fruits dev tool
    STORY_ENDING,             // the story has reached its final beat (no more content)
};

// Current point in the history. Defaults to the very start until SRAM is loaded.
inline int story_progress = STORY_START;

// Persistent storage for the story position. It lives in its own SRAM block,
// separate from the high scores (offset 0) and the admin settings (offset 64),
// so extending the story never disturbs those layouts. The magic value tells a
// freshly initialized (garbage) SRAM apart from a real save.
struct story_save_data
{
    char magic[4];
    int progress;
};

inline constexpr char STORY_SAVE_MAGIC[4] = { 'S', 'T', 'R', 'Y' };

inline constexpr int STORY_SAVE_OFFSET = 128;

// Clamps a raw stage number into the valid range.
[[nodiscard]] inline int story_clamp(int stage)
{
    if(stage < STORY_START)
    {
        return STORY_START;
    }

    if(stage >= STORY_ENDING)
    {
        return STORY_ENDING - 1;
    }

    return stage;
}

// Writes the current story_progress to SRAM.
inline void story_save()
{
    story_save_data data = {
        { STORY_SAVE_MAGIC[0], STORY_SAVE_MAGIC[1],
          STORY_SAVE_MAGIC[2], STORY_SAVE_MAGIC[3] },
        story_progress
    };

    bn::sram::write_offset(data, STORY_SAVE_OFFSET);
}

// Loads the story position from SRAM. If the magic value doesn't match (first
// boot / uninitialized SRAM) the default (STORY_START) is kept and written back.
inline void story_load()
{
    story_save_data data;
    bn::sram::read_offset(data, STORY_SAVE_OFFSET);

    if(data.magic[0] == STORY_SAVE_MAGIC[0] && data.magic[1] == STORY_SAVE_MAGIC[1] &&
       data.magic[2] == STORY_SAVE_MAGIC[2] && data.magic[3] == STORY_SAVE_MAGIC[3])
    {
        story_progress = story_clamp(data.progress);
    }
    else
    {
        story_progress = STORY_START;
        story_save();
    }
}

// Sets the story to an explicit stage (clamped) and persists it.
inline void story_set_progress(int stage)
{
    story_progress = story_clamp(stage);
    story_save();
}

// Advances one stage forward (never past the last) and persists it. Only moves
// forward, so re-triggering an earlier event won't rewind the story.
inline void story_advance()
{
    if(story_progress < STORY_ENDING - 1)
    {
        ++story_progress;
        story_save();
    }
}

}

#endif
