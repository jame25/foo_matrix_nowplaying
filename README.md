# Foobar2000 Matrix "Now Playing" Component

A Foobar2000 component that sends "Now Playing" notifications to Matrix chat rooms with real-time track updates.

## ✨ Features

- 🎵 **Real-time track notifications** - Sends track information when songs change
- ⏸️ **Playback state updates** - Optional pause/resume notifications  
- ⏹️ **Stop notifications** - Optional stop event notifications
- 🔐 **Secure Matrix integration** - Uses Matrix access tokens for authentication
- ⚡ **Asynchronous messaging** - Non-blocking message sending
- 🔄 **Automatic retry logic** - Handles failed messages gracefully
- 🧪 **Connection testing** - Test Matrix connectivity from preferences
- 🌐 **Modern Matrix API** - Uses Matrix Client-Server API v3
- 🛠️ **Multiple build approaches** - Progressive development support

## 📋 Prerequisites (for building)

- **Foobar2000** (32-bit version)
- **Visual Studio 2022** with "Desktop development with C++" workload
- **vcpkg package manager** (for dependencies)
- **Matrix account** with:
  - Personal access token
  - Room ID where notifications will be sent
 
## 📦 Installation

1. **Install foo_matrix_nowplaying.fb2k-component:**
   ```
   Download and install the file available in releases: foo_matrix_nowplaying.fb2k-component 
   ```

2. **Restart Foobar2000**

3. **Verify installation:**
   - Check **View → Console** for: `"Matrix Now Playing: Component loaded successfully"`
   - Navigate to **File → Preferences → Tools → Matrix Now Playing**

## ⚙️ Configuration

### Getting Matrix Credentials

#### Access Token
- **Element Web/Desktop:** Settings → Help & About → Advanced → Access Token
- **Element Mobile:** Settings → Help & About → Advanced → Access Token
- Copy the entire token string (starts with `syt_` or similar)

#### Room ID
- **Element:** Room Settings → Advanced → Internal room ID
- Format: `!abc123xyz:matrix.org`
- **Alternative:** Use Matrix room alias like `#room:matrix.org`

### Component Setup

1. **Open Preferences:**
   - File → Preferences → Tools → Matrix Now Playing

2. **Configure Settings:**
   - **Homeserver URL:** Your Matrix server (e.g., `https://matrix.org`)
   - **Access Token:** Your personal access token
   - **Room ID:** Target room for notifications
   - **Enable notifications:** Check desired notification types

3. **Test Configuration:**
   - Click **"Test Connection"** button
   - Check console for success/error messages

4. **Apply Settings:**
   - Click **Apply** then **OK**

## 🚀 Quick Start

### Complete Build

```batch
# Clone and setup vcpkg (if not already installed)
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.bat
./vcpkg integrate install

# Build the complete component
build-simple-matrix.bat
```

## 🔧 Building from Source

### Method 1: Automated Build Script

```batch
# Run from Visual Studio Developer Command Prompt
build-matrix-complete.bat
```

This script:
- Installs required vcpkg dependencies (curl, jsoncpp)
- Compiles all source files with proper SDK integration
- Links against foobar2000 SDK and external libraries
- Outputs `foo_matrix_nowplaying.dll` to the `output/` directory

### Method 2: Manual vcpkg + Visual Studio

1. **Install dependencies:**
```batch
vcpkg install curl:x86-windows jsoncpp:x86-windows
```

2. **Open project:**
   - Launch Visual Studio 2022
   - Open [`foo_matrix_nowplaying.vcxproj`](foo_matrix_nowplaying.vcxproj)
   - Set configuration to **Release** and platform to **x86**

3. **Build:**
   - Build → Build Solution (Ctrl+Shift+B)

### Method 3: CMake Build

```batch
mkdir build
cd build
cmake -G "Visual Studio 17 2022" -A Win32 ..
cmake --build . --config Release
```

## 🔍 Troubleshooting

### Build Issues

#### Missing Symbols Error
```
Solution: Use build-working-minimal.bat first, then build-matrix-complete.bat
Cause: Prebuilt SDK libraries may have missing symbols
```

#### vcpkg Dependencies Not Found
```bash
# Reinstall dependencies
vcpkg install curl:x86-windows jsoncpp:x86-windows
vcpkg integrate install
```

#### Wrong Architecture
```
Error: Component doesn't load in Foobar2000
Solution: Ensure you're building for x86 (32-bit), not x64
```

### Runtime Issues

#### HTTP 405 Method Not Allowed ✅ FIXED
- **Cause:** Using deprecated Matrix `/r0/` API endpoints
- **Fix:** Updated to Matrix Client-Server API v3 (`/v3/` endpoints)
- **Status:** Resolved in current version

#### HTTP 401 Unauthorized
- **Cause:** Invalid or expired access token
- **Fix:** Generate new access token from Matrix client

#### HTTP 403 Forbidden
- **Cause:** Insufficient room permissions
- **Fix:** Ensure bot has permission to send messages in target room

#### Component Not Appearing
1. Verify 32-bit build (x86 architecture)
2. Check Foobar2000 console for error messages
3. Ensure all dependencies are properly linked

#### Messages Not Sending
1. Verify component is enabled in preferences
2. Check **View → Console** for detailed error messages
3. Test connection using the "Test Connection" button
4. Verify room permissions and access token validity

## 🛠️ Development Notes

### Architecture
- **HTTP Client:** libcurl with SSL support
- **JSON Processing:** JsonCpp library
- **Threading:** Asynchronous message sending to prevent UI blocking
- **API Version:** Matrix Client-Server API v3 (modern, stable)
- **Encoding:** UTF-8 throughout for international character support

### Progressive Development Approach
1. **Start with minimal:** [`src/working_minimal.cpp`](src/working_minimal.cpp) - Basic component loading
2. **Add playback callbacks:** Track change detection
3. **Add preferences UI:** Configuration interface
4. **Add Matrix client:** HTTP communication
5. **Add error handling:** Robust error recovery

### Key Classes
- [`play_callback_matrix`](src/play_callback.cpp) - Handles foobar2000 playback events
- [`matrix_preferences_page`](src/preferences.cpp) - Configuration UI
- [`MatrixClient`](src/matrix_client.cpp) - HTTP client for Matrix API
- [`matrix_init`](src/main.cpp) - Component initialization

## 🔮 Future Enhancements

- [ ] **Rich message formatting** - Custom templates with track variables
- [ ] **Album art upload** - Send cover art to Matrix rooms
- [ ] **Multiple room support** - Send to different rooms based on criteria
- [ ] **Reaction support** - React to currently playing tracks
- [ ] **Matrix widgets integration** - Rich presence indicators
- [ ] **Scrobbling integration** - Last.fm/ListenBrainz compatibility
- [ ] **Playlist sharing** - Share current playlist to Matrix
- [ ] **Voice message support** - Audio snippets of tracks

## 📜 License

This project is provided as-is for educational and personal use. Feel free to modify and distribute according to your needs.

## 🙏 Acknowledgments

- **[Foobar2000 SDK](https://www.foobar2000.org/SDK)** by Peter Pawlowski
- **[libcurl](https://curl.se/libcurl/)** by Daniel Stenberg and contributors
- **[JsonCpp](https://github.com/open-source-parsers/jsoncpp)** by Baptiste Lepilleur
- **[Matrix Protocol](https://matrix.org/)** - Open standard for decentralized communication
- **[vcpkg](https://github.com/Microsoft/vcpkg)** - Microsoft's C++ package manager

## 📞 Support

If you encounter issues:

1. **Check the console** - View → Console in Foobar2000
2. **Review troubleshooting** - See sections above
3. **Test progressively** - Use minimal build first
4. **Verify Matrix setup** - Test connection in preferences

---

**Version:** 1.0.0 | **API:** Matrix Client-Server v3 | **Build:** Visual Studio 2022
