#include "stdafx.h"
#include "c_socket.h"
#include <stdio.h>
#include <string.h>




//int socket_set_server(unsigned short port, int listen_num)
//{
//    int ret = 0;
//    int socket_fd = 0;
//    struct sockaddr_in server_addr;
//    int flag,len = sizeof(len);
//
//    // ????????????????????
//    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
//    if (-1 == socket_fd)
//    {
//        ret = -errno;
//        printf("Get socket error: %d\n", ret);
//        goto exit;
//    }
//
//    // ????????
//    bzero(&server_addr,sizeof(server_addr));
//    server_addr.sin_family = AF_INET;
//    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
//    server_addr.sin_port = htons(port);
//
//    // ????????????????????????????????
//    if( setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &flag, len) == -1)
//    {
//        perror("setsockopt");
//        goto exit;
//    }
//
//    if (bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr))==-1)
//    {
//        ret = -errno;
//        printf("Bind port(%d) to socket unsuccessfully, error code is: %d\n", port, ret);
//        goto exit;
//    }
//
//    //????????????????????????
//    if (listen(socket_fd, listen_num) == -1)
//    {
//        ret = -errno;
//        printf("Listen unsuccessfully in port(%d), error code is: %d\n", port, ret);
//        goto exit;
//    }
//    ret = socket_fd;
//
//exit:
//    return ret;
//}
//
//
//
//
//int socket_accept(int socket_server)
//{
//    int ret = 0;
//    int tmp_flag;
//    int client_socket;
//    socklen_t sockaddr_len = sizeof(struct sockaddr);
//    struct sockaddr their_addr;
//
//    fd_set fds;
//
//    // ????????????????????????
//    tmp_flag = fcntl(socket_server, F_GETFL);
//    fcntl(socket_server, F_SETFL, tmp_flag | O_NONBLOCK);
//
//    client_socket = accept(socket_server, &their_addr, &sockaddr_len);
//    if (-1 == client_socket)
//    {
//        FD_ZERO(&fds);
//        FD_SET(socket_server, &fds);
//        FD_SET(0, &fds);
//        FD_SET(1, &fds);
//        ret = select(socket_server + 1, &fds, NULL, NULL, NULL);
//        if (ret <= 0)
//        {
//            printf("select is error, error code is %d.\n", errno);
//            ret = -errno;
//            goto exit;
//        }
//
//        if (FD_ISSET(socket_server, &fds))
//        {
//
//            client_socket = accept(socket_server, &their_addr, &sockaddr_len);
//            printf("accept socket is %d.\n", client_socket);
//        }
//    }
//    ret = client_socket;
//exit:
//    return ret;
//}


int socket_write(SOCKET socket_fd, std::string strMsg)
{
    int ret = 0;
    int send_len = 0;
    char len[4];

	//由于消息包大小可变，先获取消息包长度，发送给接收端，四个字节存消息包大小
	//send_len = strMsg.length();
	//itoa(send_len,len,10);
	//ret = send(socket_fd,len,4,0);
	//if (ret<0)
	//{
	//	goto exit;
	//}
	ret = send(socket_fd,strMsg.c_str(),200,0);
	if (ret<0)
	{
		goto exit;
	}
	
	ret = 0;
exit:
    return ret;
}



int socket_read(SOCKET socket_fd, std::string &strMsg)
{
    int ret = 0;
    int recv_len = 0;
	char buf_len[4];
	char data[1024];
	
	//ret = recv(socket_fd,buf_len,4,0);
	//if (ret<0)
	//{
	//	goto exit;
	//}	
	//recv_len += ret;
	
	ret = recv(socket_fd,data,200,0);
	if (ret<0)
	{
		goto exit;
	}
	//data[atoi(buf_len)] = '\0';
	strMsg = data;
	recv_len += ret;
	ret = recv_len;

exit:
    return ret;
}


void socket_close(SOCKET socket_fd)
{
    closesocket(socket_fd);
    Sleep(2);
}


//int socket_connect(const char * ip, unsigned short port)
//{
//    int ret = 0;
//    int err = 0;
//
//    int socket_fd = 0;
//    int tmp_flag = 0;
//
//    socklen_t socket_len = sizeof(err);
//    struct sockaddr_in server_addr;
//
//    fd_set fds;
//
//    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
//    if (-1 == socket_fd)
//    {
//        ret = -errno;
//        printf("Get socket error: %d\n", -errno);
//        goto exit;
//    }
//
//
//    // nonblock
//    tmp_flag = fcntl(socket_fd, F_GETFL);
//    fcntl(socket_fd, F_SETFL, tmp_flag | O_NONBLOCK);
//
//    bzero(&(server_addr.sin_zero), 8);
//    server_addr.sin_family = AF_INET;
//    server_addr.sin_port = htons(port);
//    server_addr.sin_addr.s_addr = inet_addr(ip);
//
//    if (connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
//    {
//        if (EINPROGRESS != errno)
//        {
//            ret = -errno;
//            printf("Enter connect progress unsuccessfully, error code is : %d.\n", ret);
//            goto exit;
//        }
//
//
//        FD_ZERO(&fds);
//        FD_SET(socket_fd, &fds);
//        ret = select(socket_fd + 1, NULL, &fds, NULL, NULL);
//        if (ret < 0)
//        {
//            ret = -errno;
//            printf("Select error, error code is : %d.\n", ret);
//            goto exit;
//        }
//
//        if (0 == ret)
//        {
//            printf("Select timeout.ret = %d\n", ret);
//        }
//
//        getsockopt(socket_fd, SOL_SOCKET, SO_ERROR, &err, &socket_len);
//        if (err != 0)
//        {
//            ret = -errno;
//            printf("Connect error, error code is : %d.\n", ret);
//            goto exit;
//        }
//    }
//
//    ret = socket_fd;
//
//exit:
//    if (ret < 0 && socket_fd != -1)
//    {
//        close(socket_fd);
//    }
//
//    return ret;
//}



