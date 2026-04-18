# Background JSON
Each background folder contains a single `background.json` file. This file defines how the client should render and behave when the background is used. 

---

## Positions

A position represents an alternative view of the current background, usually different camera angles or different character placements. 


```json
{ 
	"positions":
	{
		"wit": 
		{ 
			"background": "bg_name.png", 
			"foreground": "fg_name.png", 
			"ambience": "ambience_sound.opus",
		},
		"position 2": 
		{ 
			"background": "bg_name2.png", 
			"foreground": "fg_name2.png", 
			"ambience": "ambience_sound2.opus",
		}
	}
}
```

- Each parameter within `"positions"` is the name of the position that will be used in-client. 
- You can define as many or as little positions as needed. 
- The data structure of positions are nearly-identical to variants.
- Positions can't contain variant data, but variant data can contain positions. 

---

## Variants 

A **variant** is an alternative version of background.

### Default Variant

If you JSON only contains the base fields:

```json
{ 
	"background": "bg_name.png", 
	"foreground": "fg_name.png", 
	"ambience": "ambience_sound.opus"
}
```

This will automatically be treated as a single variant named `Default`.
This is the most common setup and is sufficent for most backgrounds.

---

### Additional Variants

You can define extra variants by adding additional parameters to the base JSON.

```json
{ 
	"background": "bg_name.png", 
	"foreground": "fg_name.png", 
	"ambience": "ambience_sound.opus",
	
	"festive_season":
	{
		"background": "festive_bg.png", 
		"ambience": "festive_sound.opus"
	}
	
}
```

- Each defined parameter becomes a variant if the object contains a `background` parameter.

---

## Switching Variants

Variants can be switched on the latest TsuserverMNO by running the following command:
`/bg_variant <variant_name>`

Example:

`/bg_variant festive_season`

If the specified variant does not exist, the client will use the default.
