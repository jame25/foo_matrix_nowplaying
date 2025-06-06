// main.cpp - Entry point for the Foobar2000 Matrix Now Playing component

#include "../foobar2000_SDK/foobar2000/SDK/foobar2000.h"
#include "play_callback.h"
#include "preferences.h"
#include <windows.h>

// Component's DLL instance handle
HINSTANCE g_hIns = NULL;

// DLL entry point
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        g_hIns = hModule;
        DisableThreadLibraryCalls(hModule);
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

// Component version declaration using the proper SDK macro
DECLARE_COMPONENT_VERSION(
    "Matrix Now Playing",
    "1.0.3",
    "Sends Now Playing notifications to Matrix chat.\n"
    "Configure your Matrix homeserver, access token, and room ID in preferences.\n\n"
    "Author: jame25\n"
    "Build date: " __DATE__ "\n\n"
    "This component sends track information to a Matrix room when tracks change in foobar2000."
);

// Validate component compatibility using the proper SDK macro
VALIDATE_COMPONENT_FILENAME("foo_matrix_nowplaying.dll");

// Initialization handler to verify the component loads and initialize services
class matrix_init : public initquit {
public:
    void on_init() override {
        console::print("Matrix Now Playing: Component loaded successfully");
    }
    
    void on_quit() override {
        console::print("Matrix Now Playing: Component unloading");
    }
};

// Service factory registrations using proper SDK macros
static initquit_factory_t<matrix_init> g_matrix_init_factory;
static play_callback_static_factory_t<play_callback_matrix> g_play_callback_matrix_factory;
static preferences_page_factory_t<matrix_preferences_page> g_matrix_preferences_page_factory;
