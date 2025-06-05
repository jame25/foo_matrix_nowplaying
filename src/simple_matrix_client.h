#pragma once

#include <string>

// Forward declarations to avoid including Windows headers in header file
typedef void* HINTERNET;

class simple_matrix_client {
private:
    std::string m_homeserver;
    std::string m_token;
    std::string m_room_id;
    
public:
    simple_matrix_client();
    ~simple_matrix_client();
    
    void init(const std::string& homeserver, const std::string& token, const std::string& room_id);
    bool send_message(const std::string& message, bool as_emote = false);
    bool send_emote(const std::string& action);
    bool test_connection();
    std::string get_room_id() const { return m_room_id; }
    
private:
    std::string create_json_message(const std::string& message, bool as_emote = false);
    bool send_http_request(const std::string& url, const std::string& method, const std::string& data, const std::string& headers);
    std::string url_encode(const std::string& value);
    std::string generate_txn_id();
};
