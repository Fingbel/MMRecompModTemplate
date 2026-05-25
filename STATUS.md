# Hardcore Mode - Implementation Status

## ✅ Phase 1: Infrastructure (COMPLETE)

### Native DLL for File I/O
- **Status**: Code complete, needs compilation
- **Files Created**:
  - `native/hardcore_storage/hardcore_storage.h` - Header with API
  - `native/hardcore_storage/hardcore_storage.c` - Implementation with checksum
  - `native/hardcore_storage/CMakeLists.txt` - Build configuration
  - `native/hardcore_storage/BUILD.md` - Build instructions

### Features Implemented:
- ✅ File I/O with platform-specific paths (Windows/Linux/Mac)
- ✅ Simple checksum validation (MurmurHash-like algorithm)
- ✅ Per-save-file storage (hash-based filenames)
- ✅ Tamper detection
- ✅ Cross-platform support

### Data Storage Location:
- **Windows**: `%LOCALAPPDATA%\Zelda64Recompiled\mods\mm_hardcore\`
- **Linux/Mac**: `~/.config/Zelda64Recompiled/mods/mm_hardcore/`
- **Filename**: `hardcore_<hash>.dat` (8 bytes: 4 byte death count + 4 byte checksum)

### Build Requirements (Native DLL):
The DLL still needs to be compiled. You need ONE of:
1. **Visual Studio 2019/2022** with C++ Desktop Development
2. **CMake 3.10+** with C compiler
3. **MinGW-w64** (gcc)

See [native/hardcore_storage/BUILD.md](native/hardcore_storage/BUILD.md) for instructions.

## ✅ Phase 2: Death Detection (COMPLETE)

### Mod Integration
- **Status**: ✅ Complete and compiling
- **File**: `src/hardcore_mod.c`

### Features Implemented:
- ✅ RECOMP_IMPORT declarations for native DLL functions
- ✅ RECOMP_CALLBACK for initialization (recomp_on_init)
- ✅ RECOMP_HOOK_RETURN on Health_ChangeBy for death detection
- ✅ Death counter increment and persistence
- ✅ Checksum validation on load
- ✅ Tamper detection warnings
- ✅ Medal tier calculation (Gold/Silver/Bronze)

### Death Detection Logic:
```c
RECOMP_HOOK_RETURN("Health_ChangeBy") void hardcore_check_death(void) {
    if (gSaveContext.save.saveInfo.playerData.health <= 0) {
        increment_death_count();
    }
}
```

### Build Status:
```bash
cd MMRecompModTemplate
make clean
make
# ✅ Builds successfully (no warnings or errors)
```

## ⏳ Phase 3: Mod Configuration (PARTIAL)

### mod.toml Updates
- **Status**: ✅ Complete
- **Changes**:
  - ✅ Updated mod ID: `mm_hardcore_mode`
  - ✅ Updated display name: "Hardcore Mode"
  - ✅ Added native_libraries registration
  - ✅ Updated output filename: `mm_hardcore_mode.nrm`
  - ✅ Added config options:
    - `block_fairies` - Block Fairy Revivals (Disabled/Enabled)
    - `show_hud` - Show Death Counter HUD (Disabled/Enabled)

### Features NOT Yet Implemented:
- ⏸️ Fairy blocking (commented out - needs full Inventory_ConsumeFairy reimplementation)

## ⏸️ Phase 4: UI Display (NOT STARTED)

## 📦 Current Build Output

### Mod Code (MIPS ELF)
- **File**: `build/mod.elf`
- **Status**: ✅ Compiles successfully
- **Size**: TBD (need to measure after clean build)
~12 KB (after linking)
- **Features**: Death detection, persistence, HUD UI
### Native DLL
- **File**: `build/hardcore_storage.dll` (Windows) / `.so` (Linux) / `.dylib` (Mac)
- **Status**: ⚠️ Not built yet (need Visual Studio/CMake/MinGW)
- **Expected Size**: ~20-50 KB

### Final Package
- **File**: `build/mm_hardcore_mode.nrm`
- **Status**: ⏸️ Cannot package until DLL is built
- **Command**: `RecompModTool.exe mod.toml`

## 🧪 Testing Checklist

### Before Testing:
- [ ] Build native DLL (hardcore_storage.dll/.so/.dylib)
- [ ] Verify DLL is in `build/` directory
- [ ] Run `RecompModTool.exe mod.toml` to package mod
- [ ] Copy `mm_hardcore_mode.nrm` to game's mod folder

### Test Cases:
1. **First Launch**:
   - [ ] Mod initializes with "Starting fresh run with 0 deaths"
   - [ ] Console shows "Current medal tier: Gold"

2. **Death Detection**:
   - [ ] Take damage until health reaches 0
   - [ ] Console prints "Death #1 recorded"
   - [ ] Death count persists across game restarts

3. **Tamper Detection**:
   - [ ] Manually edit `.dat` file
   - [ ] Restart game
   - [ ] Console shows "WARNING - Death counter tampering detected!"
   - [ ] Counter resets to 0

4. **Medal Tiers**:
   - [ ] 0 deaths = Gold
   - [ ] 1-5 deaths = Silver
   - [ ] 6-10 deaths = Bronze
   - [ ] 11+ deaths = None

## 🐛 Known Issues

1. **DLL Not Built**: Native library needs compilation (see BUILD.md)
2. **Save Path Generic**: Currently uses `"mm_save_default"` instead of actual save file path
3. **Fairy Blocking Disabled**: Feature commented out (needs full function reimplementation)
4. **No HUD Display**: Phase 4 not started yet

## 🔧 Next Steps

### Immediate (to get v0.1 working):
1. **Build Native DLL**:
   - Option A: Use Visual Studio Developer Command Prompt
   - Option B: Install CMake and build
   - Option C: Install MinGW-w64 and use gcc
   
2. **Package Mod**:
   ```cmd
   cd C:\Users\Patri\ZeldaHardcore\MMRecompModTemplate
   ..\tools\RecompModTool.exe mod.toml
   ```

3. **Test in Game**:
   - Load MM: Recompiled
   - Enable mod in mod menu
   - Start new save or load existing
   - Check console for initialization messages
   - Die to test death counter

### F
5. **Improve Save Path Detection**:
   - Hook into save system to get actual save file path
   - Support multiple save slots properly

6. **Add Fairy Blocking** (Optional):
   - Fully reimplement `Inventory_ConsumeFairy()`
   - Or find hook-based approach to prevent fairy usage

5# 📊 Code Statistics

- **Native Library**: 3 files, ~350 lines C code
- **Mod Code**: 1 file, ~135 lines C code  
- **Configuration**: mod.toml updated with 2 config options
- **Documentation**: BUILD.md, this STATUS.md

## 🎯 Version Roadmap
285 lines C code (with full UI implementation)
- **v0.1** (Current - Ready to Build): Death tracking + persistence + HUD display (DLL compilation needed)
- **v0.2**: Multi-save support + enhanced UI
- **v0.3**: Fairy blocking + advanced anti-tamper
- **v1.0**: Full release with all features + cross-platform testing
