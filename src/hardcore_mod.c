/**
 * Hardcore Mode for Majora's Mask: Recompiled
 * Tracks deaths persistently across sessions with tiered achievements
 * Uses ProxyRecomp_KV for save-slot-specific persistent storage
 */

#include "modding.h"
#include "global.h"
#include "recomputils.h"
#include "recompconfig.h"
#include "recompui.h"

// Import ProxyRecomp_KV functions for persistent storage
RECOMP_IMPORT("ProxyMM_KV", bool KV_Slot_Set_U32(const char* key, u32 value));
RECOMP_IMPORT("ProxyMM_KV", u32 KV_Slot_Get_U32(const char* key, u32 defaultValue));
RECOMP_IMPORT("ProxyMM_KV", bool KV_Slot_Has(const char* key));

// Storage key for death count
#define DEATH_COUNT_KEY "mm_hardcore.deaths"

// Global state
static u32 g_death_count = 0;
static int g_initialized = 0;
static int g_ui_initialized = 0;
static int g_data_loaded = 0;  // Track if we've loaded save data

// UI state - minimal test
static RecompuiContext g_ui_context = RECOMPUI_NULL_CONTEXT;
static RecompuiResource g_ui_test_label = RECOMPUI_NULL_RESOURCE;

// Config option IDs (must match mod.toml)
#define CONFIG_BLOCK_FAIRIES "block_fairies"
#define CONFIG_SHOW_HUD "show_hud"

/**
 * Check if a config option is enabled
 */
static int is_config_enabled(const char* option_id) {
    u32 value = recomp_get_config_u32(option_id);
    return value == 1;  // "Enabled" is second option, index 1
}

/**
 * Save current death count to persistent storage
 */
static void save_death_count(void) {
    if (!g_initialized) return;
    
    bool success = KV_Slot_Set_U32(DEATH_COUNT_KEY, g_death_count);
    if (success) {
        recomp_printf("Hardcore Mode: Saved death count: %u\n", g_death_count);
    } else {
        recomp_printf("Hardcore Mode: Failed to save death count!\n");
    }
}

/**
 * Load death count from persistent storage
 */
static void load_death_count(void) {
    // KV_Slot_Get_U32 returns the value if it exists, or the default (0) if it doesn't
    g_death_count = KV_Slot_Get_U32(DEATH_COUNT_KEY, 0);
    
    if (g_death_count > 0) {
        recomp_printf("Hardcore Mode: Loaded %u death(s) for this save.\n", g_death_count);
    } else {
        recomp_printf("Hardcore Mode: Starting fresh run with 0 deaths.\n");
    }
}

/**
 * Increment death counter
 */
static void increment_death_count(void) {
    g_death_count++;
    recomp_printf("Hardcore Mode: Death #%u recorded.\n", g_death_count);
    save_death_count();
}

/**
 * Decrement death counter (DEBUG only)
 */
static void decrement_death_count(void) {
    if (g_death_count > 0) {
        g_death_count--;
        recomp_printf("Hardcore Mode: Death count decreased to %u.\n", g_death_count);
        save_death_count();
    } else {
        recomp_printf("Hardcore Mode: Death count already at 0, cannot decrement.\n");
    }
}

/**
 * Simple integer to string converter
 * Returns pointer to static buffer (not thread-safe but fine for single-threaded game)
 */
static char* int_to_str(u32 value) {
    static char buffer[16];
    int i = 14;
    buffer[15] = '\0';
    
    if (value == 0) {
        buffer[14] = '0';
        return &buffer[14];
    }
    
    while (value > 0 && i >= 0) {
        buffer[i--] = '0' + (value % 10);
        value /= 10;
    }
    
    return &buffer[i + 1];
}

/**
 * Build death count text string
 */
static void build_death_text(char* dest, u32 deaths) {
    const char* prefix = "Deaths: ";
    int i = 0;
    
    // Copy prefix
    while (prefix[i]) {
        dest[i] = prefix[i];
        i++;
    }
    
    // Append number
    char* num_str = int_to_str(deaths);
    int j = 0;
    while (num_str[j]) {
        dest[i++] = num_str[j++];
    }
    
    dest[i] = '\0';
}

/**
 * Build medal text string
 */
static void build_medal_text(char* dest, const char* medal_name) {
    int i = 0;
    
    // Copy medal name
    while (medal_name[i]) {
        dest[i] = medal_name[i];
        i++;
    }
    
    // Append " Medal"
    const char* suffix = " Medal";
    int j = 0;
    while (suffix[j]) {
        dest[i++] = suffix[j++];
    }
    
    dest[i] = '\0';
}

/**
 * Get medal tier for current death count
 * Returns: 0 = none, 1 = bronze, 2 = silver, 3 = gold
 */
static int get_medal_tier(void) {
    if (g_death_count == 0) return 3;      // Gold
    if (g_death_count <= 5) return 2;      // Silver
    if (g_death_count <= 10) return 1;     // Bronze
    return 0;                               // No medal
}

/**
 * Create minimal test UI - just one label
 */
static void create_test_ui(void) {
    recomp_printf("Hardcore Mode: Creating test UI...\n");
    
    // Step 1: Create context
    g_ui_context = recompui_create_context();
    if (g_ui_context == RECOMPUI_NULL_CONTEXT) {
        recomp_printf("Hardcore Mode: ERROR - Failed to create UI context!\n");
        return;
    }
    recomp_printf("Hardcore Mode: UI context created\n");
    
    // IMPORTANT: Disable input capture so the game can still receive input
    recompui_set_context_captures_input(g_ui_context, 0);
    recompui_set_context_captures_mouse(g_ui_context, 0);
    recomp_printf("Hardcore Mode: Input capture disabled\n");
    
    // Step 2: OPEN context (makes it current for editing)
    recompui_open_context(g_ui_context);
    recomp_printf("Hardcore Mode: UI context opened\n");
    
    // Step 3: Create a simple label
    RecompuiResource root = recompui_context_root(g_ui_context);
    g_ui_test_label = recompui_create_label(g_ui_context, root, "Deaths: 0", LABELSTYLE_LARGE);
    
    // Position in top-right corner
    recompui_set_position(g_ui_test_label, POSITION_ABSOLUTE);
    recompui_set_top(g_ui_test_label, 20.0f, UNIT_DP);
    recompui_set_right(g_ui_test_label, 20.0f, UNIT_DP);
    
    // White text
    RecompuiColor white = { 255, 255, 255, 255 };
    recompui_set_color(g_ui_test_label, &white);
    
    // Semi-transparent black background
    RecompuiColor bg = { 0, 0, 0, 180 };
    recompui_set_background_color(g_ui_test_label, &bg);
    recompui_set_padding(g_ui_test_label, 10.0f, UNIT_DP);
    
    recomp_printf("Hardcore Mode: UI label created\n");
    
    // Step 4: CLOSE context (done editing)
    recompui_close_context(g_ui_context);
    recomp_printf("Hardcore Mode: UI context closed\n");
    
    // Step 5: Show the context
    recompui_show_context(g_ui_context);
    recomp_printf("Hardcore Mode: UI context shown - UI should be visible!\n");
    
    g_ui_initialized = 1;
}

/**
 * Update the test UI label with current death count
 */
static void update_test_ui(void) {
    if (!g_ui_initialized || g_ui_test_label == RECOMPUI_NULL_RESOURCE) {
        return;
    }
    
    // Open context for editing
    recompui_open_context(g_ui_context);
    
    char text[64];
    build_death_text(text, g_death_count);
    recompui_set_text(g_ui_test_label, text);
    recomp_printf("Hardcore Mode: UI updated: %s\n", text);
    
    // Close context when done editing
    recompui_close_context(g_ui_context);
}

/**
 * Get medal name based on tier
 */
static const char* get_medal_name(int tier) {
    const char* names[] = { "No Medal", "Bronze", "Silver", "Gold" };
    return names[tier];
}

/* UI functions disabled temporarily - requires proper UI context initialization
 
/**
 * Create and setup the HUD UI
 *
static void setup_hud_ui(void) {
    // Create UI context
    g_ui_context = recompui_create_context();
    if (g_ui_context == RECOMPUI_NULL_CONTEXT) {
        recomp_printf("Hardcore Mode: Failed to create UI context!\n");
        return;
    }
    
    // Create container for HUD elements
    g_ui_container = recompui_create_element(g_ui_context, RECOMPUI_NULL_RESOURCE);
    
    // Position container in top-right corner
    recompui_set_position(g_ui_container, POSITION_ABSOLUTE);
    recompui_set_top(g_ui_container, 20.0f, UNIT_DP);
    recompui_set_right(g_ui_container, 20.0f, UNIT_DP);
    recompui_set_display(g_ui_container, DISPLAY_FLEX);
    recompui_set_flex_direction(g_ui_container, FLEX_DIRECTION_COLUMN);
    recompui_set_align_items(g_ui_container, ALIGN_ITEMS_FLEX_END);
    
    // Create death count label
    g_ui_death_label = recompui_create_label(g_ui_context, g_ui_container, "Deaths: 0", LABELSTYLE_NORMAL);
    recompui_set_font_size(g_ui_death_label, 20.0f, UNIT_DP);
    
    RecompuiColor white = { 255, 255, 255, 255 };
    recompui_set_color(g_ui_death_label, &white);
    
    // Semi-transparent black background for readability
    RecompuiColor bg_color = { 0, 0, 0, 160 };
    recompui_set_background_color(g_ui_death_label, &bg_color);
    recompui_set_padding(g_ui_death_label, 8.0f, UNIT_DP);
    recompui_set_border_radius(g_ui_death_label, 4.0f, UNIT_DP);
    
    // Create medal tier label
    g_ui_medal_label = recompui_create_label(g_ui_context, g_ui_container, "Gold Medal", LABELSTYLE_NORMAL);
    recompui_set_font_size(g_ui_medal_label, 16.0f, UNIT_DP);
    recompui_set_margin_top(g_ui_medal_label, 4.0f, UNIT_DP);
    
    RecompuiColor gold = { 255, 215, 0, 255 };
    recompui_set_color(g_ui_medal_label, &gold);
    recompui_set_background_color(g_ui_medal_label, &bg_color);
    recompui_set_padding(g_ui_medal_label, 6.0f, UNIT_DP);
    recompui_set_border_radius(g_ui_medal_label, 4.0f, UNIT_DP);
    
    // Show the UI context
    recompui_show_context(g_ui_context);
    
    recomp_printf("Hardcore Mode: HUD UI created successfully\n");
}

/**
 * Update HUD display with current death count and medal
 *
static void update_hud_ui(void) {
    if (g_ui_death_label == RECOMPUI_NULL_RESOURCE) {
        return; // UI not initialized
    }
    
    // Update death count text
    char death_text[64];
    build_death_text(death_text, g_death_count);
    recompui_set_text(g_ui_death_label, death_text);
    
    // Update medal tier
    int tier = get_medal_tier();
    const char* medal_name = get_medal_name(tier);
    RecompuiColor medal_color = get_medal_color(tier);
    
    char medal_text[64];
    build_medal_text(medal_text, medal_name);
    recompui_set_text(g_ui_medal_label, medal_text);
    recompui_set_color(g_ui_medal_label, &medal_color);
}
*/

/**
 * Initialization callback - called once when mod loads
 */
RECOMP_CALLBACK("*", recomp_on_init) void hardcore_init(void) {
    recomp_printf("=== Hardcore Mode v1.0 - UI TEST ===\n");
    recomp_printf("Hardcore Mode: Initializing...\n");
    
    // Don't load death count here - save slot not loaded yet!
    // Will load on first gameplay update
    
    g_initialized = 1;
    
    recomp_printf("Hardcore Mode: Ready! Death tracking enabled.\n");
    recomp_printf("Hardcore Mode: Death count will be loaded when save file loads.\n");
}

/**
 * Health_ChangeBy return hook - called after player health changes
 * This is where we detect deaths
 */
RECOMP_HOOK_RETURN("Health_ChangeBy") void hardcore_check_death(void) {
    // Log all health changes for debugging
    s16 health = gSaveContext.save.saveInfo.playerData.health;
    recomp_printf("Hardcore Mode: Health changed to %d\n", health);
    
    // Check if player died (health reached 0 or below)
    if (health <= 0) {
        // Death detected!
        recomp_printf("Hardcore Mode: Death detected! Health = %d\n", health);
        if (g_initialized) {
            increment_death_count();
            int tier = get_medal_tier();
            recomp_printf("Hardcore Mode: New medal tier: %s\n", get_medal_name(tier));
            update_test_ui();
        }
    }
}

// Note: Fairy blocking feature is commented out for now
// RECOMP_PATCH requires full reimplementation of the function
// which is complex. This can be added in a future version.
//
// RECOMP_PATCH s32 Inventory_ConsumeFairy(PlayState* play) {
//     // Block fairy revival in hardcore mode
//     recomp_printf("Hardcore Mode: Fairy revival blocked!\n");
//     return 0;  // No fairy consumed
// }

/**
 * Frame update callback - called every frame during gameplay
 * Updates HUD visibility based on config option
 */
RECOMP_CALLBACK("*", recomp_on_play_update) void hardcore_update(PlayState* play) {
    if (!g_initialized) return;
    
    // Only proceed when in actual gameplay (not title screen or file select)
    // gameMode: 0 = GAMEMODE_NORMAL, 1 = GAMEMODE_TITLE_SCREEN, 2 = GAMEMODE_FILE_SELECT
    if (gSaveContext.gameMode != 0) return;
    
    // Only proceed if we have a valid save file loaded
    if (gSaveContext.fileNum < 0) return;
    
    // Load death count on first gameplay update (when save slot is active)
    if (!g_data_loaded) {
        recomp_printf("Hardcore Mode: Save file loaded, loading death count...\n");
        load_death_count();
        int tier = get_medal_tier();
        recomp_printf("Hardcore Mode: Current medal tier: %s\n", get_medal_name(tier));
        g_data_loaded = 1;
    }
    
    // Create UI on first gameplay update (with proper open/close lifecycle)
    if (!g_ui_initialized) {
        recomp_printf("Hardcore Mode: First gameplay update - creating UI...\n");
        create_test_ui();
        // Update UI to show the loaded death count
        update_test_ui();
    }
    
    // DEBUG: Press L+R+Start together to manually increment death counter for testing
    Input* input = &play->state.input[0];
    u16 pressed = input->press.button;
    u16 current = input->cur.button;
    
    // Check if L+R+Start are all held (increment)
    if ((current & BTN_L) && (current & BTN_R) && (pressed & BTN_START)) {
        recomp_printf("Hardcore Mode: DEBUG - Manual death counter increment!\n");
        increment_death_count();
        int tier = get_medal_tier();
        recomp_printf("Hardcore Mode: New medal tier: %s\n", get_medal_name(tier));
        update_test_ui();
    }
    
    // DEBUG: Press L+R+Z together to manually decrement death counter for testing
    if ((current & BTN_L) && (current & BTN_R) && (pressed & BTN_Z)) {
        recomp_printf("Hardcore Mode: DEBUG - Manual death counter decrement!\n");
        decrement_death_count();
        int tier = get_medal_tier();
        recomp_printf("Hardcore Mode: New medal tier: %s\n", get_medal_name(tier));
        update_test_ui();
    }
}
