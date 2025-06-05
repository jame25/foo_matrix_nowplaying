#include "matrix_client.h"
#include <json/json.h>
#include <sstream>
#include <chrono>

// Include foobar2000 console for debugging
#include "../foobar2000_SDK/foobar2000/SDK/foobar2000.h"

size_t matrix_client::write_callback(void* contents, size_t size, size_t nmemb, std::string* s) {
    size_t total_size = size * nmemb;
    s->append((char*)contents, total_size);
    return total_size;
}

matrix_client::matrix_client() : m_curl(nullptr) {
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

matrix_client::~matrix_client() {
    if (m_curl) {
        curl_easy_cleanup(m_curl);
    }
    curl_global_cleanup();
}

void matrix_client::init(const std::string& homeserver, const std::string& token, const std::string& room_id) {
    m_homeserver = homeserver;
    m_token = token;
    m_room_id = room_id;
    
    if (m_curl) {
        curl_easy_cleanup(m_curl);
    }
    m_curl = curl_easy_init();
}

bool matrix_client::send_message(const std::string& message, bool as_emote) {
    if (!m_curl || m_token.empty() || m_room_id.empty()) {
        return false;
    }
    
    // Generate a unique transaction ID
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    std::string txn_id = "fb2k_" + std::to_string(ms);
    
    // Construct the URL for sending messages (using v3 API)
    std::string url = m_homeserver + "/_matrix/client/v3/rooms/" + m_room_id + "/send/m.room.message/" + txn_id;
    
    // Create JSON payload
    Json::Value root;
    root["msgtype"] = as_emote ? "m.emote" : "m.text";
    root["body"] = message;
    root["format"] = "org.matrix.custom.html";
    root["formatted_body"] = message; // Could add HTML formatting here
    
    Json::StreamWriterBuilder builder;
    std::string json_str = Json::writeString(builder, root);
    
    // Reset CURL for new request
    curl_easy_reset(m_curl);
    
    // Set up CURL
    curl_easy_setopt(m_curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(m_curl, CURLOPT_CUSTOMREQUEST, "PUT");
    curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, json_str.c_str());
    
    // Set headers
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    std::string auth_header = "Authorization: Bearer " + m_token;
    headers = curl_slist_append(headers, auth_header.c_str());
    curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, headers);
    
    // Response handling
    std::string response;
    curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &response);
    
    // Timeout settings
    curl_easy_setopt(m_curl, CURLOPT_TIMEOUT, 10L);
    curl_easy_setopt(m_curl, CURLOPT_CONNECTTIMEOUT, 5L);
    
    // Perform the request
    CURLcode res = curl_easy_perform(m_curl);
    
    // Check HTTP response code
    long http_code = 0;
    curl_easy_getinfo(m_curl, CURLINFO_RESPONSE_CODE, &http_code);
    
    // Log detailed error information
    if (res != CURLE_OK) {
        std::string error_msg = "Matrix Now Playing: CURL error - ";
        error_msg += curl_easy_strerror(res);
        console::print(error_msg.c_str());
    } else if (http_code < 200 || http_code >= 300) {
        std::string error_msg = "Matrix Now Playing: HTTP error " + std::to_string(http_code);
        error_msg += " when sending to: " + url;
        if (!response.empty()) {
            error_msg += " Response: " + response.substr(0, 200); // First 200 chars
        }
        console::print(error_msg.c_str());
    } else {
        console::print("Matrix Now Playing: Message sent successfully");
    }
    
    // Clean up
    curl_slist_free_all(headers);
    
    // Return success only if CURL succeeded AND HTTP response is 2xx
    return (res == CURLE_OK && http_code >= 200 && http_code < 300);
}

bool matrix_client::send_emote(const std::string& action) {
    return send_message(action, true);
}

bool matrix_client::test_connection() {
    if (!m_curl || m_token.empty()) {
        return false;
    }
    
    // Test by getting account info (using v3 API)
    std::string url = m_homeserver + "/_matrix/client/v3/account/whoami";
    
    curl_easy_reset(m_curl);
    curl_easy_setopt(m_curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(m_curl, CURLOPT_HTTPGET, 1L);
    
    struct curl_slist* headers = nullptr;
    std::string auth_header = "Authorization: Bearer " + m_token;
    headers = curl_slist_append(headers, auth_header.c_str());
    curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, headers);
    
    std::string response;
    curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(m_curl, CURLOPT_TIMEOUT, 5L);
    
    CURLcode res = curl_easy_perform(m_curl);
    
    // Check HTTP response code and log errors
    long http_code = 0;
    curl_easy_getinfo(m_curl, CURLINFO_RESPONSE_CODE, &http_code);
    
    if (res != CURLE_OK) {
        std::string error_msg = "Matrix Now Playing: Connection test CURL error - ";
        error_msg += curl_easy_strerror(res);
        console::print(error_msg.c_str());
    } else if (http_code != 200) {
        std::string error_msg = "Matrix Now Playing: Connection test HTTP error " + std::to_string(http_code);
        error_msg += " when testing: " + url;
        if (!response.empty()) {
            error_msg += " Response: " + response.substr(0, 200); // First 200 chars
        }
        console::print(error_msg.c_str());
    } else {
        console::print("Matrix Now Playing: Connection test successful");
    }
    
    curl_slist_free_all(headers);
    
    return (res == CURLE_OK && http_code == 200);
}