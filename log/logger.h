#ifndef LOG_H
#define LOG_H
#include <memory>
#include <string>
#include <cstring>
#include <vector>
#include <iostream>
#include <fstream>
#include <pthread.h>
#include <exception>
#include <ctime>
#include <cstdio>
#include <cstdarg>
#include "block_queue.h"
#include "../locker/locker.h"
class Logger {
public:
enum LOG_LEVEL {INFO = 0, DEBUG, WARNING, ERROR};
public:
    static Logger *get_instance() {
        static Logger instance;
        return &instance;
    }
    void init(const char *file_name, int write_buf_size = 4096, int max_line = 100000, int bq_size = 1000);
    void write_log(LOG_LEVEL, const char *format, ...);
    void flush();
    void *async_write();
private:
    Logger();
    virtual ~Logger();
    static void *flush_log_thread(void *args);
    
    void new_log_file(bool is_new_day);
    
private:
    std::string f_name;
    int write_buf_size;
    bool is_init;
    
    BlockQueue<std::string> msg_queue;
    int cur_day;
    int log_per_day;
    int cur_line;
    int max_line_perfile;
    std::ofstream cur_ofs;
    pthread_t write_thread;
    locker m_mutex;
    bool m_stop;
};

#define LOG_DEBUG(format, ...) {Logger::get_instance()->write_log(Logger::DEBUG, format, ##__VA_ARGS__); Logger::get_instance()->flush();}
#define LOG_INFO(format, ...) {Logger::get_instance()->write_log(Logger::INFO, format, ##__VA_ARGS__); Logger::get_instance()->flush();}
#define LOG_WARN(format, ...) {Logger::get_instance()->write_log(Logger::WARNING, format, ##__VA_ARGS__); Logger::get_instance()->flush();}
#define LOG_ERROR(format, ...) {Logger::get_instance()->write_log(Logger::ERROR, format, ##__VA_ARGS__); Logger::get_instance()->flush();}
#endif