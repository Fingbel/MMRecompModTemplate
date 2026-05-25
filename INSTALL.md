# 🎮 Hardcore Mode - Installation Guide

## ✅ Build Complete!

Your mod has been successfully built:
- `mm_hardcore_mode.nrm` (3.3 KB) - Main mod package  
- `hardcore_storage.dll` (45.6 KB) - Native library for file I/O

## 📦 Installation Steps

### 1. Locate MM: Recompiled Mods Folder

**Windows:**
```
%LOCALAPPDATA%\Zelda64Recompiled\mods\
```

To open quickly:
- Press `Win + R`
- Type: `%LOCALAPPDATA%\Zelda64Recompiled\mods\`
- Press Enter

Create the folder if it doesn't exist.

### 2. Create Mod Folder

Inside the mods folder, create a folder for your mod:
```
%LOCALAPPDATA%\Zelda64Recompiled\mods\mm_hardcore_mode\
```

### 3. Copy Files

Copy both files from `build\` to the mod folder:

```powershell
# From PowerShell:
$modsFolder = "$env:LOCALAPPDATA\Zelda64Recompiled\mods\mm_hardcore_mode"
New-Item -ItemType Directory -Force -Path $modsFolder
Copy-Item "C:\Users\Patri\ZeldaHardcore\MMRecompModTemplate\build\mm_hardcore_mode.nrm" -Destination $modsFolder
Copy-Item "C:\Users\Patri\ZeldaHardcore\MMRecompModTemplate\build\hardcore_storage.dll" -Destination $modsFolder
```

**Final structure:**
```
%LOCALAPPDATA%\Zelda64Recompiled\mods\mm_hardcore_mode\
├── mm_hardcore_mode.nrm
└── hardcore_storage.dll
```

### 4. Launch MM: Recompiled

1. Start the game
2. Go to **Mods** menu
3. You should see **"Hardcore Mode"** in the list
4. Enable it
5. Configure options (optional):
   - Show Death Counter HUD: Enabled/Disabled
   - Block Fairy Revivals: Currently disabled (not implemented yet)

### 5. Start Playing!

- Death counter appears in top-right corner
- Gold Medal for 0 deaths
- Deaths persist across game sessions
- Data stored in: `%LOCALAPPDATA%\Zelda64Recompiled\mods\mm_hardcore\hardcore_*.dat`

## 🧪 Testing

### First Launch Test
1. Enable mod and start a new game or load save
2. **Look for**: Death counter HUD in top-right (if enabled)
3. **Should show**: "Deaths: 0" and "Gold Medal" in gold color

### Death Test
1. Die in-game (fall off cliff, take damage until health = 0)
2. **Expected**: Counter updates to "Deaths: 1", medal changes to "Silver Medal"
3. Restart game
4. **Expected**: Death count persists (still shows 1)

### Medal Tiers
- **0 deaths**: 🥇 Gold Medal (gold color)
- **1-5 deaths**: 🥈 Silver Medal (silver color)
- **6-10 deaths**: 🥉 Bronze Medal (bronze color)
- **11+ deaths**: ⚪ No Medal (white color)

## 🐛 Troubleshooting

### Mod doesn't appear in list
- **Check**: Files are in correct folder
- **Check**: Both .nrm AND .dll files are present
- **Try**: Restart the game

### "Failed to load native library" error
- **Check**: `hardcore_storage.dll` is in same folder as .nrm file
- **Try**: Run game as administrator
- **Windows**: Make sure it's the 64-bit DLL (not 32-bit)

### HUD not visible
- **Check**: Mod Settings → Hardcore Mode → "Show Death Counter HUD" is Enabled
- **Check**: You're in actual gameplay (not main menu)

### Deaths not persisting
- **Check**: Data folder exists: `%LOCALAPPDATA%\Zelda64Recompiled\mods\mm_hardcore\`
- **Check**: No errors in game console about file I/O

## 🎯 Goal

**Can you beat Majora's Mask with 0 deaths and earn the Gold Medal?** 🏆

Good luck with your Hardcore run!
