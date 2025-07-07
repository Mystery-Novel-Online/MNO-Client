# Character File Structure

Character parameter files are used to define the various variables used across all character outfits. 

---

## Top-Level Structure

```json
{
  "showname": "...",
  "side": "...",
  "gender": "...",
  "outfit_order": [ ... ],
  "scaling_mode": "...",
  "scaling_presets": [ ... ]
}
```

---

## `showname`

- **Type:** `string`
- **Description:** The display name of the character.
- **Example:** `"Makoto Naegi"`

---

## `side`

- **Type:** `string`
- **Description:** Indicates the background position the character is assigned to by default.
- **Example:** `"wit"`

---

## `gender`

- **Type:** `string`
- **Description:** The gender of the character.
- **Example:** `"female"`

---

## `outfit_order`

- **Type:** `array of strings`
- **Description:** Lists the preferred display order of outfits for the character. Any outfits not listed here will be appended to the bottom of the list in alphabetical order.
- **Example:**

```json
"outfit_order": ["Default", "Swimsuit"]
```

---

## `scaling_mode`

- **Type:** `string`
- **Description:** Determines how the character is scaled in the view.
    - `"width_smooth"` - Scales the character so that it fits the width of the viewport. 
    - `"width_pixels"` - Designed for pixel art to make sure the sprite is only scaled perfectly. 

---

## `scaling_presets`

- **Type:** `array of objects`
- **Description:** Preset configurations for scaling the character in different contexts (e.g., close-up shots).

### Preset Object Fields:

- `name` (string): The name of the preset (e.g., `"Close-up"`).
- `vertical` (integer): Vertical offset applied when the preset is active.
- `scale` (integer): Scaling factor to apply.

### Example

```json
{
  "name": "Close-up",
  "vertical": -168,
  "scale": 700
}
```

---