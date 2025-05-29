# Lua Theme Functions

This is an outline of functions that you can call from within a lua script in a theme.

## 📑 Table of Contents

- 🔊 Music Audio
    - [Audio.BGM.Play](#audiobgmplay)
    - [Audio.BGM.Stop](#audiobgmstop)
    - [Audio.BGM.SetSpeed](#audiobgmsetspeed)
    - [Audio.BGM.SetPitch](#audiobgmsetpitch)
    - [Audio.BGM.ToggleReverb](#audiobgmtogglereverb)
- 🔔 SFX Audio
    - [Audio.SFX.Play](#audiosfxplay)
- 💬 Blip Audio
    - [Audio.Blip.Tick](#audiobliptick)
    - [Audio.Blip.SetRate](#audioblipsetrate)
    - [Audio.Blip.SetSound](#audioblipsetsound)
    - [Audio.Blip.SetGender](#audioblipsetgender)
- ⚙ System Audio
    - [Audio.System.Play](#audiosystemplay)
- 📦 Widgets
  - [Widget.Move](#widgetmove)
  - [Widget.Raise](#widgetraise)
  - [Widget.SetVisible](#widgetsetvisible)
  - [Widget.SetParent](#widgetsetparent)
- 🔖 Stickers
  - [Sticker.Create](#stickercreate)
- 🕹 Buttons
  - [Button.Create](#buttoncreate)
- 🎚 Sliders
  - [Slider.Create](#slidercreate)
  - [Slider.CreateVertical](#slidercreatevertical)
  - [Slider.GetValue](#slidergetvalue)
  - [Slider.SetValue](#slidersetvalue)
- 📁 Tabs
  - [Tabs.Change](#tabschange)
- 💬 ChoiceDialog
  - [ChoiceDialog.TriggerCustom](#choicedialogtriggercustom)
  - [ChoiceDialog.SetText](#choicedialogsettext)
- 📝 In Character
  - [IC.InputField.Focus](#icinputfieldfocus)
  - [IC.InputField.GetText](#icinputfieldgettext)
  - [IC.InputField.SetText](#icinputfieldsettext)
  - [IC.InputField.Append](#icinputfieldappend)
- 💭 Out of Character
  - [OOC.Log.Append](#ooclogappend)
  - [OOC.Name.Get](#oocnameget)
  - [OOC.Name.Set](#oocnameset)
  - [OOC.InputField.GetText](#oocinputfieldgettext)
  - [OOC.InputField.SetText](#oocinputfieldsettext)
- 🚨 System
  - [System.Alert](#systemalert)
- 🌐 Server
  - [Server.GetClientId](#servergetclientid)
  - [Server.GetCharacterId](#servergetcharacterid)
  - [Server.GetCurrentCharacter](#servergetcurrentcharacter)
- 👤 Character
  - [Character.GetShowname](#charactergetshowname)
  - [Character.GetIniswap](#charactergetiniswap)
  - [Character.Exists](#characterexists)
  - [Character.Switch](#characterswitch)
  - [Character.SetScale](#charactersetscale)
  - [Character.SetVertical](#charactersetvertical)
  - [Character.SetHorizontal](#charactersethorizontal)

---

# 🔊 Music Audio
## Audio.BGM.Play
Plays an audio file located in the `sounds/music/` directory on the `Music` track.

- **Parameters:**

  | Name       | Type     | Description                 |
  |------------|----------|-----------------------------|
  | `musicPath`| `String` | Path to the music file      |

- **Example:**
  ```lua
  Audio.BGM.Play("dro_dr1/All All Apologies.opus")
  ```

---

## Audio.BGM.Stop
Completely stops the audio that is being played on the music track.
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
- **Example:**
  ```lua
  Audio.SFX.Play("DRding")
  ```

# 💬 Blip Audio

## Audio.Blip.Tick
Plays the currently set blip file once. 

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
- **Example:**
  ```lua
  Audio.System.Play("screenshot")
  ```

  
# 📦 Widgets

## Widget.Move
Moves a widget to the specified coordinates.

- **Parameters:**
  | Name       | Type     | Description                 |
  |------------|----------|-----------------------------|
  | `widgetName`| `String` | The name of the widget that will be moved. |
  | `x`| `Int` | The x coordinate. |
  | `y`| `Int` | The y coordinate. |
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
- **Example:**
  ```lua
  Widget.Raise("char_select")
  ```

# 🔖 Stickers

## Sticker.Create
Create a sticker widget that will be rendered on the client. 

- **Parameters:**
  | Name       | Type     | Description                 |
  |------------|----------|-----------------------------|
  | `stickerName`| `String` | The name of the sticker that will be created. |
  | `imageFile`| `String` | The file name of the image to be used on the sticker sourced from the theme folder. |
  | `x`| `Int` | The x coordinate in pixels. |
  | `y`| `Int` | The y coordinate in pixels. |
  | `width`| `Int` | The width of the sticker in pixels. |
  | `height`| `Int` | The height of the sticker in pixels. |
- **Example:**
  ```lua
  Sticker.Create("sticker_taskbar", "taskbar", 0, 823, 5120, 30)
  ```


# ⚙ Buttons

## Button.Create
Creates a button that can call an event when clicked. 

- **Parameters:**
  | Name       | Type     | Description                 |
  |------------|----------|-----------------------------|
  | `buttonName`| `String` | The name of the button that will be created, which will be prefixed with `button_` after creation. |
  | `x`| `Int` | The x coordinate in pixels. |
  | `y`| `Int` | The y coordinate in pixels. |
  | `width`| `Int` | The width of the button in pixels. |
  | `height`| `Int` | The height of the button in pixels. |
- **Example:**
  ```lua
  Button.Create("PlayChimes", 544, 960, 200, 30)

  function PlayChimesButtonEvent()
    Audio.System.Play("sfx/chimes.mp3")
    Widget.Move("button_PlayChimes", 0, 0)
  end
  ```

# 🎚 Sliders

## Slider.Create
Creates a slider that can call an event when the value is changed. 

- **Parameters:**
  | Name       | Type     | Description                 |
  |------------|----------|-----------------------------|
  | `sliderName`| `String` | The name of the slider that will be created, which will be prefixed with `slider_` after creation. |
  | `x`| `Int` | The x coordinate in pixels. |
  | `y`| `Int` | The y coordinate in pixels. |
  | `width`| `Int` | The width of the button in pixels. |
  | `height`| `Int` | The height of the button in pixels. |
  | `minimum`| `Int` | The minimum value the slider can reach. |
  | `maximum`| `Int` | The maximum value the slider can reach. |
- **Example:**
  ```lua
  Slider.Create("MusicSpeed", 1004, 678, 146, 20, -100, 100)

  function MusicSpeedValueChanged(value)
    Audio.BGM.SetSpeed(value)
  end
  ```
---
## Slider.CreateVertical
Creates a vertical slider that can call an event when the value is changed. 

- **Parameters:**
  | Name       | Type     | Description                 |
  |------------|----------|-----------------------------|
  | `sliderName`| `String` | The name of the slider that will be created, which will be prefixed with `slider_` after creation. |
  | `x`| `Int` | The x coordinate in pixels. |
  | `y`| `Int` | The y coordinate in pixels. |
  | `width`| `Int` | The width of the button in pixels. |
  | `height`| `Int` | The height of the button in pixels. |
  | `minimum`| `Int` | The minimum value the slider can reach. |
  | `maximum`| `Int` | The maximum value the slider can reach. |
- **Example:**
  ```lua
  Slider.CreateVertical("MusicSpeed", 1004, 678, 146, 20, -100, 100)

  function MusicSpeedValueChanged(value)
    Audio.BGM.SetSpeed(value)
  end
  ```
---
## Slider.GetValue
Returns the current value of the specified slider. 

- **Parameters:**
  | Name       | Type     | Description                 |
  |------------|----------|-----------------------------|
  | `sliderName`| `String` | The name of the slider you want the value from.|
- **Returns:** Int
- **Example:**
  ```lua
  local musicSpeed = Slider.GetValue("slider_MusicSpeed")
  ```
---
## Slider.SetValue
Creates a vertical slider that can call an event when the value is changed. 

- **Parameters:**
  | Name       | Type     | Description                 |
  |------------|----------|-----------------------------|
  | `sliderName`| `String` | The name of the slider you want to set the value of.|
  | `incomingValue`| `Int` | The value you want to set the slider to. |
- **Example:**
  ```lua
  Slider.SetValue("slider_MusicSpeed", 30)
  ```

# 📁 Tabs
## Tabs.Change
Triggers a tab change without clicking on the tab itself. 

- **Parameters:**
  | Name       | Type     | Description                 |
  |------------|----------|-----------------------------|
  | `tabGroup`| `String` | The group that the target group is part of. |
  | `tabName`| `String` | The name of the tab you wish to switch to. |
- **Example:**
  ```lua
  Tabs.Change("lists", "areas")
  ```


# 💬 ChoiceDialog
## ChoiceDialog.TriggerCustom
Triggers the choice pop up with a custom event attached to it. 

- **Parameters:**
  | Name       | Type     | Description                 |
  |------------|----------|-----------------------------|
  | `text`| `String` | The text that will be displayed on the pop up. |
  | `signal`| `String` | The signal that will be sent to script when responding to the dialog. |
- **Example:**
  ```lua
  ChoiceDialog.TriggerCustom("Would you like to switch to a new character?", "SwitchCharacter")

  function SwitchCharacterAccepted() 
    Character.Switch("Persona1")
  end

  function SwitchCharacterDeclined() 
    Audio.System.Play("sfx/chimes.mp3")
  end
  ```
---
## ChoiceDialog.SetText
Sets the text that is displayed on the choice pop up.

- **Parameters:**
  | Name       | Type     | Description                 |
  |------------|----------|-----------------------------|
  | `text`| `String` | The text that will be displayed on the pop up. |
  | `isVisible`| `Bool` | Sets whether or not the pop up will be visible after setting the text. |
- **Example:**
  ```lua
  ChoiceDialog.SetText("You have received a pair request.", true)
  ```

# 📝 In Character
## IC.InputField.Focus
Sets the window focus to the IC text field. 

- **Example:**
  ```lua
  IC.InputField.Focus()
  ```
---
## IC.InputField.GetText
Returns the current text in the IC text field. 

- **Returns:** String
- **Example:**
  ```lua
  local outgoingMessage = IC.InputField.GetText()
  ```
---

## IC.InputField.SetText
Replaces the text in the IC text field. 

- **Parameters:**
  | Name       | Type     | Description                 |
  |------------|----------|-----------------------------|
  | `text`| `String` | The text that will put into the text field. |
- **Example:**
  ```lua
  IC.InputField.SetText("[Pass]")
  ```
---

## IC.InputField.Append
Appends the text to the end of the current contents in the IC text field.

- **Parameters:**
  | Name       | Type     | Description                 |
  |------------|----------|-----------------------------|
  | `text`| `String` | The text that will be appended to the end of the text field. |
- **Example:**
  ```lua
  IC.InputField.Append("\\s")
  ```

# 💭 Out of Character
## OOC.Log.Append
Adds a message to the OOC chat log.

- **Parameters:**
  | Name       | Type     | Description                 |
  |------------|----------|-----------------------------|
  | `displayName`| `String` | The name of the user that will appear in the chat log. |
  | `messageContent`| `String` | The message that will appear in the chat log. |
- **Example:**
  ```lua
  OOC.Log.Append("Lua", "This is a message from your Lua script.")
  ```
---
## OOC.Name.Get
Returns your current OOC name.

- **Returns:** String
- **Example:**
  ```lua
  local oocName = OOC.Name.Get()
  ```
---
## OOC.Name.Set
Sets your OOC username. 

- **Parameters:**
  | Name       | Type     | Description                 |
  |------------|----------|-----------------------------|
  | `displayName`| `String` | The name that the OOC name will be set to. |
- **Example:**
  ```lua
  OOC.Name.Set("RoleplayerGuy")
  ```
---
## OOC.InputField.GetText
Returns your current OOC Message.

- **Returns:** String
- **Example:**
  ```lua
  local outgoingText = OOC.InputField.GetText()
  ```
---
## OOC.InputField.SetText
Sets your OOC message. 

- **Parameters:**
  | Name       | Type     | Description                 |
  |------------|----------|-----------------------------|
  | `message`| `String` | The message that the OOC field will be set to. |
- **Example:**
  ```lua
  OOC.InputField.SetText("/help")
  ```

# 🚨 System
## System.Alert
Causes the application icon in your task bar to behave similarly to a call word.

- **Example:**
  ```lua
  System.Alert()
  ```

# 🌐 Server
## Server.GetClientId
Returns your current client id that was set by the server.

- **Returns:** Int
- **Example:**
  ```lua
  local currentClientId = Server.GetClientId();
  ```
---
## Server.GetCharacterId
Returns your current character id as recognized by the server.

- **Returns:** Int
- **Example:**
  ```lua
  local currentCharacterId = Server.GetCharacterId();
  ```
---
## Server.GetCurrentCharacter
Returns the name of your current character folder as recognized by the server.

- **Returns:** String
- **Example:**
  ```lua
  local serverCharacterName = Server.GetCurrentCharacter();
  ```

# 👤 Character
## Character.GetShowname
Returns your current showname.

- **Returns:** String
- **Example:**
  ```lua
  local userShowname = Character.GetShowname();
  ```
---
## Character.GetIniswap
Returns the name of your current character folder as recognized by the client.

- **Returns:** String
- **Example:**
  ```lua
  local currentIniswap = Character.GetIniswap();
  ```
---

## Character.Exists
Returns whether or not a character exists within your content files. 

- **Parameters:**
  | Name       | Type     | Description                 |
  |------------|----------|-----------------------------|
  | `character`| `String` | The folder name of the character who you want to check for. |
- **Returns:** Bool
- **Example:**
  ```lua
  local personaExists = Character.GetIniswap("Persona3");
  ```
---

## Character.Switch
Attempts to switch to the character folder specified. 

- **Parameters:**
  | Name       | Type     | Description                 |
  |------------|----------|-----------------------------|
  | `character`| `String` | The folder name of the character who you're attempting to switch to. |
- **Example:**
  ```lua
  Character.Switch("Persona3");
  ```
---

## Character.SetScale
Sets the value of the character scale slider. 

- **Parameters:**
  | Name       | Type     | Description                 |
  |------------|----------|-----------------------------|
  | `value`| `Int` | The target value ranging between 1 and 2000.  |
- **Example:**
  ```lua
  Character.SetScale(500);
  ```
---

## Character.SetVertical
Sets the value of the character vertical slider. 

- **Parameters:**
  | Name       | Type     | Description                 |
  |------------|----------|-----------------------------|
  | `value`| `Int` | The target value ranging between -1000 and 1000.  |
- **Example:**
  ```lua
  Character.SetVertical(0);
  ```
---

## Character.SetHorizontal
Sets the value of the character horizontal slider. 

- **Parameters:**
  | Name       | Type     | Description                 |
  |------------|----------|-----------------------------|
  | `value`| `Int` | The target value ranging between 1 and 960.  |
- **Example:**
  ```lua
  Character.SetHorizontal(480);
  ```
