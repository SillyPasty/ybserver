#include "logger.h"

Logger::Logger() {

}

Logger::~Logger() {
    m_stop = true;
}

void Logger::init(const char *file_name, int write_buffer_size, int max_line, int bq_size) {
    if (is_init)
        return;
    is_init = true;
    f_name = std::string(file_name);
    write_buf_size = write_buffer_size;
    max_line_perfile = max_line;
    msg_queue = BlockQueue<std::string>(bq_size);
    m_stop = false;
    m_mutex.lock();
    new_log_file(true);
    m_mutex.unlock();
    if(pthread_create(&write_thread, nullptr, flush_log_thread, nullptr)) {
        throw std::exception();
    }
    if (pthread_detach(write_thread)) {
        throw std::exception();
    }
}

void Logger::write_log(LOG_LEVEL log_level, const char *format, ...) {
    std::vector<std::string> log_info{
        "INFO:",
        "DEBUG:",
        "WARNING:",
        "ERROR:"
    };

    struct timeval now = {0, 0};
    gettimeofday(&now, NULL);
    time_t t = now.tv_sec;
    tm *local_tm = localtime(&t);
    
    int year = 1900 + local_tm->tm_year;
    int month = 1 + local_tm->tm_mon;
    int day = local_tm->tm_mday;
    int hour = local_tm->tm_hour;
    int min = local_tm->tm_min;
    int sec = local_tm->tm_sec;
    int usec = now.tv_usec;
    char info_str[50];
    char *info_fmt = "%04d-%02d-%02d_%02d:%02d:%02d.%06d %s";
    sprintf(info_str, info_fmt, year, month, day, hour, min, sec, usec, log_info[log_level].c_str());
    char write_buffer[write_buf_size] = {0};
    va_list valst;
    va_start(valst, format);
    vsprintf(write_buffer, format, valst);
    
    std::string header(info_str), info(write_buffer);
    info += "\n";
    header += info;
    // std::cout << header;
    msg_queue.push(header);
}

void Logger::flush() {
    m_mutex.lock();
    if (cur_ofs)
        cur_ofs.flush();
    m_mutex.unlock();
}

void Logger::new_log_file(bool is_new_day) {
    // m_mutex.lock();
    if (cur_ofs.is_open()) {
        cur_ofs.flush();
        cur_ofs.close();
    }
    time_t now = time(0);
    tm *local_tm = localtime(&now);

    int year = 1900 + local_tm->tm_year;
    int month = 1 + local_tm->tm_mon;
    int day = local_tm->tm_mday;
    char file_name[f_name.size() + 30] = {0};
    ++log_per_day;
    if (is_new_day) {
        log_per_day = 1;
        cur_day = day;
        std::cout << cur_day;
    }
    cur_line = 0;
    sprintf(file_name, "%s_%04d_%02d_%02d_No_%d.log", f_name.c_str(), year, month, day, log_per_day);
    cur_ofs.open(std::string(file_name), std::ios::out);
    // m_mutex.unlock();
}


void *Logger::flush_log_thread(void *args) {
    Logger::get_instance()->async_write();
}

void *Logger::async_write() {
    std::string context;
    while (!m_stop) {
        context = msg_queue.pop(100);
        if (context != "") {
            m_mutex.lock();
            time_t now = time(0);
            tm *local_tm = localtime(&now);
            int day, tmp1, tmp2;
            const char *tmp = context.c_str();
            sscanf(tmp, "%d-%d-%d", &tmp1, &tmp2, &day);
            cur_day = 20;
            if (day != cur_day) {
                std::cout << "new_f" << std::endl;
                new_log_file(true);
            }
            ++cur_line;
            cur_line = 2000000;
            if (cur_line >= max_line_perfile) {
                new_log_file(false);
            }
            cur_ofs << context;
            m_mutex.unlock();
            // std::cout << "after flush!";
        }
    }
}