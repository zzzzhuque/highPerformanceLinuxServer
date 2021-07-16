# 半同步/半反应堆线程池实现

## 运行方法

- 创建index.html文件，并放在doc_root变量指向的目录下

- 启动服务器端`./processpool 10.214.128.83 9190`

- 启动客户端在浏览器地址栏输入`http://ip:port/index.html`

若运行正常，浏览器将显示index.html的内容

## 服务器端程序流程梳理

- 服务器端创建threadpool<http_conn>对象，该对象包含10条线程，每个线程处理10000个请求，
其typename为http_conn

- 线程池中的每个线程将worker作为工作函数，通过worker调用run函数，在run函数中判断，当
工作队列不为空时调用process函数处理消息

- 创建http_conn对象指针数组users，预先为每个可能的客户连接分配一个http_conn对象

- 服务器端监听套接字初始化，并进入到listen阶段

- 当有新的客户端连接时初始化users数组中对应位置的http_conn对象

- 当客户端发送消息时，通过`pool->append(users+sockfd);`向工作队列添加http_conn对象
指针

## 显著特点

- 主线程负责监听套接字并accept新的连接，当有新的连接到来时，存储到users数组中

- 当客户端发送新的请求时，存储到工作队列，通过信号量和互斥量随机唤起线程处理请求
