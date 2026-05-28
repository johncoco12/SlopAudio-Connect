# SlopAudio Connect

Desktop companion app for SlopSmith Plus. Captures audio from your instrument, detects pitch in real time using the YIN algorithm, and streams it to the SlopSmith Plus server over UDP so your playing is reflected in the game.

## Features

- **Auto-discovery** — finds SlopSmith Plus servers on your LAN via UDP beacon; manual IP entry as fallback
- **Profile login** — fetches profiles from the server, PIN-based auth (hashed, never sent in plain text)
- **Real-time pitch detection** — YIN algorithm, ~60 Hz output, frequency + confidence per frame
- **VST3 plugin chain** — load, reorder, bypass, and edit plugins that run on your audio before detection
- **Session lifecycle** — automatically starts and stops monitoring when the game starts or ends a track
- **Audio device selection** — pick input/output device, sample rate, and buffer size

## Requirements

- Linux, macOS, or Windows
- CMake 3.22+, C++17 compiler
- libcurl (system package)
- A running SlopSmith Plus server

## Build

```bash
git clone https://github.com/your-org/SlopAudio-Connect.git
cd SlopAudio-Connect
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

JUCE 7 and nlohmann/json are fetched automatically by CMake.

The binary lands at `build/SlopAudioConnect_artefacts/SlopAudioConnect` (`.app` bundle on macOS).

## Usage

1. **Connect** — open the Connect tab, select your server from the list or enter IP/port manually
2. **Login** — pick your profile, enter your PIN, click Login
3. **Audio** — go to the Audio tab, select your input device and set buffer size
4. **Plugins** — optionally add VST3 plugins to the chain via the Plugins tab
5. **Play** — once logged in, the app follows the game session automatically; monitoring starts and stops with each track

The status bar at the bottom shows connection state and live pitch when monitoring is active.

## Architecture

| Layer | Tech |
|---|---|
| GUI | JUCE 7 |
| Audio / pitch | JUCE AudioDeviceManager + hand-written YIN |
| Network | Raw UDP sockets (control + pitch stream) |
| Serialisation | nlohmann/json |
| Build | CMake 3.22, C++17 |

## Troubleshooting

**Server not appearing in the list** — check that the SlopSmith Plus server is running and that UDP broadcast traffic on port 54920 isn't firewalled. Use manual connect as a fallback.

**Login denied** — verify the correct profile is selected and the PIN is right. Auth is validated server-side.

**No pitch in game** — confirm the right input device is selected in the Audio tab and that it's receiving signal. The pitch stream only runs when the game is in the Monitoring state (track actively playing).

**Crackling audio** — increase buffer size in the Audio tab. 512 samples is a good starting point.

**VST3 plugins not found after scan** — default scan paths are `~/.vst3` and `/usr/lib/vst3` on Linux, `~/Library/Audio/Plug-Ins/VST3` on macOS, `C:\Program Files\Common Files\VST3` on Windows.


