#ifndef SUIKA_HIGH_SCORES_H
#define SUIKA_HIGH_SCORES_H

#include "bn_array.h"

namespace suika
{

// Number of scores kept in the ranking table.
constexpr int HIGH_SCORE_COUNT = 5;

using high_score_table = bn::array<int, HIGH_SCORE_COUNT>;

// Reads the ranking from cartridge SRAM. When nothing has been saved yet (or
// the stored data is not ours) a zeroed table is returned.
[[nodiscard]] high_score_table load_high_scores();

// Inserts a score into the ranking and, when it places, saves the whole table
// back to SRAM. Returns the reached rank (0 is the best) or -1 when it didn't
// make the top five.
int submit_high_score(int score);

}

#endif
