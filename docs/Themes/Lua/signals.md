# Lua Theme Signals

Signals are functions that are called as a way for the client to tell the script "We just did this thing", letting the script do something with this knowledge before continuing. 

This is an outline of signals that are called by the client.


- [Custom Command Signals](#custom-command-signals)
- [OnUpdate](#onupdate)
- [OnAreaLook](#onarealook)
- [OnAreaDescriptionRecieved](#onareadescriptionrecieved)
- [OnCourtroomSetup](#oncourtroomsetup)
- [OnTogglesCreated](#ontogglescreated)
- [OnTabChanged](#ontabchanged)
- [OnCharacterLoad](#oncharacterload)
- [OnPairRequest](#onpairrequest)
- [OnPairRequestAccepted](#onpairrequestaccepted)
- [OnPairRequestDeclined](#onpairrequestdeclined)
- [OnSongChange](#onsongchange)
- [OnBlipTick](#onbliptick)
- [OnMessageReceived](#onmessagereceived)
- [OnMessageStart](#onmessagestart)
- [OnMessageTick](#onmessagetick)
- [OnMessageCompleted](#onmessagecompleted)
- [OnOOCMessage](#onoocmessage)
- [OnOOCMessageSend](#onoocmessagesend)
- [OnICTextChanged](#onictextchanged)
- [OnICMessageSend](#onicmessagesend)
- [OnWindowFocusChange](#onwindowfocuschange)
- [OnWindowResized](#onwindowresized)
- [OnWindowMoved](#onwindowmoved)

---

# Custom Command Signals
It is possible to create custom commands, however they do not currently take in any parameters. For this example the command will be `/BGMReverb`

- **Example:**
  ```lua
    function OnCommand_bgmreverb()
        Audio.BGM.ToggleReverb(true)
    end
  ```


---

# OnUpdate
Called every frame, providing a deltaTime for specific timing related funcitonality. 
- **Parameters:**
  | Name       | Type     | Description                 |
  |------------|----------|-----------------------------|
  | `deltaTime`| `Int` | The nanoseconds since the last OnUpdate. |

---

# OnAreaLook
Called anytime the look button is pressed.
- **Parameters:**
  | Name       | Type     | Description                 |
  |------------|----------|-----------------------------|
  | `areaReason`| `Int` | The id associated to the message currently displayed on the player list. |

---
# OnAreaDescriptionRecieved
Called when the area description is updated. 
- **Parameters:**
  | Name       | Type     | Description                 |
  |------------|----------|-----------------------------|
  | `areaDescription`| `String` | The contents of the area description. |

---
# OnCourtroomSetup
Called when the courtroom has finished creating all of its widgets. 
- **Parameters:** None

--- 
# OnTogglesCreated
Called when the courtroom has finished creating all of the tabs defined in the theme. 
- **Parameters:** None

---
# OnTabChanged
Called when user triggers a tab switch.
- **Parameters:**
  | Name       | Type     | Description                 |
  |------------|----------|-----------------------------|
  | `tabName`| `String` | The name of the tab. |
  | `tabGroup`| `String` | The name of the tabs group. |

--- 
# OnCharacterLoad
Called when client switches the character folder they are using. 
- **Parameters:**
  | Name       | Type     | Description                 |
  |------------|----------|-----------------------------|
  | `folderName`| `String` | The name of the character folder. |

---
# OnPairRequest
Called when a pair request is recieved. 
- **Parameters:**
  | Name       | Type     | Description                 |
  |------------|----------|-----------------------------|
  | `senderName`| `String` | The name of whoever sent the pair request. |


--- 
# OnPairRequestAccepted
Called when the user accepts a pair request.
- **Parameters:** None

---
# OnPairRequestDeclined
Called when the user declines a pair request.
- **Parameters:** None

---
# OnSongChange
Called when a song change is sent by the server. 
- **Parameters:**
  | Name       | Type     | Description                 |
  |------------|----------|-----------------------------|
  | `songPath`| `String` | The file path of the song. |
  | `songName`| `String` | The metadata name of the song. |
  | `submitter`| `String` | The name of the person who changed the song. |

---
# OnBlipTick
Called after the client plays a blip sound effect.  
- **Parameters:** None

---
# OnMessageReceived
Called when the client recieves an IC message from the server.
- **Parameters:**
  | Name       | Type     | Description                 |
  |------------|----------|-----------------------------|
  | `showname`| `String` | The showname of the speaking character. |
  | `character`| `String` | The folder name of the character. |
  | `emote`| `String` | The image name of the emote. |
  | `text`| `String` | The text contents of the message. |
  | `isEmpty`| `Bool` | Whether or not the message is empty. |

---
# OnMessageStart
Called when the client starts to type out the recieved IC message.
- **Parameters:** None

---
# OnMessageTick
Called when the client inserts a character into the chatbox. 
- **Parameters:**
  | Name       | Type     | Description                 |
  |------------|----------|-----------------------------|
  | `character`| `String` | The character that was inserted into the chatbox. |

---
# OnMessageCompleted
Called when the client finishes typing out the recieved IC message. 
- **Parameters:**
  | Name       | Type     | Description                 |
  |------------|----------|-----------------------------|
  | `isEmpty`| `Bool` | Whether or not the message is empty. |

---
# OnOOCMessage
Called when the client recieves an OOC message from the server. 

- **Parameters:**
  | Name       | Type     | Description                 |
  |------------|----------|-----------------------------|
  | `senderName`| `String` | The display name of whoever sent the OOC message. |
  | `message`| `String` | The contents of the OOC message. |

---
# OnOOCMessageSend
Called right before an OOC message is sent to the server. 
- **Parameters:** None

---
# OnICTextChanged
Called when the input field for the IC message is updated.  
- **Parameters:** None

---
# OnICMessageSend
Called right before an IC message is sent to the server. 
- **Parameters:** None

---
# OnWindowFocusChange
Called when the focus of the window is changed. 
- **Parameters:**
  | Name       | Type     | Description                 |
  |------------|----------|-----------------------------|
  | `isFocused`| `Bool` | Whether or not the window is now focused. |

---
# OnWindowResized
Called when the size of the window is changed. 
- **Parameters:**
  | Name       | Type     | Description                 |
  |------------|----------|-----------------------------|
  | `width`| `Int` | The new width of the window. |
  | `height`| `Int` | The new height of the window. |

---
# OnWindowMoved
Called when the position of the window is moved. 
- **Parameters:**
  | Name       | Type     | Description                 |
  |------------|----------|-----------------------------|
  | `x`| `Int` | The new x value of the window. |
  | `y`| `Int` | The new y value of the window. |

