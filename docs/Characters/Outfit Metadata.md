# Outfit File Structure

Outfit parameter files are used to define the various emotes a character can use and its relevant rules. 

---

## Top-Level Structure

```json
{
  "default_rules": { ... },
  "layers": [ ... ],
  "emotes": [ ... ]
}
```

---

## `default_rules`

Defines the rules emotes will follow by default unless individually specified.

### Fields

- `show_desk` (boolean):  
  Indicates whether the desk layer should be shown by default.

- `ignore_offsets` (boolean):
  Indicates whether or not the player offset sliders should affected the positioning of the sprite.  

Example:
```json
"default_rules": {
  "show_desk": true,
  "ignore_offsets" : true
}
```

---

## `layers`

An array of layer definitions, each representing the additional sprites that can be drawn alongside the main sprite.

### Layer Object

Each layer has the following properties:

- `name` (string):  
  Identifier for the layer (e.g., `"face"`, `"effect"`).

- `offset` (object):  
  Defines the position and size of the layer in pixels.
  - `x` (int): X-coordinate of the layer.
  - `y` (int): Y-coordinate of the layer.
  - `width` (int): Width of the layer.
  - `height` (int): Height of the layer.

- `order` (string):  
  The draw order priority. Layers will always use `"top"` by default. 
    - `"top"` 
    - `"below"` 

- `toggle` (optional, string):  
  A toggleable visibility option for user control (e.g., `"Effect"`). These can be toggled by right-clicking the emote selector in-client. 

### Example

```json
{
  "name": "face",
  "offset": { "x": 60, "y": 60, "width": 63, "height": 63 },
  "order": "top"
}
```

---

## `emotes`

An array defining the metadata for various emotes used in-client.

### Emote Object

Each emote has the following fields:

- `name` (string):  
  Name of the emote (e.g., `"smile"`, `"talk"`).

- `image` (optional, string):  
  The base image used (e.g., `"body"`).

- `pre` (optional, string):  
  Specifies a pre-animation to play before the emote appears.

- `sequence` (optional, string):  
  Defines an animation sequence name (e.g., `"Ghost Away"`).

- `sfx` (optional, string):  
  Name of a sound effect to play (e.g., `"explosion"`).

- `sfx_delay` (optional, int):  
  Defines how many miliseconds to wait before playing the sound effeect defined in `sfx` (e.g., `3000` to wait 3 seconds).

- `outfit` (optional, string):  
  If wanting to use an `image` from another outfit folder you can define what folder to grab it from here. (e.g., `"Default"`).
    - If the parameter is present but left blank, it will use the root directory  



video
desk
ignore_offsets

### Example

```json
{
  "name": "dissapointed",
  "image": "body",
  "face": "dissapointed",
  "sfx": "explosion"
}
```

---

## Summary

The `outfit.json` file is a compact and structured way to define:
- Display rules (`default_rules`)
- Layered visual components (`layers`)
- Character expressions or animations (`emotes`)

This structure allows for modular and customizable character rendering in an interactive or animated environment.
