#include "thread_pool.h"
#include "server.h"
#include <iostream>

void* ThreadPool::worker_thread(void* arg) {
    ThreadPool* pool = static_cast<ThreadPool*>(arg);
    
    while (true) {
        Task task;
        
        pthread_mutex_lock(&pool->queue_mutex_);
        
        while (pool->tasks_.empty() && !pool->stop_) {
            pthread_cond_wait(&pool->condition_, &pool->queue_mutex_);
        }
        
        if (pool->stop_ && pool->tasks_.empty()) {
            pthread_mutex_unlock(&pool->queue_mutex_);
            return NULL;
        }
        
        task = pool->tasks_.front();
        pool->tasks_.pop();
        
        pthread_mutex_unlock(&pool->queue_mutex_);
        
        task.server->handle_client(task.client_socket);
    }
    
    return NULL;
}

ThreadPool::ThreadPool(size_t num_threads) : stop_(false) {
    pthread_mutex_init(&queue_mutex_, NULL);
    pthread_cond_init(&condition_, NULL);
    
    for (size_t i = 0; i < num_threads; ++i) {
        pthread_t thread;
        pthread_create(&thread, NULL, worker_thread, this);
        workers_.push_back(thread);
    }
}

ThreadPool::~ThreadPool() {
    stop();
    pthread_mutex_destroy(&queue_mutex_);
    pthread_cond_destroy(&condition_);
}

void ThreadPool::enqueue(Server* server, int client_socket) {
    Task task;
    task.server = server;
    task.client_socket = client_socket;
    
    pthread_mutex_lock(&queue_mutex_);
    if (stop_) {
        pthread_mutex_unlock(&queue_mutex_);
        return;
    }
    tasks_.push(task);
    pthread_mutex_unlock(&queue_mutex_);
    
    pthread_cond_signal(&condition_);
}

void ThreadPool::stop() {
    pthread_mutex_lock(&queue_mutex_);
    stop_ = true;
    pthread_mutex_unlock(&queue_mutex_);
    
    pthread_cond_broadcast(&condition_);
    
    for (size_t i = 0; i < workers_.size(); ++i) {
        pthread_join(workers_[i], NULL);
    }
}
