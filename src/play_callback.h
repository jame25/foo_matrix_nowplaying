#pragma once

#include "../foobar2000_SDK/foobar2000/SDK/foobar2000.h"
#include "simple_matrix_client.h"
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>

class play_callback_matrix : public play_callback_static {
private:
    simple_matrix_client m_client;
    std::thread m_worker_thread;
    std::queue<std::string> m_message_queue;
    std::mutex m_queue_mutex;
    std::condition_variable m_queue_cv;
    bool m_running;
    
    void worker_thread();
    std::string format_track_info(metadb_handle_ptr p_track);
    
public:
    play_callback_matrix();
    ~play_callback_matrix();
    
    void reinit_client();
    void ensure_worker_thread();
    
    // play_callback methods
    unsigned get_flags() override {
        return flag_on_playback_new_track | flag_on_playback_stop | flag_on_playback_pause;
    }
    
    void on_playback_starting(play_control::t_track_command p_command, bool p_paused) override;
    void on_playback_new_track(metadb_handle_ptr p_track) override;
    void on_playback_stop(play_control::t_stop_reason p_reason) override;
    void on_playback_seek(double p_time) override;
    void on_playback_pause(bool p_state) override;
    void on_playback_edited(metadb_handle_ptr p_track) override;
    void on_playback_dynamic_info(const file_info& p_info) override;
    void on_playback_dynamic_info_track(const file_info& p_info) override;
    void on_playback_time(double p_time) override;
    void on_volume_change(float p_new_val) override;
};