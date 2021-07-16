# 半同步/半异步进程池实现

## 运行方法

- 编译execgi.cpp为execgi可执行文件，并与进程池可执行文件放在同一目录下

- 启动服务器端`./processpool 10.214.128.83 9190`

- 启动客户端`./client 10.214.128.83 9190`

若运行正常，客户端输出如下
```bash
connected......
message from server: test CGI service!
message from server: test CGI service!
```

## 服务器端程序流程梳理

- 服务器端监听套接字初始化，并进入到listen阶段

- 根据单例模式创建进程池对象，其typename为cgi_conn

- 每个子进程都拥有一个processpool，根据`m_idx`区分
主进程(-1)和子进程(>=0)

- 调用pool->run()主进程运行`run_parent`，子进程
运行`run_child`

## 显著特点

- 主进程负责监听套接字，当有新的连接到来时，通过round robin将连接分配给子进程

- 在子进程完成客户端连接的accept，由客户端监听连接套接字的消息