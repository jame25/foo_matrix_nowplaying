#include "simple_matrix_client.h"
#include "../foobar2000_SDK/foobar2000/SDK/foobar2000.h"
#include <sstream>
#include <chrono>
#include <iomanip>

// Include Windows headers after foobar2000 SDK to avoid conflicts
#include <windows.h>
#include <wininet.h>

#pragma comment(lib, "wininet.lib")

simple_matrix_client::simple_matrix_client() {
}

simple_matrix_client::~simple_matrix_client() {
}

void simple_matrix_client::init(const std::string& homeserver, const std::string& token, const std::string& room_id) {
    m_homeserver = homeserver;
    m_token = token;
    m_room_id = room_id;
}

std::string simple_matrix_client::generate_txn_id() {
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    return "fb2k_" + std::to_string(ms);
}

std::string simple_matrix_client::create_json_message(const std::string& message, bool as_emote) {
    // Simple JSON creation without external library
    std::string escaped_message = message;
    
    // Basic JSON escaping
    size_t pos = 0;
    while ((pos = escaped_message.find("\"", pos)) != std::string::npos) {
        escaped_message.replace(pos, 1, "\\\"");
        pos += 2;
    }
    while ((pos = escaped_message.find("\n", pos)) != std::string::npos) {
        escaped_message.replace(pos, 1, "\\n");
        pos += 2;
    }
    
    std::stringstream json;
    json << "{";
    json << "\"msgtype\":\"" << (as_emote ? "m.emote" : "m.text") << "\",";
    json << "\"body\":\"" << escaped_message << "\",";
    json << "\"format\":\"org.matrix.custom.html\",";
    json << "\"formatted_body\":\"" << escaped_message << "\"";
    json << "}";
    
    return json.str();
}

std::string simple_matrix_client::url_encode(const std::string& value) {
    std::ostringstream escaped;
    escaped.unsetf(std::ios_base::floatfield);
    escaped.flags(std::ios_base::hex | std::ios_base::uppercase);

    for (char c : value) {
        if (('0' <= c && c <= '9') || ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') ||
            c == '-' || c == '.' || c == '_' || c == '~') {
            escaped << c;
        } else {
            escaped << '%' << std::setw(2) << std::setfill('0') << (int)(unsigned char)c;
        }
    }
    return escaped.str();
}

bool simple_matrix_client::send_http_request(const std::string& url, const std::string& method, const std::string& data, const std::string& headers) {
    HINTERNET hInternet = InternetOpenA("foobar2000-matrix/1.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (!hInternet) {
        console::print("Matrix Now Playing: Failed to initialize WinINet");
        return false;
    }

    // Parse URL
    std::string host, path;
    size_t protocol_end = url.find("://");
    if (protocol_end == std::string::npos) {
        InternetCloseHandle(hInternet);
        return false;
    }
    
    std::string url_without_protocol = url.substr(protocol_end + 3);
    size_t path_start = url_without_protocol.find("/");
    if (path_start != std::string::npos) {
        host = url_without_protocol.substr(0, path_start);
        path = url_without_protocol.substr(path_start);
    } else {
        host = url_without_protocol;
        path = "/";
    }

    HINTERNET hConnect = InternetConnectA(hInternet, host.c_str(), INTERNET_DEFAULT_HTTPS_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
    if (!hConnect) {
        console::print("Matrix Now Playing: Failed to connect to server");
        InternetCloseHandle(hInternet);
        return false;
    }

    DWORD flags = INTERNET_FLAG_SECURE | INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE;
    HINTERNET hRequest = HttpOpenRequestA(hConnect, method.c_str(), path.c_str(), NULL, NULL, NULL, flags, 0);
    if (!hRequest) {
        console::print("Matrix Now Playing: Failed to create HTTP request");
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return false;
    }

    // Add headers
    std::string all_headers = "Content-Type: application/json\r\n";
    all_headers += "Authorization: Bearer " + m_token + "\r\n";
    all_headers += headers;

    BOOL result = HttpSendRequestA(hRequest, all_headers.c_str(), all_headers.length(), 
                                   (LPVOID)data.c_str(), data.length());
    
    if (!result) {
        console::print("Matrix Now Playing: Failed to send HTTP request");
        InternetCloseHandle(hRequest);
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return false;
    }

    // Check response code
    DWORD statusCode = 0;
    DWORD statusCodeSize = sizeof(statusCode);
    HttpQueryInfoA(hRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &statusCode, &statusCodeSize, NULL);

    // Read response
    char buffer[1024];
    DWORD bytesRead;
    std::string response;
    while (InternetReadFile(hRequest, buffer, sizeof(buffer) - 1, &bytesRead) && bytesRead > 0) {
        buffer[bytesRead] = '\0';
        response += buffer;
    }

    InternetCloseHandle(hRequest);
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);

    if (statusCode >= 200 && statusCode < 300) {
        console::print("Matrix Now Playing: Message sent successfully");
        return true;
    } else {
        std::string error_msg = "Matrix Now Playing: HTTP error " + std::to_string(statusCode);
        error_msg += " when sending to: " + url;
        if (!response.empty()) {
            error_msg += " Response: " + response.substr(0, 200);
        }
        console::print(error_msg.c_str());
        return false;
    }
}

bool simple_matrix_client::send_message(const std::string& message, bool as_emote) {
    if (m_token.empty() || m_room_id.empty()) {
        return false;
    }
    
    std::string txn_id = generate_txn_id();
    std::string encoded_room_id = url_encode(m_room_id);
    std::string url = m_homeserver + "/_matrix/client/v3/rooms/" + encoded_room_id + "/send/m.room.message/" + txn_id;
    std::string json_data = create_json_message(message, as_emote);
    
    return send_http_request(url, "PUT", json_data, "");
}

bool simple_matrix_client::send_emote(const std::string& action) {
    return send_message(action, true);
}

bool simple_matrix_client::test_connection() {
    if (m_token.empty()) {
        return false;
    }
    
    std::string url = m_homeserver + "/_matrix/client/v3/account/whoami";
    return send_http_request(url, "GET", "", "");
}
