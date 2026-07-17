#ifndef SUIKA_SETTINGS_H
#define SUIKA_SETTINGS_H

#include "bn_sram.h"

namespace suika
{

// Global toggles configured from the settings screen. "admin mode" has no
// gameplay effect yet; it just remembers the choice for future features.
//
// The "Admin mode" option is disabled (shown grayed out) until the player
// enters the secret combo LEFT, RIGHT, RIGHT, B, LEFT, A in the settings
// screen. Once unlocked it stays enabled for the rest of the session.
inline bool admin_unlocked = false;
inline bool admin_mode = false;

// Set when the unlock combo fires so the settings screen re-opens straight into
// the admin panel after the creepy scene plays.
inline bool admin_panel_pending = false;

// Set when Corrupted Fruits is activated so the settings screen re-opens straight
// into the Developer Tools menu after the creepy scene plays.
inline bool dev_tools_pending = false;

// Developer Tools toggle (no gameplay effect yet).
inline bool corrupted_fruits = false;

// Persistent settings stored in SRAM so the admin panel stays unlocked and the
// Corrupted Fruits toggle is remembered across power cycles. The magic value
// tells a freshly initialized (garbage) SRAM apart from real saved data.
struct settings_save_data
{
    char magic[4];
    bool admin_unlocked;
    bool corrupted_fruits;
};

inline constexpr char SETTINGS_SAVE_MAGIC[4] = { 'A', 'D', 'M', 'N' };

// SRAM byte offset for the settings block. High scores occupy the start of SRAM
// (offset 0), so the admin settings live further along to avoid clobbering them.
inline constexpr int SETTINGS_SAVE_OFFSET = 64;

// Writes the current admin_unlocked / corrupted_fruits values to SRAM.
inline void settings_save()
{
    settings_save_data data = {
        { SETTINGS_SAVE_MAGIC[0], SETTINGS_SAVE_MAGIC[1],
          SETTINGS_SAVE_MAGIC[2], SETTINGS_SAVE_MAGIC[3] },
        admin_unlocked,
        corrupted_fruits
    };

    bn::sram::write_offset(data, SETTINGS_SAVE_OFFSET);
}

// Loads persisted settings from SRAM. If the magic value doesn't match (first
// boot / uninitialized SRAM) the defaults are kept and written back.
inline void settings_load()
{
    settings_save_data data;
    bn::sram::read_offset(data, SETTINGS_SAVE_OFFSET);

    if(data.magic[0] == SETTINGS_SAVE_MAGIC[0] && data.magic[1] == SETTINGS_SAVE_MAGIC[1] &&
       data.magic[2] == SETTINGS_SAVE_MAGIC[2] && data.magic[3] == SETTINGS_SAVE_MAGIC[3])
    {
        admin_unlocked = data.admin_unlocked;
        corrupted_fruits = data.corrupted_fruits;
    }
    else
    {
        admin_unlocked = false;
        corrupted_fruits = false;
        settings_save();
    }
}

}

#endif
