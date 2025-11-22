#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <queue>
#include <pthread.h>

class Server;

struct Task {
    Server* server;
    int client_socket;
};

class ThreadPool {
public:
    ThreadPool(size_t num_threads);
    ~ThreadPool();
    
    void enqueue(Server* server, int client_socket);
    void stop();
    
private:
    static void* worker_thread(void* arg);
    
    std::vector<pthread_t> workers_;
    std::queue<Task> tasks_;
    pthread_mutex_t queue_mutex_;
    pthread_cond_t condition_;
    bool stop_;
};

#endif
