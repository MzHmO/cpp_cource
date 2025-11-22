#ifndef SERVER_H
#define SERVER_H

#include <string>

class ThreadPool;

class Server {
public:
    Server(const std::string& host, int port, const std::string& directory);
    ~Server();
    
    bool start();
    void stop();
    void run();
    void handle_client(int client_socket);
    
private:
    std::string build_response(int status_code, const std::string& content, const std::string& content_type = "text/html");
    std::string get_mime_type(const std::string& file_path);
    bool file_exists(const std::string& path);
    std::string read_file(const std::string& path);
    
    std::string host_;
    int port_;
    std::string directory_;
    int server_socket_;
    bool running_;
    ThreadPool* thread_pool_;
};

#endif
