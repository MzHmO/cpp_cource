#ifndef HTTP_PARSER_H
#define HTTP_PARSER_H

#include <string>
#include <unordered_map>

struct HttpRequest {
    std::string method;
    std::string path;
    std::string version;
    std::unordered_map<std::string, std::string> headers;
};

class HttpParser {
public:
    static bool parse_request(const std::string& request, HttpRequest& parsed_request);
    static std::string url_decode(const std::string& str);
    
private:
    static std::string to_lower(const std::string& str);
};

#endif
