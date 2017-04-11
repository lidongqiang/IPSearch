#ifndef C_SOCKET_H
#define C_SOCKET_H
#include <iostream>

/* 套接字操作集 */
// 设置服务器套接字
//int socket_set_server(unsigned short port, int listen_num);
// 接受请求
//int socket_accept(int socket_server);
// 发送
int socket_write(SOCKET socket_fd, std::string strMsg);
// 读
int socket_read(SOCKET socket_fd, std::string &strMsg);
// 关闭套接字
void socket_close(SOCKET socket_fd);
// 连接服务器
//int socket_connect(const char * ip, unsigned short port);



#endif
