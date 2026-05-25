# 🎮 Hardcore Mode - Final Build Guide

## 🎉 Development Complete!

All 4 phases are now implemented:
- ✅ Phase 1: Native DLL infrastructure
- ✅ Phase 2: Death detection & persistence
- ✅ Phase 3: Configuration & mod.toml
- ✅ Phase 4: HUD UI display

**Last remaining step**: Build the native DLL and package the mod!

---

## 📋 Prerequisites

You need **ONE** of the following to build the native DLL:

### Option A: Visual Studio 2019/2022 (Recommended for Windows)
- Download: https://visualstudio.microsoft.com/downloads/
- Install "Desktop development with C++" workload
- ✅ Easiest option for Windows users

### Option B: CMake + Compiler
- CMake 3.10+: https://cmake.org/download/
- MSVC, GCC, or Clang compiler
- ✅ Best for cross-platform builds

### Option C: MinGW-w64
- Download: https://www.mingw-w64.org/downloads/
- Or via Chocolatey: `choco install mingw`
- ✅ Lightweight alternative

---

## 🔨 Step-by-Step Build Instructions

### Step 1: Build the Native DLL

Choose your method:

#### **Method A: Visual Studio (Recommended)**

1. Open "x64 Native Tools Command Prompt for VS 2022" from Start Menu
2. Navigate to the project:
   ```cmd
   cd C:\Users\Patri\ZeldaHardcore\MMRecompModTemplate\native\hardcore_storage
   ```
3. Compile the DLL:
   ```cmd
   cl /LD hardcore_storage.c /Fe..\..\build\hardcore_storage.dll shell32.lib /DHARDCORE_STORAGE_EXPORTS
   ```
4. Verify the output:
   ```cmd
   dir ..\..\build\hardcore_storage.dll
   ```

#### **Method B: CMake**

1. Open PowerShell or Command Prompt
2. Navigate to the native library folder:
   ```bash
   cd C:\Users\Patri\ZeldaHardcore\MMRecompModTemplate\native\hardcore_storage
   ```
3. Generate build files:
   ```bash
   cmake -B build
   ```
4. Build the DLL:
   ```bash
   cmake --build build --config Release
   ```
5. The DLL will be in `build/Release/hardcore_storage.dll`
6. Copy to project build folder:
   ```bash
   copy build\Release\hardcore_storage.dll ..\..\build\
   ```

#### **Method C: MinGW-w64**

1. Open PowerShell or Command Prompt
2. Navigate to the native library folder:
   ```bash
   cd C:\Users\Patri\ZeldaHardcore\MMRecompModTemplate\native\hardcore_storage
   ```
3. Compile with GCC:
   ```bash
   gcc -shared -o ../../build/hardcore_storage.dll hardcore_storage.c -lshell32 -DHARDCORE_STORAGE_EXPORTS -O2
   ```
4. Verify:
   ```bash
   dir ..\..\build\hardcore_storage.dll
   ```

---

### Step 2: Verify Both Files Exist

Make sure you have both the mod code AND the native DLL:

```powershell
cd C:\Users\Patri\ZeldaHardcore\MMRecompModTemplate
dir build\mod.elf
dir build\hardcore_storage.dll
```

You should see:
- ✅ `mod.elf` (~12 KB) - The mod code
- ✅ `hardcore_storage.dll` (~20-50 KB) - File I/O library

---

### Step 3: Package the Mod

Now package everything into a `.nrm` file:

```powershell
cd C:\Users\Patri\ZeldaHardcore\MMRecompModTemplate
..\tools\RecompModTool.exe mod.toml
```

**Expected Output:**
```
Successfully created mod: build/mm_hardcore_mode.nrm
```

The `.nrm` file is a renamed zip archive containing:
- `mod.elf` - Your MIPS mod code
- `hardcore_storage.dll` - Native library for file I/O
- `mod.toml` - Mod metadata

---

## 🎮 Installing & Testing the Mod

### Installation

1. **Find your MM: Recompiled mods folder:**
   - Windows: `%LOCALAPPDATA%\Zelda64Recompiled\mods\`
   - Create the folder if it doesn't exist

2. **Copy the mod file:**
   ```powershell
   copy build\mm_hardcore_mode.nrm %LOCALAPPDATA%\Zelda64Recompiled\mods\
   ```

3. **Launch MM: Recompiled**
   - The mod should appear in the mod menu
   - Enable "Hardcore Mode"
   - Configure options if desired

### Testing Checklist

#### ✅ First Launch Test
1. Start the game with mod enabled
2. Check console output (if visible):
   - Should see "Hardcore Mode: Initializing..."
   - Should see "Hardcore Mode: Starting fresh run with 0 deaths."
   - Should see "Hardcore Mode: HUD UI created successfully"
3. **In-game**: Look for death counter in top-right corner
   - Should show "Deaths: 0"
   - Should show "Gold Medal" in gold color

#### ✅ Death Detection Test
1. Play the game and die (fall off cliff, take damage until health = 0, etc.)
2. **Expected**:
   - Console prints "Hardcore Mode: Death #1 recorded."
   - HUD immediately updates to "Deaths: 1"
   - Medal changes to "Silver Medal" in silver color
3. Restart the game
   - Death count should persist (still shows 1 death)

#### ✅ Medal Tier Test
- **0 deaths**: Gold Medal (gold color)
- **1-5 deaths**: Silver Medal (silver color)
- **6-10 deaths**: Bronze Medal (bronze color)
- **11+ deaths**: No Medal Medal (white color)

#### ✅ Config Options Test
1. Exit to main menu
2. Go to Mod Settings → Hardcore Mode
3. Try toggling "Show Death Counter HUD":
   - **Enabled**: HUD visible during gameplay
   - **Disabled**: HUD hidden
4. **Note**: "Block Fairy Revivals" is currently disabled (commented out in code)

#### ✅ Tamper Detection Test
1. Play and record a death
2. Exit the game
3. Find the data file:
   - Windows: `%LOCALAPPDATA%\Zelda64Recompiled\mods\mm_hardcore\hardcore_*.dat`
4. Edit the file with a hex editor (change any byte)
5. Restart the game
6. **Expected**: Console shows "WARNING - Death counter tampering detected!"
7. Counter resets to 0

---

## 🐛 Troubleshooting

### "Failed to create UI context!"
- **Cause**: recompui API not available (game version too old)
- **Fix**: Update MM: Recompiled to version 1.2.2 or newer

### Mod doesn't appear in mod list
- **Cause**: .nrm file not in correct folder
- **Fix**: Copy to `%LOCALAPPDATA%\Zelda64Recompiled\mods\`

### "Failed to save death count!"
- **Cause**: DLL not loaded or file permissions issue
- **Fix**: 
  - Verify `hardcore_storage.dll` is in the `.nrm` archive
  - Check mod.toml `native_libraries` section
  - Run game as administrator (if on Windows)

### HUD not visible
- **Cause**: Config option disabled
- **Fix**: Go to Mod Settings → Hardcore Mode → Enable "Show Death Counter HUD"

### Deaths not persisting
- **Cause**: DLL file I/O not working
- **Check**:
  - Verify data file created: `%LOCALAPPDATA%\Zelda64Recompiled\mods\mm_hardcore\`
  - Check console for "Failed to save death count" message
  - Verify DLL loaded correctly (no error messages at startup)

---

## 📁 File Structure Summary

After building, you should have:

```
ZeldaHardcore/
├── MMRecompModTemplate/
│   ├── build/
│   │   ├── mod.elf                    ✅ Mod code (MIPS)
│   │   ├── hardcore_storage.dll       ✅ Native DLL
│   │   └── mm_hardcore_mode.nrm       ✅ Final packaged mod
│   ├── native/
│   │   └── hardcore_storage/
│   │       ├── hardcore_storage.h
│   │       ├── hardcore_storage.c
│   │       ├── CMakeLists.txt
│   │       └── BUILD.md
│   ├── src/
│   │   └── hardcore_mod.c             ✅ Main mod implementation
│   ├── mod.toml                       ✅ Mod configuration
│   └── STATUS.md                      📄 Implementation status
└── tools/
    └── RecompModTool.exe              🔧 Packaging tool
```

---

## 🎯 What You've Built

### Core Features
- ✅ **Persistent death tracking** across game sessions
- ✅ **Anti-tamper protection** with checksum validation
- ✅ **Tiered achievement system** (Gold/Silver/Bronze medals)
- ✅ **Real-time HUD display** in top-right corner
- ✅ **Configurable options** (HUD visibility)
- ✅ **Cross-platform ready** (Windows/Linux/Mac)

### Technical Implementation
- **Death Detection**: Hooks into `Health_ChangeBy` function
- **File I/O**: Native DLL with platform-specific paths
- **UI System**: recompui API with Flexbox layout
- **Data Storage**: 8-byte files with MurmurHash checksum
- **Color Coding**: Dynamic medal colors based on death count

---

## 🚀 Next Steps

1. **Build the DLL** (choose Method A, B, or C above)
2. **Package the mod** with RecompModTool
3. **Install and test** in MM: Recompiled
4. **Share your achievement!** Aim for that Gold Medal run! 🏆

---

## 📞 Need Help?

- Review [STATUS.md](STATUS.md) for detailed implementation info
- Check [native/hardcore_storage/BUILD.md](native/hardcore_storage/BUILD.md) for DLL build details
- Verify all files compile without errors: `make clean && make`

**Good luck with your Hardcore run!** 🎮✨
