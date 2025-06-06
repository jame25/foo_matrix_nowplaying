#include "announce_hotkey.h"
#include "simple_matrix_client.h"
#include "preferences.h"
#include <sstream>

static const GUID guid_announce_command = { 0x12345678, 0x1234, 0x5678, { 0x90, 0xab, 0xcd, 0xef, 0x12, 0x34, 0x56, 0x78 } };
static const GUID guid_announce_group = { 0x87654321, 0x4321, 0x8765, { 0x90, 0xab, 0xcd, 0xef, 0x87, 0x65, 0x43, 0x21 } };

static mainmenu_group_popup_factory g_announce_group(guid_announce_group, mainmenu_groups::playback, mainmenu_commands::sort_priority_dontcare, "Matrix Now Playing");

t_uint32 announce_hotkey_command::get_command_count() {
    return cmd_total;
}

GUID announce_hotkey_command::get_command(t_uint32 p_index) {
    switch (p_index) {
        case cmd_announce_current_track:
            return guid_announce_command;
        default:
            uBugCheck();
    }
}

void announce_hotkey_command::get_name(t_uint32 p_index, pfc::string_base& p_out) {
    switch (p_index) {
        case cmd_announce_current_track:
            p_out = "Announce current track to Matrix";
            break;
        default:
            uBugCheck();
    }
}

bool announce_hotkey_command::get_description(t_uint32 p_index, pfc::string_base& p_out) {
    switch (p_index) {
        case cmd_announce_current_track:
            p_out = "Sends the currently playing track info to the configured Matrix room";
            return true;
        default:
            return false;
    }
}

GUID announce_hotkey_command::get_parent() {
    return guid_announce_group;
}

void announce_hotkey_command::execute(t_uint32 p_index, service_ptr_t<service_base> p_callback) {
    switch (p_index) {
        case cmd_announce_current_track:
            announce_current_track();
            break;
        default:
            uBugCheck();
    }
}

void announce_hotkey_command::announce_current_track() {
    static_api_ptr_t<playback_control> pc;
    
    if (!pc->is_playing()) {
        console::print("Matrix Now Playing: No track currently playing");
        return;
    }

    metadb_handle_ptr track;
    if (!pc->get_now_playing(track)) {
        console::print("Matrix Now Playing: Failed to get current track");
        return;
    }

    std::string message = format_current_track(track);
    if (message.empty()) {
        console::print("Matrix Now Playing: Failed to format track info");
        return;
    }

    simple_matrix_client client;
    client.init(cfg_matrix_homeserver.get_ptr(),
                cfg_matrix_token.get_ptr(), 
                cfg_matrix_room_id.get_ptr());

    // Use the configured send_as_action setting
    if (client.send_message(message, cfg_send_as_action)) {
        console::print("Matrix Now Playing: Successfully announced current track");
    } else {
        console::print("Matrix Now Playing: Failed to announce track to Matrix");
    }
}

std::string announce_hotkey_command::format_current_track(metadb_handle_ptr p_track) {
    if (!p_track.is_valid()) return "Unknown Track";
    
    pfc::string8 artist, title, album;
    static_api_ptr_t<titleformat_compiler> compiler;
    
    try {
        service_ptr_t<titleformat_object> script_artist, script_title, script_album;
        
        compiler->compile_safe(script_artist, "[%artist%]");
        compiler->compile_safe(script_title, "[%title%]");
        compiler->compile_safe(script_album, "[%album%]");
        
        p_track->format_title(nullptr, artist, script_artist, nullptr);
        p_track->format_title(nullptr, title, script_title, nullptr);
        p_track->format_title(nullptr, album, script_album, nullptr);
    } catch (...) {
        // Fallback to basic info
        file_info_impl info;
        if (p_track->get_info(info)) {
            const char* artist_str = info.meta_get("artist", 0);
            const char* title_str = info.meta_get("title", 0);
            const char* album_str = info.meta_get("album", 0);
            
            if (artist_str) artist = artist_str;
            if (title_str) title = title_str;
            if (album_str) album = album_str;
        }
    }
    
    // Format the message using cfg_message_format
    std::stringstream ss;
    service_ptr_t<titleformat_object> script;
    compiler->compile_safe(script, cfg_message_format.get_ptr());
    
    pfc::string8 formatted_string;
    p_track->format_title(nullptr, formatted_string, script, nullptr);

    // Basic HTML escaping for the final message to avoid issues in Matrix
    std::string html_escaped_string = formatted_string.get_ptr();
    
    size_t pos = 0;
    while ((pos = html_escaped_string.find("&", pos)) != std::string::npos) {
        html_escaped_string.replace(pos, 1, "&amp;");
        pos += 5;
    }
    while ((pos = html_escaped_string.find("<", pos)) != std::string::npos) {
        html_escaped_string.replace(pos, 1, "&lt;");
        pos += 4;
    }
    while ((pos = html_escaped_string.find(">", pos)) != std::string::npos) {
        html_escaped_string.replace(pos, 1, "&gt;");
        pos += 4;
    }
    
    return html_escaped_string;
}

static mainmenu_commands_factory_t<announce_hotkey_command> g_announce_hotkey_factory;