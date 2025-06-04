#pragma once

#include "../foobar2000_SDK/foobar2000/SDK/foobar2000.h"
#include "../foobar2000_SDK/foobar2000/SDK/preferences_page.h"  // Base preferences_page interface
#include "resource.h"
#include <string>

// Configuration GUIDs
extern const GUID guid_cfg_matrix_homeserver;
extern const GUID guid_cfg_matrix_token;
extern const GUID guid_cfg_matrix_room_id;
extern const GUID guid_cfg_enabled;
extern const GUID guid_cfg_notify_pause;
extern const GUID guid_cfg_notify_stop;
extern const GUID guid_cfg_message_format;
extern const GUID guid_matrix_preferences_page;

// Configuration variables
extern cfg_string cfg_matrix_homeserver;
extern cfg_string cfg_matrix_token;
extern cfg_string cfg_matrix_room_id;
extern cfg_bool cfg_enabled;
extern cfg_bool cfg_notify_pause;
extern cfg_bool cfg_notify_stop;
extern cfg_string cfg_message_format;

// Preferences page
class matrix_preferences : public preferences_page_instance {
private:
    HWND m_hwnd;
    preferences_page_callback::ptr m_callback;
    
    bool m_homeserver_changed;
    bool m_token_changed;
    bool m_room_id_changed;
    bool m_enabled_changed;
    bool m_notify_pause_changed;
    bool m_notify_stop_changed;
    bool m_message_format_changed;
    
    void update_ui();
    void apply_settings();
    void on_change();
    
public:
    matrix_preferences(HWND parent, preferences_page_callback::ptr callback);
    
    HWND get_wnd() override;
    t_uint32 get_state() override;
    void apply() override;
    void reset() override;
    
    static INT_PTR CALLBACK dialog_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
};

class matrix_preferences_page : public preferences_page_v3 {
public:
    const char* get_name() override;
    GUID get_guid() override;
    GUID get_parent_guid() override;
    
    double get_sort_priority() override {
        return 0;
    }
    
    preferences_page_instance::ptr instantiate(HWND parent, preferences_page_callback::ptr callback) override {
        return new service_impl_t<matrix_preferences>(parent, callback);
    }
};
