#include "server.h"
#include "http_parser.h"
#include "thread_pool.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <sstream>
#include <fstream>
#include <sys/stat.h>

Server::Server(const std::string& host, int port, const std::string& directory)
    : host_(host), port_(port), directory_(directory), server_socket_(-1), running_(false) {
    thread_pool_ = new ThreadPool(4);
}

Server::~Server() {
    stop();
    delete thread_pool_;
}

bool Server::start() {
    server_socket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_ == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return false;
    }
    
    int opt = 1;
    if (setsockopt(server_socket_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cerr << "setsockopt failed" << std::endl;
        close(server_socket_);
        return false;
    }
    
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_);
    
    if (host_ == "0.0.0.0") {
        server_addr.sin_addr.s_addr = INADDR_ANY;
    } else {
        server_addr.sin_addr.s_addr = inet_addr(host_.c_str());
    }
    
    if (bind(server_socket_, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        close(server_socket_);
        return false;
    }
    
    if (listen(server_socket_, 10) < 0) {
        std::cerr << "Listen failed" << std::endl;
        close(server_socket_);
        return false;
    }
    
    running_ = true;
    return true;
}

void Server::stop() {
    running_ = false;
    if (server_socket_ != -1) {
        close(server_socket_);
        server_socket_ = -1;
    }
    if (thread_pool_) {
        thread_pool_->stop();
    }
}

void Server::run() {
    while (running_) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        
        int client_socket = accept(server_socket_, (struct sockaddr*)&client_addr, &client_len);
        
        if (client_socket < 0) {
            if (running_) {
                std::cerr << "Accept failed" << std::endl;
            }
            continue;
        }
        
        thread_pool_->enqueue(this, client_socket);
    }
}

void Server::handle_client(int client_socket) {
    char buffer[4096];
    ssize_t bytes_read = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        std::string request_str(buffer);
        
        HttpRequest request;
        if (HttpParser::parse_request(request_str, request)) {
            if (request.method == "GET") {
                std::string file_path = directory_ + request.path;
                if (file_path[file_path.length() - 1] == '/') {
                    file_path += "index.html";
                }
                
                if (file_exists(file_path)) {
                    std::string content = read_file(file_path);
                    std::string mime_type = get_mime_type(file_path);
                    std::string response = build_response(200, content, mime_type);
                    send(client_socket, response.c_str(), response.length(), 0);
                } else {
                    std::string not_found = "<html><body><h1>404 Not Found</h1></body></html>";
                    std::string response = build_response(404, not_found);
                    send(client_socket, response.c_str(), response.length(), 0);
                }
            } else {
                std::string not_allowed = "<html><body><h1>405 Method Not Allowed</h1></body></html>";
                std::string response = build_response(405, not_allowed);
                send(client_socket, response.c_str(), response.length(), 0);
            }
        } else {
            std::string bad_request = "<html><body><h1>400 Bad Request</h1></body></html>";
            std::string response = build_response(400, bad_request);
            send(client_socket, response.c_str(), response.length(), 0);
        }
    }
    
    close(client_socket);
}

std::string Server::build_response(int status_code, const std::string& content, const std::string& content_type) {
    std::string status_text;
    switch (status_code) {
        case 200: status_text = "OK"; break;
        case 404: status_text = "Not Found"; break;
        case 405: status_text = "Method Not Allowed"; break;
        case 400: status_text = "Bad Request"; break;
        default: status_text = "Unknown"; break;
    }
    
    std::stringstream response;
    response << "HTTP/1.0 " << status_code << " " << status_text << "\r\n";
    response << "Content-Type: " << content_type << "\r\n";
    response << "Content-Length: " << content.length() << "\r\n";
    response << "Connection: close\r\n";
    response << "\r\n";
    response << content;
    
    return response.str();
}

std::string Server::get_mime_type(const std::string& file_path) {
    size_t dot_pos = file_path.find_last_of('.');
    if (dot_pos != std::string::npos) {
        std::string ext = file_path.substr(dot_pos + 1);
        if (ext == "html" || ext == "htm") return "text/html";
        if (ext == "css") return "text/css";
        if (ext == "js") return "application/javascript";
        if (ext == "jpg" || ext == "jpeg") return "image/jpeg";
        if (ext == "png") return "image/png";
        if (ext == "gif") return "image/gif";
        if (ext == "txt") return "text/plain";
    }
    return "text/html";
}

bool Server::file_exists(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

std::string Server::read_file(const std::string& path) {
    std::ifstream file(path.c_str(), std::ios::binary);
    if (!file) {
        return "";
    }
    
    std::string content;
    file.seekg(0, std::ios::end);
    content.resize(file.tellg());
    file.seekg(0, std::ios::beg);
    file.read(&content[0], content.size());
    
    return content;
}
