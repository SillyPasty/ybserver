#include "../log/logger.h"
#include "../dao/connection_pool.h"
#include <unistd.h>
#include <string>
int main()
{
    connection_pool *cpp = connection_pool::GetInstance();
    std::string url = "cdb-iien6iby.cd.tencentcdb.com";
    int port = 10115;
    std::string user = "root";
    std::string password = "semester07";
    std::string db_name = "test";
    cpp->init(url, user, password, db_name, port, 8, 1);
    MYSQL *conn = cpp->get_connection();
    char *query_q = "SELECT * FROM test_table";
    char *query_ins_q = "INSERT INTO test_table(id, value) VALUES (2, 'value1')";
    mysql_query(conn, query_ins_q);
    MYSQL_RES *res;
    if (!(res = mysql_store_result(conn))) //获得sql语句结束后返回的结果集
    {
        printf("Couldn't get result from %s\n", mysql_error(conn));
        return false;
    }
    int af_rows = mysql_affected_rows(conn);
    printf("number of dataline returned: %d\n", af_rows);
    char *str_field[32];        //定义一个字符串数组存储字段信息
    for (int i = 0; i < 2; i++) //在已知字段数量的情况下获取字段名
    {
        str_field[i] = mysql_fetch_field(res)->name;
    }
    for (int i = 0; i < 2; i++) //打印字段
        printf("%10s\t", str_field[i]);
    MYSQL_ROW column;
    while (column = mysql_fetch_row(res)) //在已知字段数量情况下，获取并打印下一行
    {
        printf("%10s\t%10s\t", column[0], column[1]); //column是列数组
    }

    cpp->release_connection(conn);
    conn = nullptr;
    delete conn;
    getchar();
    return 0;
}