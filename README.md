# Tower Defense — Setup Guide (SFML 2.6.1 + VS 2017)

## Project Structure

```
TowerDefense/
├── include/
│   ├── Entity.h       ← Abstract base class (pure virtual)
│   ├── Enemy.h        ← Enemy base + BasicEnemy, FastEnemy, TankEnemy
│   ├── Tower.h        ← Tower base + CannonTower, MachineGunTower, SniperTower
│   └── Game.h         ← Central game manager
├── src/
│   ├── Enemy.cpp
│   ├── Tower.cpp
│   ├── Game.cpp
│   └── main.cpp
└── TowerDefense.vcxproj
```

---

## Step 1 — Download SFML 2.6.1

1. Go to https://www.sfml-dev.org/download/sfml/2.6.1/
2. Download **"Visual C++ 15 (2017) — 64-bit"**
3. Extract to `C:\SFML-2.6.1\`

Your folder should look like:
```
C:\SFML-2.6.1\
    bin\        ← the .dll files
    include\    ← the header files
    lib\        ← the .lib files
```

---

## Step 2 — Open in Visual Studio 2017

Option A — Open the .vcxproj directly:
- File → Open → Project/Solution → select `TowerDefense.vcxproj`

Option B — Create a new Empty Project and add files:
- File → New → Project → Visual C++ → Empty Project
- Add all .h files to Header Files filter
- Add all .cpp files to Source Files filter
- Then do Steps 3 and 4 manually below

---

## Step 3 — Fix SFML Path (if needed)

Open `TowerDefense.vcxproj` in a text editor.
Find this line near the top:

```xml
<SFMLDir>C:\SFML-2.6.1\</SFMLDir>
```

Change it to wherever you extracted SFML.

---

## Step 4 — Copy the DLLs (CRITICAL!)

After building, your .exe will crash without the DLLs.

From `C:\SFML-2.6.1\bin\` copy these into your output folder
(same folder as the .exe, usually `x64\Debug\`):

**For Debug build:**
- `sfml-graphics-d-2.dll`
- `sfml-window-d-2.dll`
- `sfml-system-d-2.dll`
- `openal32.dll`

**For Release build:**
- `sfml-graphics-2.dll`
- `sfml-window-2.dll`
- `sfml-system-2.dll`
- `openal32.dll`

---

## Step 5 — Build and Run

1. Set configuration to **Debug** and platform to **x64**
2. Build → Build Solution (Ctrl+Shift+B)
3. Run (F5)

---

## Controls

| Key / Action         | Effect                          |
|----------------------|---------------------------------|
| `SPACE`              | Start next wave                 |
| `1`                  | Select Cannon Tower (50g)       |
| `2`                  | Select Machine Gun Tower (75g)  |
| `3`                  | Select Sniper Tower (100g)      |
| Left Click (on map)  | Place selected tower            |

---

## OOP Requirements Checklist

| Requirement              | Where it lives                        |
|--------------------------|---------------------------------------|
| Abstract base class      | `Entity` (update/render = pure virtual) |
| Virtual functions        | All of Enemy and Tower                |
| Derived classes          | Basic/Fast/Tank + Cannon/MG/Sniper    |
| Polymorphic containers   | `vector<Enemy*>` + `vector<Tower*>`   |
| Encapsulation            | protected members in Enemy/Tower      |
| Virtual destructor       | `Entity::~Entity()` + Game::~Game()   |
| Memory management        | `Game::~Game()` deletes all pointers  |

---

## Enemies

| Type        | HP  | Speed | Gold | Notes            |
|-------------|-----|-------|------|------------------|
| BasicEnemy  | 80  | 80    | 10   | SLIME            |
| FastEnemy   | 40  | 160   | 15   | BEE              |
| TankEnemy   | 300 | 40    | 50   | MONSTER          |

## Towers

| Type          | Damage | Range | Fire Rate | Cost |
|---------------|--------|-------|-----------|------|
| Cannon        | 25     | 120   | 1/sec     | 50g  |
| Machine Gun   | 8      | 80    | 5/sec     | 75g  |
| Sniper        | 80     | 250   | 0.4/sec   | 100g |

---

## Common Errors

**"Cannot open include file: SFML/Graphics.hpp"**
→ Your include path is wrong. Check Step 3.

**Linker error: unresolved external symbol sfml-**
→ Your lib path is wrong. Check the .vcxproj SFMLDir value.

**Runs but crashes immediately**
→ Missing DLLs. Do Step 4.

**"arial.ttf not found" / blank text**
→ The code tries C:\Windows\Fonts\arial.ttf which exists on all Windows PCs.
  If text still doesn't show, put arial.ttf in the same folder as your .exe.
