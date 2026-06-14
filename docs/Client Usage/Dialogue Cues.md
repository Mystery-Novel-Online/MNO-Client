# Dialogue Cues
Dialogue cues are tags embedded into dialogue text which controls how the message appears, sounds and behaves. 

## Quick Examples
```
<hl:red>Warning!</hl>

<scale:1.5>Big text!</scale>

Wait for it...<wait:1000>Now!

<sfx:explosion.opus>BOOM!

<blip:Typewriter>To whom it may concern...
```

---
## Change Text Color - `<hl:color>`

```text
<hl:red>red text</hl>
```

Hex values can also be specified:

```text
<hl:FFAA00>orange text</hl>
```


---

## Change Text Scale - `<sc:value>`

```text
<sc:2.0>large text</sc>
```

---

## New Line - `<nl>`

```text
Line One<nl>
Line Two
```

---
## Pause Dialogue - `<wait:duration>`

```text
Wait for it...<wait:1000>Now!
```

Pauses the text progression for 1000 milliseconds (1 second).

---


## Change Text Speed - `<speed:value>`

```text
<speed:10>Fast text
```

Adjusts how quickly text is revealed for everyone.

---

## Flip - `<flip>`

```text
Let's <flip>flip <flip>together!<flip>
```

Toggles the "flip" state of the character sprite. 

---

## Hide - `<hide>`

```text
Now you see me: <hide>now you don't. 
```

Toggles the "visibility" state of the character on screen.


---

## Layer Swap - `<layer:name:asset>`

```text
<layer:eyes:closed>
```

Changes the asset that is loaded for a specified layer.

---

## Sound Effect - `<sfx:sound>`

Plays a sound effect at a specified point.

```text
<sfx:sparkle.opus>
```

A shortcut for this tag can be activated by holding Ctrl and right clicking a sound in the SFX list.

---

## Swap Message Blips - `<blip:blip_set>`

```text
<blip:Typewriter>"To whom it may concern..."
```

Changes the blip set that is used for the rest of the message.


---
