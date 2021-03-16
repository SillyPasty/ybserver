# 轻量级C++http server
- 实现了监听指定端口，解析http请求功能
## 涉及技术
- epoll ET模式
- Proactor模式
- 异步日志队列
- 时间堆计时器
- 线程池
- 数据库连接池
## 已有实现
- GET指定html页面
- 简单登录和注册
## TODO
- 压测
- 针对c++11 特性优化
- 定时关闭连接功能
- http请求函数注册功能
- LT支持
- Reactor支持
