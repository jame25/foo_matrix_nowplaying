#include "play_callback.h"
#include "preferences.h"
#include "simple_matrix_client.h"
#include <sstream>

// Config variables are declared in preferences.h

play_callback_matrix::play_callback_matrix() : m_running(false) {
    // Don't start thread in constructor - defer until first use
}

play_callback_matrix::~play_callback_matrix() {
    if (m_running) {
        m_running = false;
        m_queue_cv.notify_all();
        if (m_worker_thread.joinable()) {
            m_worker_thread.join();
        }
    }
}

void play_callback_matrix::reinit_client() {
    m_client.init(cfg_matrix_homeserver.get_ptr(),
                  cfg_matrix_token.get_ptr(),
                  cfg_matrix_room_id.get_ptr());
}

void play_callback_matrix::ensure_worker_thread() {
    if (!m_running) {
        m_running = true;
        m_worker_thread = std::thread(&play_callback_matrix::worker_thread, this);
    }
}

void play_callback_matrix::worker_thread() {
    while (m_running) {
        std::unique_lock<std::mutex> lock(m_queue_mutex);
        m_queue_cv.wait(lock, [this] { return !m_message_queue.empty() || !m_running; });
        
        if (!m_running) break;
        
        if (!m_message_queue.empty()) {
            std::string message = m_message_queue.front();
            m_message_queue.pop();
            lock.unlock();
            
            // Retry logic
            for (int retry = 0; retry < 3; retry++) {
                if (m_client.send_message(message, cfg_send_as_action)) {
                    break;
                }
                if (retry < 2) {
                    Sleep(1000); // Wait 1 second before retry
                }
            }
        }
    }
}

std::string play_callback_matrix::format_track_info(metadb_handle_ptr p_track) {
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

void play_callback_matrix::on_playback_starting(play_control::t_track_command p_command, bool p_paused) {
    // Reinit client in case settings changed
    reinit_client();
    ensure_worker_thread();
}

void play_callback_matrix::on_playback_new_track(metadb_handle_ptr p_track) {
    reinit_client();
    if (!cfg_enabled || !p_track.is_valid()) return;
    
    ensure_worker_thread();
    std::string message = format_track_info(p_track);
    
    {
        std::lock_guard<std::mutex> lock(m_queue_mutex);
        m_message_queue.push(message);
    }
    m_queue_cv.notify_one();
}

void play_callback_matrix::on_playback_stop(play_control::t_stop_reason p_reason) {
    if (!cfg_enabled || !cfg_notify_stop) return;
    
    if (p_reason != play_control::stop_reason_starting_another) {
        ensure_worker_thread();
        std::string message;
        switch (p_reason) {
            case play_control::stop_reason_user:
                message = "⏹️ <em>Playback stopped</em>";
                break;
            case play_control::stop_reason_eof:
                message = "⏹️ <em>End of playlist</em>";
                break;
            default:
                message = "⏹️ <em>Playback stopped</em>";
                break;
        }
        
        std::lock_guard<std::mutex> lock(m_queue_mutex);
        m_message_queue.push(message);
        m_queue_cv.notify_one();
    }
}

void play_callback_matrix::on_playback_pause(bool p_state) {
    if (!cfg_enabled || !cfg_notify_pause) return;
    
    ensure_worker_thread();
    std::lock_guard<std::mutex> lock(m_queue_mutex);
    m_message_queue.push(p_state ? "⏸️ <em>Playback paused</em>" : "▶️ <em>Playback resumed</em>");
    m_queue_cv.notify_one();
}

// Empty implementations for unused callbacks
void play_callback_matrix::on_playback_seek(double p_time) {}
void play_callback_matrix::on_playback_edited(metadb_handle_ptr p_track) {}
void play_callback_matrix::on_playback_dynamic_info(const file_info& p_info) {}
void play_callback_matrix::on_playback_dynamic_info_track(const file_info& p_info) {}
void play_callback_matrix::on_playback_time(double p_time) {}
void play_callback_matrix::on_volume_change(float p_new_val) {}
