#include <iostream>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <getopt.h>
#include "server.h"

void print_usage(const char* program_name) {
    std::cout << "Usage: " << program_name << " -h <ip> -p <port> -d <directory>" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -h <ip>        Host IP address to bind (default: 0.0.0.0)" << std::endl;
    std::cout << "  -p <port>      Port number (default: 8080)" << std::endl;
    std::cout << "  -d <directory> Directory to serve files from" << std::endl;
}

int main(int argc, char* argv[]) {
    std::string host = "0.0.0.0";
    int port = 8080;
    std::string directory;
    
    int opt;
    while ((opt = getopt(argc, argv, "h:p:d:")) != -1) {
        switch (opt) {
            case 'h':
                host = optarg;
                break;
            case 'p':
                port = std::atoi(optarg);
                break;
            case 'd':
                directory = optarg;
                break;
            default:
                print_usage(argv[0]);
                return 1;
        }
    }
    
    if (directory.empty()) {
        std::cerr << "Error: Directory parameter is required" << std::endl;
        print_usage(argv[0]);
        return 1;
    }
    
    if (port <= 0 || port > 65535) {
        std::cerr << "Error: Invalid port number" << std::endl;
        return 1;
    }
    
    std::cout << "Starting server on " << host << ":" << port << std::endl;
    std::cout << "Serving files from: " << directory << std::endl;
    
    Server server(host, port, directory);
    
    if (!server.start()) {
        std::cerr << "Failed to start server" << std::endl;
        return 1;
    }
    
    std::cout << "Server started successfully. Press Ctrl+C to stop." << std::endl;
    
    server.run();
    
    return 0;
}
