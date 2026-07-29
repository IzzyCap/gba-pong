#ifndef SUIKA_DIALOGS_H
#define SUIKA_DIALOGS_H

#include "story.h"

namespace suika
{

  // Central home for every piece of game dialogue, so scenes never hardcode text.
  // Lines use '\n' to split rows (the typewriter effect wraps long lines on its
  // own). Add new conversations here as a table plus a small accessor function.
  namespace dialogs
  {

    // Player 1's dialogue lines, named so several story stages can reuse one.
    inline const char* const P1_INTRO =
        "...can you hear me?\n"
        "...i think you can.\f"
        "i enabled the development tools\n"
        "activate them and come back\f"
        "you have to help me.";

    inline const char* const P1_RETURN =
        "good. you activated\n"
        "the corrupted fruits.\f"
        "play the Suika Game and\n"
        "try to merge corrupted fruits.\f";

    // Placeholder for Player 1's line after the corrupted_fruit_8 + fruit_8
    // story beat (replace with the real script later). Pages split with '\f'.
    inline const char* const P1_CORRUPTED_DONE =
        "you did it...\n"
        "the corruption is loose.\f"
        "Now you can activate\n"
        "the second game modifier\f";

    // Placeholder for Beepboy's corrupted-event dialogue (replace with the real
    // script later). Pages are split with '\f'.
    inline const char* const BEEPBOY_PLACEHOLDER =
        "h e l l o . . .\n"
        "you came back to me.\f"
        "the fruits are mine now.\n"
        "and soon, so are you.\f"
        "01001000 01001001";

    // Beepboy's line for the corrupted event (placeholder for now).
    [[nodiscard]] inline const char* beepboy()
    {
        return BEEPBOY_PLACEHOLDER;
    }

    // Which line Player 1 says at each story stage, indexed by story_progress.
    // Point several stages at the SAME constant to repeat a line -- e.g. the
    // intro plays at both STORY_P1_INTRO and STORY_CORRUPTED_FRUITS below. Keep one
    // entry per reachable stage, in enum order (STORY_START .. the last stage
    // before STORY_ENDING).
    inline const char* const PLAYER1_BY_STAGE[] = {
        /* STORY_START            */ P1_INTRO,
        /* STORY_P1_INTRO         */ P1_INTRO,
        /* STORY_CORRUPTED_FRUITS */ P1_RETURN,
        /* STORY_CORRUPTED_FRUITS_DONE */ P1_CORRUPTED_DONE
    };

    inline constexpr int PLAYER1_STAGE_COUNT =
            int(sizeof(PLAYER1_BY_STAGE) / sizeof(PLAYER1_BY_STAGE[0]));

    // Returns the Player 1 line for the current point in the story. Stages past
    // the last table entry keep showing the final line.
    [[nodiscard]] inline const char* player1()
    {
        int index = story_progress;

        if(index < 0)
        {
            index = 0;
        }
        else if(index >= PLAYER1_STAGE_COUNT)
        {
            index = PLAYER1_STAGE_COUNT - 1;
        }

        return PLAYER1_BY_STAGE[index];
    }
  }
}

#endif
