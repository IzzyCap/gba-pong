#include "high_scores.h"

#include "bn_sram.h"

namespace suika
{

namespace
{
    // SRAM starts as garbage on a fresh cartridge, so a tag lets us tell our own
    // save data apart from noise. Change it to invalidate old saves on purpose.
    constexpr char SAVE_TAG[4] = {'S', 'U', 'I', 'K'};

    struct save_data
    {
        char tag[4];
        int scores[HIGH_SCORE_COUNT];
    };

    save_data read_save()
    {
        save_data data;
        bn::sram::read(data);

        bool valid = data.tag[0] == SAVE_TAG[0] && data.tag[1] == SAVE_TAG[1] &&
                     data.tag[2] == SAVE_TAG[2] && data.tag[3] == SAVE_TAG[3];

        if(! valid)
        {
            for(int index = 0; index < 4; ++index)
            {
                data.tag[index] = SAVE_TAG[index];
            }

            for(int index = 0; index < HIGH_SCORE_COUNT; ++index)
            {
                data.scores[index] = 0;
            }
        }

        return data;
    }
}

high_score_table load_high_scores()
{
    save_data data = read_save();
    high_score_table scores;

    for(int index = 0; index < HIGH_SCORE_COUNT; ++index)
    {
        scores[index] = data.scores[index];
    }

    return scores;
}

int submit_high_score(int score)
{
    save_data data = read_save();

    int rank = -1;

    for(int index = 0; index < HIGH_SCORE_COUNT; ++index)
    {
        if(score > data.scores[index])
        {
            rank = index;
            break;
        }
    }

    if(rank >= 0)
    {
        // Shift the lower scores down one spot to make room for the new one.
        for(int index = HIGH_SCORE_COUNT - 1; index > rank; --index)
        {
            data.scores[index] = data.scores[index - 1];
        }

        data.scores[rank] = score;
        bn::sram::write(data);
    }

    return rank;
}

}
