# Lua Theme Events

Events are functions that are called when the client asks the script if they would like to take control of a specific function, allowing the script to override the default functionality. 

This is an outline of events that are called by the client.

- [Tab Toggle Events](#tab-toggle-events)
- [AreaDescriptionEvent](#areadescriptionevent)
- [SongChangeEvent](#songchangeevent)
- [BlipTickEvent](#bliptickevent)
- [ChatboxImageEvent](#chatboximageevent)
- [PairRequestEvent](#pairrequestevent)
- [OOCMessageEvent](#oocmessageevent)

--- 
# Tab Toggle Events
Overrides the default functionality for clicking on a tab button. The name of these events change depending on the group they're part of. For the example the tabs will be in a group named `lists`.

- **Parameters:**
  | Name       | Type     | Description                 |
  |------------|----------|-----------------------------|
  | `tabName`| `String` | The name of the tab that has been toggled. |

- **Example:**
  ```lua
    function listsToggleEvent(tabName)
        if tabName == "areas" then
            Audio.System.Play("sfx/chimes.mp3")
        end
    end
  ```

---

# AreaDescriptionEvent
Overrides the default functionality for setting the Area Description text when it's recieved by the server.

- **Parameters:**
  | Name       | Type     | Description                 |
  |------------|----------|-----------------------------|
  | `areaDescription`| `String` | The area description as recieved by the server. |

- **Example:**
  ```lua
    function AreaDescriptionEvent(areaDescription)
        Area.SetDescription(areaDescription)
    end
  ```

---

# SongChangeEvent
Overrides the default functionality when the client sends a song change. 

- **Parameters:**
  | Name       | Type     | Description                 |
  |------------|----------|-----------------------------|
  | `songPath`| `String` | The file path of the song. |
  | `songName`| `String` | The metadata name of the song. |
  | `submitter`| `String` | The name of the person who changed the song. |

- **Example:**
  ```lua
    function SongChangeEvent(songPath, songName, submitter)
        if songPath == "dro_extra/Mi Oh Daa.opus" then
            OOC.Log.Append("BLOCKED", submitter .. " has attempted to play Mi Oh Daa.")
        else
            Audio.BGM.Play(songPath)
        end
    end
  ```
---

# BlipTickEvent
Overrides the default functionality for triggering a blip tick.
- **Example:**
  ```lua
    function BlipTickEvent()
        Audio.Blip.Tick()
    end
  ```
---

# ChatboxImageEvent
Overrides the default functionality for the chatbox image updating when a message is recieved. 

- **Parameters:**
  | Name       | Type     | Description                 |
  |------------|----------|-----------------------------|
  | `isSelf`| `Bool` | Whether or not the incoming message is sent by you. |
  | `isEmpty`| `Bool` | Whether or not the incoming message is empty. |
  | `speakerOffset`| `String` | `left`, `right`, or empty depending on if the speaker is paired or not, and where they are positioned if so. |


---

# PairRequestEvent
Overrides the default functionality for receiving a pair request. 

- **Parameters:**
  | Name       | Type     | Description                 |
  |------------|----------|-----------------------------|
  | `senderName`| `String` | The name of whoever sent the pair request. |

---

# OOCMessageEvent
Overrides the default functionality for receiving an OOC message from the server. 

- **Parameters:**
  | Name       | Type     | Description                 |
  |------------|----------|-----------------------------|
  | `senderName`| `String` | The display name of whoever sent the OOC message. |
  | `message`| `String` | The contents of the OOC message. |

- **Example:**
  ```lua
    function OOCMessageEvent(senderName, message)
        OOC.Log.Append(senderName, message)
    end
  ```