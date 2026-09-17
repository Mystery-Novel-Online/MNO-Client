# Folder Structure
A character folder is set up with at least a `char.json` and `char_icon.png` file in it's root directory, with an outfits folder for organizing specific emotes.

## Minimal Structure

```text
Shirou Emiya (FSN)/
│   char.json
│   char_icon.png
│   sounds.ini
│
└── outfits/
    └── Default/
        └── outfit.json
```

## Required files
- `char_icon.png` - The preview icon for your character that is displayed throughout the client.
- `char.json` - Defines the parameters that the character will use across all outfits.
- `outfits/` - Contains all the outfit folders. 
- At least one outfit subfolder which contains an `outfit.json`
---
## Optional Files
- `sounds.ini` - Assigns a custom list of sound effects to be loaded by the client when the character is selected.
- `animations.ini` - Assigns a custom list of animations to be loaded by the client when the character is selected.
- `animations/` - Contains animation files that are exclusive to this character, and has priority over global animations with the same name.

---
## Outfit Structure
Each outfit contains its own configuration and assets.
```markdown
outfits/
├───Casual/
│   │   body.webp
│   │   outfit.json
│   │
│   └───emotions
│           Blush.png
│           Closed.png
│           Dumbfounded.png
│           Happy.png
│           Smile.png
│
└───School Uniform/
    │   cautious.webp
    │   default.webp
    │   outfit.json
    │
    ├───emotions
    │       Cautious.png
    │       Closed.png
    │       Confused.png
    │       Serious.png
    │       Smile.png
    │       Sweat.png
    │
    └───face
            closed.webp
            confused.webp
            serious.webp
            smile.webp
            sweat.webp
```
---
### Emotions Folder
- This folder is found in every outfit folder and contains the button images used by the emote selector. 
- These images are suggested to be 82x82 in file size and stored in `.webp` format, but can still use `.png`. 
- The file name for each button must share the name of the emote it is targetting, not the filename of the sprite it uses. 
