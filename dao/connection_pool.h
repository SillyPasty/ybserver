#ifndef CONNECTION_POOL_H
#define CONNECTION_POOL_H

#include <stdio.h>
#include <list>
#include <mysql/mysql.h>
#include <error.h>
#include <string.h>
#include <iostream>
#include <string>
#include "../locker/locker.h"
#include "../log/logger.h"

using namespace std;

class connection_pool
{
public:
    MYSQL *get_connection();              //获取数据库连接
    bool release_connection(MYSQL *conn); //释放连接
    int cur_free_conn();                  //获取连接
    void destory_pool();                  //销毁所有连接

    //单例模式
    static connection_pool *GetInstance();

    void init(string url, string user, string passWord, string db_name, int port, int max_conn, int close_log);

private:
    connection_pool();
    ~connection_pool();

    int max_conn;  //最大连接数
    int cur_conn;  //当前已使用的连接数
    int free_conn; //当前空闲的连接数
    locker lock;
    list<MYSQL *> conn_list; //连接池
    cond reserve;

public:
    string m_url;      //主机地址
    int m_port;        //数据库端口号
    string m_user;     //登陆数据库用户名
    string m_password; //登陆数据库密码
    string m_db_name;  //使用数据库名
    int m_close_log;   //日志开关
};

#endif