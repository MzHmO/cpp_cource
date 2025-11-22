#include "http_parser.h"
#include <sstream>
#include <algorithm>
#include <cctype>

bool HttpParser::parse_request(const std::string& request, HttpRequest& parsed_request) {
    std::istringstream stream(request);
    std::string line;
        if (!std::getline(stream, line)) {
        return false;
    }
    
    std::istringstream first_line(line);
    if (!(first_line >> parsed_request.method >> parsed_request.path >> parsed_request.version)) {
        return false;
    }
    
    parsed_request.path = url_decode(parsed_request.path);
    
    while (std::getline(stream, line) && line != "\r") {
        size_t colon_pos = line.find(':');
        if (colon_pos != std::string::npos) {
            std::string key = line.substr(0, colon_pos);
            std::string value = line.substr(colon_pos + 1);
            
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            parsed_request.headers[to_lower(key)] = value;
        }
    }
    
    return true;
}

std::string HttpParser::url_decode(const std::string& str) {
    std::string result;
    for (size_t i = 0; i < str.length(); i++) {
        if (str[i] == '%' && i + 2 < str.length()) {
            int hex_value;
            std::istringstream hex_stream(str.substr(i + 1, 2));
            if (hex_stream >> std::hex >> hex_value) {
                result += static_cast<char>(hex_value);
                i += 2;
            } else {
                result += str[i];
            }
        } else if (str[i] == '+') {
            result += ' ';
        } else {
            result += str[i];
        }
    }
    return result;
}

std::string HttpParser::to_lower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), 
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}
