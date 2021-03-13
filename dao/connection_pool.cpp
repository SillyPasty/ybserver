#include "connection_pool.h"

using namespace std;

connection_pool::connection_pool()
{
    cur_conn = 0;
    free_conn = 0;
}

connection_pool::~connection_pool()
{
    destory_pool();
}

connection_pool *connection_pool::GetInstance()
{
    static connection_pool conn_pool;
    return &conn_pool;
}

//构造初始化
void connection_pool::init(string url, string user, string password, string db_name, int port, int max_conn, int close_log)
{
    m_url = url;
    m_port = port;
    m_user = user;
    m_password = password;
    m_db_name = db_name;
    m_close_log = close_log;

    for (int i = 0; i < max_conn; i++)
    {
        MYSQL *con = NULL;
        con = mysql_init(con);

        if (con == nullptr)
        {
            // LOG_ERROR("MySQL Error");
            exit(1);
        }
        con = mysql_real_connect(con, url.c_str(), user.c_str(), password.c_str(), db_name.c_str(), port, NULL, 0);

        if (con == nullptr)
        {
            // LOG_ERROR("MySQL Error");
            exit(1);
        }
        conn_list.push_back(con);
        ++free_conn;
    }

    reserve = cond();

    max_conn = free_conn;
}

//当有请求时，从数据库连接池中返回一个可用连接，更新使用和空闲连接数
MYSQL *connection_pool::get_connection()
{
    MYSQL *con = nullptr;

    lock.lock();
    if (conn_list.size() == 0)
        reserve.wait(lock.get());
    con = conn_list.front();
    conn_list.pop_front();

    --free_conn;
    ++cur_conn;

    lock.unlock();
    return con;
}

//释放当前使用的连接
bool connection_pool::release_connection(MYSQL *con)
{
    if (nullptr == con)
        return false;

    lock.lock();

    conn_list.push_back(con);
    ++free_conn;
    --cur_conn;

    reserve.signal();
    lock.unlock();

    return true;
}

//销毁数据库连接池
void connection_pool::destory_pool()
{

    lock.lock();
    for (auto conn = conn_list.begin(); conn != conn_list.end(); ++conn)
    {
        mysql_close(*conn);
    }
    cur_conn = 0;
    free_conn = 0;
    conn_list.clear();
    lock.unlock();
}

//当前空闲的连接数
int connection_pool::cur_free_conn()
{
    return free_conn;
}