#pragma once

#include "../foobar2000_SDK/foobar2000/SDK/foobar2000.h"

class announce_hotkey_command : public mainmenu_commands {
public:
    enum {
        cmd_announce_current_track = 0,
        cmd_total
    };

    t_uint32 get_command_count() override;
    GUID get_command(t_uint32 p_index) override;
    void get_name(t_uint32 p_index, pfc::string_base& p_out) override;
    bool get_description(t_uint32 p_index, pfc::string_base& p_out) override;
    GUID get_parent() override;
    void execute(t_uint32 p_index, service_ptr_t<service_base> p_callback) override;

private:
    void announce_current_track();
    std::string format_current_track(metadb_handle_ptr p_track);
};