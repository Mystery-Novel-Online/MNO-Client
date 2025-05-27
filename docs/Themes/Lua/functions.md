# Lua API Functions

This is an outline of functions that you can call from within a lua script in a theme.

## 📑 Table of Contents

- [🔊 Music Audio](#-music-audio)
    - [Audio.BGM.Play](#audiobgmplay)
    - [Audio.BGM.Stop](#audiobgmstop)
    - [Audio.BGM.SetSpeed](#audiobgmsetspeed)
    - [Audio.BGM.SetPitch](#audiobgmsetpitch)
    - [Audio.BGM.ToggleReverb](#audiobgmtogglereverb)
- [🔔 SFX Audio](#-sfx-audio)
    - [Audio.SFX.Play](#audiosfxplay)
- [💬 Blip Audio](#-blip-audio)
    - [Audio.Blip.Tick](#audiobliptick)
    - [Audio.Blip.SetRate](#audioblipsetrate)
    - [Audio.Blip.SetSound](#audioblipsetsound)
    - [Audio.Blip.SetGender](#audioblipsetgender)
- [⚙ System Audio](#-system-audio)
    - [Audio.System.Play](#audiosystemplay)
- [📦 Widgets](#widgets)
  - [Widget.Move](#widgetmove)
  - [Widget.Raise](#widgetraise)
  - [Widget.SetVisible](#widgetsetvisible)
  - [Widget.SetParent](#widgetsetparent)
- [🔖 Sticker](#sticker)
  - [Sticker.Create](#stickercreate)
- [🕹 Button](#button)
  - [Button.Create](#buttoncreate)
- [🎚 Slider](#slider)
  - [Slider.Create](#slidercreate)
  - [Slider.CreateVertical](#slidercreatevertical)
  - [Slider.SetValue](#slidersetvalue)
  - [Slider.GetValue](#slidergetvalue)
- [📁 Tabs](#tabs)
  - [Tabs.Change](#tabschange)
- [💬 ChoiceDialog](#choicedialog)
  - [ChoiceDialog.TriggerCustom](#choicedialogtriggercustom)
  - [ChoiceDialog.SetText](#choicedialogsettext)
- [👤 Character](#character)
  - [Character.GetShowname](#charactergetshowname)
  - [Character.GetIniswap](#charactergetiniswap)
  - [Character.Exists](#characterexists)
  - [Character.Switch](#characterswitch)
  - [Character.SetScale](#charactersetscale)
  - [Character.SetVertical](#charactersetvertical)
  - [Character.SetHorizontal](#charactersethorizontal)
- [🌐 Server](#server)
  - [Server.GetClientId](#servergetclientid)
  - [Server.GetCharacterId](#servergetcharacterid)
  - [Server.GetCurrentCharacter](#servergetcurrentcharacter)
- [📝 IC.InputField](#icinputfield)
  - [IC.InputField.Focus](#icinputfieldfocus)
  - [IC.InputField.GetText](#icinputfieldgettext)
  - [IC.InputField.SetText](#icinputfieldsettext)
  - [IC.InputField.Append](#icinputfieldappend)
- [💭 OOC.Log](#ooclog)
  - [OOC.Log.Append](#ooclogappend)
- [💭 OOC.Name](#oocname)
  - [OOC.Name.Get](#oocnameget)
  - [OOC.Name.Set](#oocnameset)
- [💭 OOC.InputField](#oocinputfield)
  - [OOC.InputField.GetText](#oocinputfieldgettext)
  - [OOC.InputField.SetText](#oocinputfieldsettext)
- [🚨 System](#system)
  - [System.Alert](#systemalert)
- [🗺 Area](#area)
  - [Area.SetDescription](#areasetdescription)

---

# 🔊 Music Audio
## Audio.BGM.Play
Plays an audio file located in the `sounds/music/` directory on the `Music` track.

- **Parameters:**

  | Name       | Type     | Description                 |
  |------------|----------|-----------------------------|
  | `musicPath`| `String` | Path to the music file      |

- **Returns:** None

- **Example:**
  ```lua
  Audio.BGM.Play("dro_dr1/All All Apologies.opus")
  ```

---

## Audio.BGM.Stop
Completely stops the audio that is being played on the music track.

- **Parameters:** None
- **Returns:** None
- **Example:**
  ```lua
  Audio.BGM.Stop()
  ```

---



### Audio.BGM.SetSpeed
Sets the current speed of the audio being played on the music track.

- **Parameters:**

  | Name       | Type     | Description                 |
  |------------|----------|-----------------------------|
  | `speedValue`| `Float` | The speed at which the music will be played back at. |
- **Returns:** None
- **Example:**
  ```lua
  Audio.BGM.SetSpeed(1.6)
  ```

---


### Audio.BGM.SetPitch
Sets the current pitch of the audio being played on the music track.

- **Parameters:**

  | Name       | Type     | Description                 |
  |------------|----------|-----------------------------|
  | `pitchValue`| `Float` | The pitch at which the music will be played back at. |
- **Returns:** None
- **Example:**
  ```lua
  Audio.BGM.SetPitch(0.5)
  ```

---


### Audio.BGM.ToggleReverb 
Toggles the reverb on the audio currently being played on the music track. 

- **Parameters:**

  | Name       | Type     | Description                 |
  |------------|----------|-----------------------------|
  | `reverbState`| `Bool` |        |
- **Returns:** None
- **Example:**
  ```lua
  Audio.BGM.ToggleReverb(true)
  ```

# 🔔 SFX Audio

## Audio.SFX.Play
Plays an audio file located in the `sounds/general/` directory on the `Effects` track.

- **Parameters:**
  | Name       | Type     | Description                 |
  |------------|----------|-----------------------------|
  | `effectName`| `String` | The name of the sound effect you wish to play. |
- **Returns:** None
- **Example:**
  ```lua
  Audio.SFX.Play("DRding")
  ```

# 💬 Blip Audio

## Audio.Blip.Tick
Plays the currently set blip file once. 

- **Parameters:** None
- **Returns:** None
- **Example:**
  ```lua
  Audio.Blip.Tick()
  ```
---

## Audio.Blip.SetRate   
Sets how often blips will play when typing a message. 

- **Parameters:**
  | Name       | Type     | Description                 |
  |------------|----------|-----------------------------|
  | `blipRate`| `Int` | The specified amount of characters need to be typed out before the sound is played again. |
- **Returns:** None
- **Example:**
  ```lua
  Audio.Blip.SetRate(14)
  ```
---

## Audio.Blip.SetSound 
Sets the current blip to a file located in `sounds/general/`. 

- **Parameters:**
  | Name       | Type     | Description                 |
  |------------|----------|-----------------------------|
  | `soundFile`| `String` | The file name of the sound effect to be used as a blip. |
- **Returns:** None
- **Example:**
  ```lua
  Audio.Blip.SetSound("sfx-blipfemale.opus")
  ```
--- 

## Audio.Blip.SetGender
Sets the current blip to a file based on the specified gender similarly to the default implementation. 

- **Parameters:**
  | Name       | Type     | Description                 |
  |------------|----------|-----------------------------|
  | `gender`| `String` | The gender of the blip file you wish to use. |
- **Returns:** None
- **Example:**
  ```lua
  Audio.Blip.SetGender("female")
  ```

# ⚙ System Audio

## Audio.System.Play 
Plays an audio file located in `sounds/general/` on the `System` audio track. 

- **Parameters:**
  | Name       | Type     | Description                 |
  |------------|----------|-----------------------------|
  | `effectName`| `String` | The name of the sound effect you wish to play. |
- **Returns:** None
- **Example:**
  ```lua
  Audio.System.Play("screenshot")
  ```

  
# ⚙ Widgets

## Widget.Move
Moves a widget to the specified coordinates.

- **Parameters:**
  | Name       | Type     | Description                 |
  |------------|----------|-----------------------------|
  | `widgetName`| `String` | The name of the widget that will be moved. |
  | `x`| `Int` | The x coordinate. |
  | `y`| `Int` | The y coordinate. |
- **Returns:** None
- **Example:**
  ```lua
  Widget.Move("player_list", 41, 21)
  ```

--- 
## Widget.SetVisible
Sets whether or not the specified widget is currently active.

- **Parameters:**
  | Name       | Type     | Description                 |
  |------------|----------|-----------------------------|
  | `widgetName`| `String` | The name of the widget that you are targeting. |
  | `visibleState`| `Bool` | The visiblity state that the widget will be set to. |
- **Returns:** None
- **Example:**
  ```lua
  Widget.SetParent("viewport", false)
  ```

--- 
## Widget.SetParent
Parents the target widget onto another one.

- **Parameters:**
  | Name       | Type     | Description                 |
  |------------|----------|-----------------------------|
  | `parentName`| `String` | The name of the widget that will become the parent of the child. |
  | `childName`| `String` | The name of the widget that will become the child of the parent. |

- **Returns:** None
- **Example:**
  ```lua
  Widget.SetParent("char_select", "player_list")
  ```

--- 

## Widget.Raise
Raises the specified widget to the top of the render stack.

- **Parameters:**
  | Name       | Type     | Description                 |
  |------------|----------|-----------------------------|
  | `widgetName`| `String` | The name of the widget that will be raised. |
- **Returns:** None
- **Example:**
  ```lua
  Widget.Raise("char_select")
  ```