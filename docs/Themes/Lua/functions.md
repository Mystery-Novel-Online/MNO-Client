# Lua API Functions

This is an outline of functions that you can call from within a lua script in a theme.

---


# 📁 Audio Functions

## 🔊 BGM
### [Audio.BGM.Play](#Audio.BGM.Play)  
Plays an audio file located in the `sounds/music/` directory on the music track.

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

### [Audio.BGM.Stop](#Audio.BGM.Stop)  
Completely stops the audio that is being played on the music track.

- **Parameters:** None
- **Returns:** None
- **Example:**
  ```lua
  Audio.BGM.Stop()
  ```

---



### [Audio.BGM.SetSpeed](#Audio.BGM.Play)  
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


### [Audio.BGM.SetPitch](#Audio.BGM.Play) 
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


### [Audio.BGM.ToggleReverb](#Audio.BGM.ToggleReverb)  
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

## 🔔 SFX

### [Audio.SFX.Play](#Audio.SFX.Play)  
Plays an audio file located in the `sounds/general/` directory on the sound effect track.

- **Parameters:**
  | Name       | Type     | Description                 |
  |------------|----------|-----------------------------|
  | `effectName`| `String` | The name of the sound effect you wish to play. |
- **Returns:** None
- **Example:**
  ```lua
  Audio.SFX.Play("DRding")
  ```

## 💬 Blip

### [Audio.Blip.Tick](#Audio.Blip.Tick)  
Plays the currently set blip file once. 

- **Parameters:** None
- **Returns:** None
- **Example:**
  ```lua
  Audio.Blip.Tick()
  ```
---

### [Audio.Blip.SetRate](#Audio.Blip.SetRate)    
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

### [Audio.Blip.SetSound](#Audio.Blip.SetSound)   
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

### [Audio.Blip.SetGender](#Audio.Blip.SetGender)    
Sets the current blip to a file based on the specified gender similarly to the default implementation. 

- **Parameters:**
  | Name       | Type     | Description                 |
  |------------|----------|-----------------------------|
  | `gender`| `String` | The name of the sound effect you wish to play. |
- **Returns:** None
- **Example:**
  ```lua
  Audio.Blip.SetGender("female")
  ```
---

## ⚙ System

### [Audio.System.Play](#Audio.System.Play)  
