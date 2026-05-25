# Quick Start Guide - Your First MM Recomp Mod

## What You Have Now

You've successfully cloned the Majora's Mask Recompiled mod template! This template provides:
- A complete build system (Makefile)
- Header files from the MM decompilation project
- Example mod structure
- Configuration file (mod.toml)

## Before You Begin

Make sure you have installed:
1. ✅ LLVM/Clang 18.1.8 (check with `clang --version`)
2. ✅ Make (check with `make --version`)
3. ✅ RecompModTool (download from https://github.com/N64Recomp/N64Recomp/releases)

See the main `SETUP_GUIDE.md` in the root folder for installation instructions.

## Your First Mod - "Hello World"

### Step 1: Configure Your Mod

Edit `mod.toml` to set your mod's information:

```toml
[manifest]
game_id = "mm.n64.us.rev1"
id = "my_first_mod"  # Change this to your mod's unique ID
version = "1.0.0"
authors = ["Your Name"]  # Add your name
name = "My First Mod"  # Your mod's display name
description = "My first Majora's Mask mod!"  # Brief description
minimum_recomp_version = "1.2.2"
```

### Step 2: Write Some Code

The `src/` folder is where your mod code goes. For example, create `src/my_mod.c`:

```c
#include "modding.h"
#include "global.h"

// Hook into the game's main function to print a message
RECOMP_HOOK("Graph_TaskSet00") void my_graph_hook(GraphicsContext* gfxCtx) {
    // Your code runs before Graph_TaskSet00
    // This is just an example - in a real mod you'd do something more interesting!
}

// Example: Patch a function to change game behavior
RECOMP_PATCH void example_patched_function(void) {
    // Replace an entire function's logic
    // Be careful - this prevents other mods from patching the same function!
}

// Export a function that other mods can use
RECOMP_EXPORT void my_mod_do_something(s32 param) {
    // Other mods can call this function
}
```

### Step 3: Build Your Mod

```powershell
# Navigate to the mod template folder
cd MMRecompModTemplate

# Compile the mod code (creates build/mm_recomp_mod.elf)
make

# If you have make installed with chocolatey or on PATH:
make -j4  # Use 4 parallel jobs for faster compilation
```

### Step 4: Package Your Mod

```powershell
# Run RecompModTool to create the .nrm file
RecompModTool mod.toml build

# Your mod file will be at: build/my_first_mod.nrm
```

### Step 5: Test Your Mod

1. Open Majora's Mask: Recompiled
2. Drag and drop your `.nrm` file onto the game window (before starting)
3. Or click "Install Mods" button in the mod menu
4. Enable your mod in the mod menu
5. Start the game and test!

## Common Modding Patterns

### Hook Into Game Functions

Hooks let multiple mods modify the same function:

```c
// Entry hook - runs BEFORE the target function
RECOMP_HOOK("Player_Init") void my_player_init_hook(Actor* thisx, PlayState* play) {
    // You can access the function's parameters
    // Do something before Player_Init runs
}

// Return hook - runs AFTER the target function
RECOMP_HOOK_RETURN("Player_Init") void my_player_init_return_hook() {
    // Runs after Player_Init completes
}
```

### Replace Functions (Use Sparingly)

Only one mod can replace a function:

```c
RECOMP_PATCH s32 func_808333CC(Player* this) {
    // Completely replace the function's logic
    return 0;
}
```

### Create Events for Other Mods

```c
// Declare an event
RECOMP_DECLARE_EVENT(my_mod_event(Player* player, s32 value));

// Call the event somewhere in your code
void my_function(Player* player) {
    my_mod_event(player, 42);  // All registered callbacks will run
}

// Other mods can register callbacks:
// RECOMP_CALLBACK("my_first_mod", my_mod_event) void my_callback(Player* player, s32 value) { }
```

### Export Functions for Other Mods

```c
RECOMP_EXPORT u8 my_utility_function(s32 param) {
    return param > 0 ? 1 : 0;
}

// Other mods import like this:
// RECOMP_IMPORT("my_first_mod", u8 my_utility_function(s32 param));
```

## Important Files

- `mod.toml` - Mod configuration and metadata
- `src/` - Your mod's C source code
- `include/` - Header files (API headers, game headers)
- `mm-decomp/` - Majora's Mask decompilation (reference)
- `Zelda64RecompSyms/` - Symbol files for recompilation
- `build/` - Compiled output (generated)
- `Makefile` - Build configuration

## Useful Headers

- `modding.h` - Core modding macros (RECOMP_HOOK, RECOMP_PATCH, etc.)
- `z64recomp_api.h` - Zelda64-specific API functions
- `recomputils.h` - Memory allocation, utilities
- `recompconfig.h` - Read mod config options
- `recompui.h` - Create custom UI elements
- `global.h` - Game structs and types

## Finding Functions to Hook/Patch

1. Look in `mm-decomp/src/` for game source code
2. Check `Zelda64RecompSyms/mm.us.rev1.syms.toml` for function names
3. Browse other mods on https://thunderstore.io/c/zelda-64-recompiled/
4. Ask in the modding Discord: https://discord.gg/H2RAnQ4Vec

## Troubleshooting

### Build Fails
- Ensure you're using LLVM 18.1.8 (not 19.x)
- Check that `clang` and `make` are in your PATH
- Run `git submodule update --init --recursive`

### Missing Headers
- If you get "file not found" errors for asset headers, create empty files in `include/dummy_headers/` matching the path

### RecompModTool Errors
- Make sure `build/mm_recomp_mod.elf` exists (run `make` first)
- Check your `mod.toml` for syntax errors
- Ensure function names match those in the symbol file

## Next Steps

- Read the full modding docs: https://hackmd.io/fMDiGEJ9TBSjomuZZOgzNg
- Check out example mods on Thunderstore
- Join the modding Discord for help and discussion
- Look at the `ui-example` branch for UI creation examples

Happy modding! 🎭
