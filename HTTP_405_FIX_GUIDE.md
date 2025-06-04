# Matrix Now Playing HTTP 405 Error Fix

## Problem Identified

The HTTP 405 "Method Not Allowed" error was caused by two main issues:

1. **Deprecated API Endpoints**: The Matrix client was using `/r0/` API endpoints which many Matrix servers no longer support
2. **Incomplete Build**: The `minimal-working.bat` script was only building the basic component without Matrix functionality

## Fixes Applied

### 1. Updated Matrix API Endpoints

**Changed in `src/matrix_client.cpp`:**
- Line 45: `/r0/` → `/v3/` for message sending endpoint
- Line 96: `/r0/` → `/v3/` for connection test endpoint

The Matrix Client-Server API v3 is the current stable version and should be supported by all modern Matrix servers.

### 2. Enhanced Error Handling

**Added to `src/matrix_client.cpp`:**
- Detailed HTTP response code checking
- Console logging for debugging HTTP errors
- Better error messages showing the exact URL and response

### 3. Complete Build Script

**Created `build-matrix-complete.bat`:**
- Compiles all necessary source files:
  - `main.cpp` (component entry point)
  - `matrix_client.cpp` (HTTP client with fixes)
  - `play_callback.cpp` (track change notifications)
  - `preferences.cpp` (configuration UI)
- Links with required dependencies:
  - foobar2000 SDK libraries
  - libcurl (for HTTP requests)
  - jsoncpp (for JSON handling)
  - System libraries (ws2_32, crypt32, etc.)

## How to Build and Test

### 1. Build the Complete Component

```batch
# Run from Visual Studio Developer Command Prompt
build-matrix-complete.bat
```

This will create `output\foo_matrix_nowplaying.dll` with all Matrix functionality included.

### 2. Install and Configure

1. Copy `output\foo_matrix_nowplaying.dll` to your foobar2000 `components` folder
2. Restart foobar2000
3. Go to **File → Preferences → Components → Matrix Now Playing**
4. Configure:
   - **Homeserver**: Your Matrix server URL (e.g., `https://matrix.org`)
   - **Access Token**: Your Matrix access token
   - **Room ID**: The room where notifications should be sent (e.g., `!roomid:matrix.org`)
5. Click **Test Connection** to verify settings

### 3. Monitor for Errors

Check the foobar2000 console (**View → Console**) for detailed error messages:
- `Matrix Now Playing: Connection test successful` - Good!
- `Matrix Now Playing: HTTP error 405` - Still having issues
- `Matrix Now Playing: Message sent successfully` - Working correctly

## Common Matrix API Issues

### HTTP 405 Method Not Allowed
- **Cause**: Using deprecated `/r0/` endpoints
- **Fix**: Updated to `/v3/` endpoints ✅

### HTTP 401 Unauthorized
- **Cause**: Invalid or expired access token
- **Fix**: Generate a new access token from your Matrix client

### HTTP 403 Forbidden
- **Cause**: Bot doesn't have permission to send messages to the room
- **Fix**: Invite the bot to the room and give it appropriate permissions

### HTTP 404 Not Found
- **Cause**: Invalid room ID or homeserver URL
- **Fix**: Verify the room ID format (should start with `!`) and homeserver URL

## Testing the Fix

1. Build with the new script: `build-matrix-complete.bat`
2. Install the component in foobar2000
3. Configure Matrix settings in preferences
4. Use the "Test Connection" button to verify API connectivity
5. Play a track to test automatic notifications
6. Check the console for any error messages

The component should now successfully connect to Matrix servers using the v3 API and send track notifications without HTTP 405 errors.