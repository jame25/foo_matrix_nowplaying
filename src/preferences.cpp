#include "preferences.h"
#include "simple_matrix_client.h"
#include "../foobar2000_SDK/foobar2000/SDK/preferences_page.h"
#include "../foobar2000_SDK/foobar2000/SDK/foobar2000.h"
#include <windowsx.h>
#include <Uxtheme.h>
#include "../foobar2000_SDK/libPPUI/DarkMode.h"

#pragma comment(lib, "UxTheme.lib")

// Configuration GUIDs
const GUID guid_cfg_matrix_homeserver = { 0x87654321, 0x4321, 0x8765, { 0x43, 0x21, 0x87, 0x65, 0x43, 0x21, 0x87, 0x65 } };
const GUID guid_cfg_matrix_token = { 0x11223344, 0x5566, 0x7788, { 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00 } };
const GUID guid_cfg_matrix_room_id = { 0xAABBCCDD, 0xEEFF, 0x0011, { 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99 } };
const GUID guid_cfg_enabled = { 0x12121212, 0x3434, 0x5656, { 0x78, 0x78, 0x90, 0x90, 0xAB, 0xAB, 0xCD, 0xCD } };
const GUID guid_cfg_notify_pause = { 0x56565656, 0x7878, 0x9090, { 0xAB, 0xAB, 0xCD, 0xCD, 0xEF, 0xEF, 0x01, 0x01 } };
const GUID guid_cfg_notify_stop = { 0x78787878, 0x9090, 0xABAB, { 0xCD, 0xCD, 0xEF, 0xEF, 0x01, 0x01, 0x23, 0x23 } };
const GUID guid_cfg_message_format = { 0x90909090, 0xABAB, 0xCDCD, { 0xEF, 0xEF, 0x01, 0x01, 0x23, 0x23, 0x45, 0x45 } };
const GUID guid_matrix_preferences_page = { 0x12345678, 0x1234, 0x1234, { 0x12, 0x34, 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc } };

// Configuration variables
cfg_string cfg_matrix_homeserver(guid_cfg_matrix_homeserver, "https://matrix.org");
cfg_string cfg_matrix_token(guid_cfg_matrix_token, "");
cfg_string cfg_matrix_room_id(guid_cfg_matrix_room_id, "");
cfg_bool cfg_enabled(guid_cfg_enabled, true);
cfg_bool cfg_notify_pause(guid_cfg_notify_pause, true);
cfg_bool cfg_notify_stop(guid_cfg_notify_stop, true);
cfg_string cfg_message_format(guid_cfg_message_format, "🎵 Now Playing: %artist% - %title%");

// Component GUID
static const GUID g_guid_matrix_nowplaying = 
{ 0x12345678, 0x1234, 0x5678, { 0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78 } };

matrix_preferences::matrix_preferences(HWND parent, preferences_page_callback::ptr callback)
    : m_hwnd(NULL), m_callback(callback),
      m_homeserver_changed(false), m_token_changed(false), m_room_id_changed(false),
      m_enabled_changed(false), m_notify_pause_changed(false), m_notify_stop_changed(false), m_message_format_changed(false),
      m_darkMode(DarkMode::QueryUserOption()) {
    
    // Create the dialog
    m_hwnd = CreateDialogParam(core_api::get_my_instance(), 
                         MAKEINTRESOURCE(IDD_MATRIX_PREFS), 
                         parent, 
                         dialog_proc,
                         (LPARAM)this);
    
    if (m_hwnd) {
        ShowWindow(m_hwnd, SW_SHOW);
    }
}

HWND matrix_preferences::get_wnd() {
    return m_hwnd;
}

t_uint32 matrix_preferences::get_state() {
    t_uint32 state = preferences_state::resettable;
    state |= preferences_state::dark_mode_supported;
    if (m_homeserver_changed || m_token_changed || m_room_id_changed || 
        m_enabled_changed || m_notify_pause_changed || m_notify_stop_changed || m_message_format_changed) {
        state |= preferences_state::changed;
    }
    return state;
}

void matrix_preferences::apply() {
    console::print("Matrix preferences: Applying settings...");
    apply_settings();
    
    // Reset change flags
    m_homeserver_changed = false;
    m_token_changed = false;
    m_room_id_changed = false;
    m_enabled_changed = false;
    m_notify_pause_changed = false;
    m_notify_stop_changed = false;
    m_message_format_changed = false;
    console::print("Matrix preferences: Settings applied successfully");
}

void matrix_preferences::reset() {
    cfg_matrix_homeserver = "https://matrix.org";
    cfg_matrix_token = "";
    cfg_matrix_room_id = "";
    cfg_enabled = true;
    cfg_notify_pause = true;
    cfg_notify_stop = true;
    cfg_message_format = "🎵 Now Playing: %artist% - %title%";
    
    update_ui();
    on_change();
}

void matrix_preferences::update_ui() {
    update_ui_with_hwnd(m_hwnd);
}

void matrix_preferences::update_ui_with_hwnd(HWND hwnd) {
    console::printf("Matrix: Loading homeserver: %s", cfg_matrix_homeserver.get_ptr());
    const char* token = cfg_matrix_token.get_ptr();
    console::printf("Matrix: Loading token: %s", token && strlen(token) > 0 ? "***MASKED***" : "(empty)");
    console::printf("Matrix: Loading room ID: %s", cfg_matrix_room_id.get_ptr());
    
    console::printf("Matrix: HWND is %s", hwnd ? "valid" : "NULL");
    
    BOOL result1 = SetDlgItemTextA(hwnd, IDC_HOMESERVER, cfg_matrix_homeserver.get_ptr());
    console::printf("Matrix: SetDlgItemTextA homeserver result: %d", result1);
    
    BOOL result2 = SetDlgItemTextA(hwnd, IDC_TOKEN, cfg_matrix_token.get_ptr());
    console::printf("Matrix: SetDlgItemTextA token result: %d", result2);
    
    BOOL result3 = SetDlgItemTextA(hwnd, IDC_ROOM_ID, cfg_matrix_room_id.get_ptr());
    console::printf("Matrix: SetDlgItemTextA room ID result: %d", result3);
    
    CheckDlgButton(hwnd, IDC_ENABLED, cfg_enabled ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hwnd, IDC_NOTIFY_PAUSE, cfg_notify_pause ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hwnd, IDC_NOTIFY_STOP, cfg_notify_stop ? BST_CHECKED : BST_UNCHECKED);
    SetDlgItemTextA(hwnd, IDC_MESSAGE_FORMAT, cfg_message_format.get_ptr());
}

void matrix_preferences::apply_settings() {
    char buffer[1024];
    
    GetDlgItemTextA(m_hwnd, IDC_HOMESERVER, buffer, sizeof(buffer));
    console::printf("Matrix: Setting homeserver to: %s", buffer);
    cfg_matrix_homeserver = buffer;
    
    GetDlgItemTextA(m_hwnd, IDC_TOKEN, buffer, sizeof(buffer));
    console::printf("Matrix: Setting token to: %s", strlen(buffer) > 0 ? "***MASKED***" : "(empty)");
    cfg_matrix_token = buffer;
    
    GetDlgItemTextA(m_hwnd, IDC_ROOM_ID, buffer, sizeof(buffer));
    console::printf("Matrix: Setting room ID to: %s", buffer);
    cfg_matrix_room_id = buffer;
    
    GetDlgItemTextA(m_hwnd, IDC_MESSAGE_FORMAT, buffer, sizeof(buffer));
    cfg_message_format = buffer;
    
    cfg_enabled = IsDlgButtonChecked(m_hwnd, IDC_ENABLED) == BST_CHECKED;
    cfg_notify_pause = IsDlgButtonChecked(m_hwnd, IDC_NOTIFY_PAUSE) == BST_CHECKED;
    cfg_notify_stop = IsDlgButtonChecked(m_hwnd, IDC_NOTIFY_STOP) == BST_CHECKED;
}

void matrix_preferences::on_change() {
    if (m_callback.is_valid()) {
        m_callback->on_state_changed();
    }
}

INT_PTR CALLBACK matrix_preferences::dialog_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    matrix_preferences* instance = (matrix_preferences*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    
    switch (msg) {
    case WM_INITDIALOG:
        // Store the instance pointer
        instance = (matrix_preferences*)lparam;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)instance);
        
        // Initialize controls
        if (instance) {
            instance->update_ui_with_hwnd(hwnd);
        }
        
        // Return TRUE to let dialog manager set focus to first control
        return TRUE;
        
    case WM_COMMAND:
        if (instance) {
            switch (LOWORD(wparam)) {
            case IDC_HOMESERVER:
                if (HIWORD(wparam) == EN_CHANGE) {
                    instance->m_homeserver_changed = true;
                    instance->on_change();
                }
                break;
                
            case IDC_TOKEN:
                if (HIWORD(wparam) == EN_CHANGE) {
                    instance->m_token_changed = true;
                    instance->on_change();
                }
                break;
                
            case IDC_ROOM_ID:
                if (HIWORD(wparam) == EN_CHANGE) {
                    instance->m_room_id_changed = true;
                    instance->on_change();
                }
                break;
                
            case IDC_ENABLED:
                if (HIWORD(wparam) == BN_CLICKED) {
                    instance->m_enabled_changed = true;
                    instance->on_change();
                }
                break;
                
            case IDC_NOTIFY_PAUSE:
                if (HIWORD(wparam) == BN_CLICKED) {
                    instance->m_notify_pause_changed = true;
                    instance->on_change();
                }
                break;
                
            case IDC_NOTIFY_STOP:
                if (HIWORD(wparam) == BN_CLICKED) {
                    instance->m_notify_stop_changed = true;
                    instance->on_change();
                }
                break;
                
            case IDC_MESSAGE_FORMAT:
                if (HIWORD(wparam) == EN_CHANGE) {
                    instance->m_message_format_changed = true;
                    instance->on_change();
                }
                break;
                
            case IDC_TEST_CONNECTION:
                if (HIWORD(wparam) == BN_CLICKED) {
                    // Test connection
                    instance->apply_settings();
                    
                    simple_matrix_client test_client;
                    test_client.init(cfg_matrix_homeserver.get_ptr(),
                                   cfg_matrix_token.get_ptr(),
                                   cfg_matrix_room_id.get_ptr());
                    
                    if (test_client.test_connection()) {
                        MessageBoxA(hwnd, "Connection successful!", "Matrix Connection Test", MB_OK | MB_ICONINFORMATION);
                        
                        // Optionally send a test message
                        if (cfg_matrix_room_id.get_length() > 0) {
                            test_client.send_message("🎵 Foobar2000 Matrix component connected!");
                        }
                    } else {
                        MessageBoxA(hwnd, "Connection failed! Please check your settings.", "Matrix Connection Test", MB_OK | MB_ICONERROR);
                    }
                }
                break;
            }
        }
        return TRUE;
    }
    
    return FALSE;
}

const char* matrix_preferences_page::get_name() {
    return "Matrix Now Playing";
}

GUID matrix_preferences_page::get_guid() {
    return guid_matrix_preferences_page;
}

GUID matrix_preferences_page::get_parent_guid() {
    return preferences_page::guid_tools;
}

// Factory registration is in main.cpp
