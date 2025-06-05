#pragma once

#include <string>
#include <curl/curl.h>

class matrix_client {
private:
    std::string m_homeserver;
    std::string m_token;
    std::string m_room_id;
    CURL* m_curl;
    
public:
    matrix_client();
    ~matrix_client();
    
    void init(const std::string& homeserver, const std::string& token, const std::string& room_id);
    bool send_message(const std::string& message, bool as_emote = false);
    bool send_emote(const std::string& action);
    bool test_connection();
    
private:
    static size_t write_callback(void* contents, size_t size, size_t nmemb, std::string* s);
};